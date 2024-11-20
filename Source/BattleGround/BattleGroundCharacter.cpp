// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleGroundCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include <Components/SkeletalMeshComponent.h>
#include "Bullet.h"
#include <Components/SceneComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/StaticMesh.h>
#include <Kismet/GameplayStatics.h>
#include "InteractWidget.h"
#include "UMG/Public/Components/CanvasPanel.h"
#include "Camera/PlayerCameraManager.h"
#include "MasterItem.h"
#include "UMG/Public/Components/TextBlock.h"
#include "Kismet/KismetArrayLibrary.h"

//////////////////////////////////////////////////////////////////////////
// ABattleGroundCharacter

ABattleGroundCharacter::ABattleGroundCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	/*bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;*/

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// 총 스켈레탈메시 컴포넌트 등록
	//gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));

	////// 부모 컴포넌트를 Mesh 컴포넌트로 설정
	//gunMeshComp->SetupAttachment(GetMesh(), TEXT("gun_fire_button"));

	////// 스켈레탈메시 데이터 로드
	//ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPS_Weapon_Bundle/Weapons/Meshes/AR4/SK_AR4.SK_AR4'"));

	////// 데이터 로드가 성공했다면, 데이터가 존재한다면
	//if (TempGunMesh.Succeeded())
	//{
	//	UE_LOG(LogTemp,Warning,TEXT("Succeeded2"));
	//	// 스켈레탈메시 데이터 할당
	//	gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
	//	// 위치는 블루프린트에서 조정한다. 
	//	gunMeshComp->SetRelativeLocationAndRotation(FVector(-8.286, 4.083, 0.322), FRotator(-0.633	, -13.0365 , 178.53));
	//}

	//firePos = CreateDefaultSubobject<USceneComponent>(TEXT("FirePos"));
	//firePos->SetupAttachment(gunMeshComp);
	//firePos->SetRelativeLocation(FVector(0, 65, 15));
	//firePos->SetRelativeRotation(FRotator(0, 90, 0));

	static ConstructorHelpers::FClassFinder<UInteractWidget> tempInteractWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Inventory/WB_InteractWidget.WB_InteractWidget_C'"));
	if (tempInteractWidget.Succeeded()) {
		InteractWidgetFactory = tempInteractWidget.Class;
	}
}

void ABattleGroundCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	currHp = FullHp;

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	InteractWidget = CreateWidget<UInteractWidget>(GetWorld(), InteractWidgetFactory);
	InteractWidget->AddToViewport(0);
	InteractWidget->ItemNameCanvas->SetRenderOpacity(0);
}

void ABattleGroundCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetOverlapItemCount() > 0) {
		TraceItem();
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABattleGroundCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABattleGroundCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABattleGroundCharacter::Look);

		//Fire
		PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ABattleGroundCharacter::InputFire);
		PlayerInputComponent->BindAction(TEXT("InputF"), IE_Pressed, this, &ABattleGroundCharacter::InputFKey);
	}

}

void ABattleGroundCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABattleGroundCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABattleGroundCharacter::InputFire()
{
	//FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("testPos"));
	/*FTransform firePosition = firePos->GetComponentTransform();
	GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);*/
}

void ABattleGroundCharacter::SetOverlapItemCount(int32 InOverlapItemCnt)
{
	OverlapItemCnt = OverlapItemCnt + InOverlapItemCnt;
	if (OverlapItemCnt > 0) {
		InteractWidget->ItemNameCanvas->SetRenderOpacity(1);
	}
	else {
		tempItem = nullptr;
		InteractWidget->ItemNameCanvas->SetRenderOpacity(0);
	}
}

void ABattleGroundCharacter::InputFKey()
{
	GEngine->AddOnScreenDebugMessage(-1, 999, FColor::Purple, FString::Printf(TEXT("%s >> InputFKey"), *FDateTime::UtcNow().ToString(TEXT("%H:%M:%S"))), true, FVector2D(1.5f, 1.5f));
	if (tempItem != nullptr) {
		if (ItemToInventory(tempItem)) {
			if (InventoryRef != nullptr) {
				InventoryRef->BuildInventory();
				InventoryRef->BuildGroundItems();
			}
		}
	}
}

bool ABattleGroundCharacter::ItemToInventory(AMasterItem* InItem)
{
	if (InItem) {
		int32 InItemWeight = (int32)(InItem->ItemData.Weight * InItem->ItemData.Amount);
	
		if ((InItemWeight + GetInvenItemWeight()) <= MaxWeight) {
			if (InItem->ItemData.IsStackAble) {
				if (HasItemOnce(InItem)) {
					IncreaseAmount(InItem, FindIndex(InItem));
					return true;
				}
				else {
					return AddItem(InItem);
				}
			}
			else {
				return AddItem(InItem);
			}
		}

	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 999, FColor::Purple, FString::Printf(TEXT("%s >> InItem is Null"), *FDateTime::UtcNow().ToString(TEXT("%H:%M:%S"))), true, FVector2D(1.5f, 1.5f));
	}
	return false;
}

bool ABattleGroundCharacter::AddItem(AMasterItem* InItem)
{
	ItemArr.Add(InItem->ItemData);
	if (InItem->ItemData.Category == EItemEnum::Consumeables) {
		InItem->Destroy();
	}
	else {
		InItem->Destroy();
		// TODO: 같은 이름의 아이템이 없으므로 장착
	}
	return true;
}

int32 ABattleGroundCharacter::GetInvenItemWeight()
{
	int32 CurrentWeight = 0;
	for (auto& Item : ItemArr)
	{
		float ItemWeight = Item.Weight * Item.Amount;
		CurrentWeight += ItemWeight;
	}
	return CurrentWeight;	 
}

int32 ABattleGroundCharacter::FindIndex(AMasterItem* InItem)
{
	for (int i = 0; i < ItemArr.Num(); i++)
	{
		if (ItemArr[i].Name == InItem->ItemData.Name) {
			return i;
		}
	}
	return -1;
}

void ABattleGroundCharacter::IncreaseAmount(AMasterItem* InItem, int32 Index)
{
	FItemData ItemData;
	
	ItemData = ItemArr[Index];
	ItemData.Amount += InItem->ItemData.Amount;

	ItemArr[Index] = ItemData;
	InItem->Destroy();
}

bool ABattleGroundCharacter::HasItemOnce(AMasterItem* InItem)
{
	bool bEqual = false;
	for (auto Item : ItemArr) {
		if (InItem->ItemData.Name == Item.Name) {
			bEqual = true;
			break;
		}
	}
	return bEqual;
}

void ABattleGroundCharacter::TraceItem()
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector ViewportWorldPosition;
	FVector ViewportWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		ViewportCenter,
		ViewportWorldPosition,
		ViewportWorldDirection);

	FVector Start = ViewportWorldPosition;
	FVector End = Start + ViewportWorldDirection * TraceMaxDistance;
	
	FHitResult OutHit;
	TArray<AActor*> ActorsToIgnore;
	
	bool bHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(),
														Start, 
														End, 
														50, 
														50, 
														UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel7),
														false,
														ActorsToIgnore,
														EDrawDebugTrace::ForOneFrame,
														OutHit,
														true);

	if (bHit) {
		if (AMasterItem* MasterItem = Cast<AMasterItem>(OutHit.GetActor())) {
			tempItem = MasterItem;
			InteractWidget->ItemName->SetText(FText::FromString(MasterItem->ItemData.Name));
		}
	}
}
