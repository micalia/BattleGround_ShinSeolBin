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

	// �� ���̷�Ż�޽� ������Ʈ ���
	//gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));

	////// �θ� ������Ʈ�� Mesh ������Ʈ�� ����
	//gunMeshComp->SetupAttachment(GetMesh(), TEXT("gun_fire_button"));

	////// ���̷�Ż�޽� ������ �ε�
	//ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPS_Weapon_Bundle/Weapons/Meshes/AR4/SK_AR4.SK_AR4'"));

	////// ������ �ε尡 �����ߴٸ�, �����Ͱ� �����Ѵٸ�
	//if (TempGunMesh.Succeeded())
	//{
	//	UE_LOG(LogTemp,Warning,TEXT("Succeeded2"));
	//	// ���̷�Ż�޽� ������ �Ҵ�
	//	gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
	//	// ��ġ�� �������Ʈ���� �����Ѵ�. 
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
		InteractWidget->ItemNameCanvas->SetRenderOpacity(0);
	}
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
														UEngineTypes::ConvertToTraceType								(ECollisionChannel::ECC_GameTraceChannel7),
														false,
														ActorsToIgnore,
														EDrawDebugTrace::ForOneFrame,
														OutHit,
														true);

	if (bHit) {
		if (AMasterItem* MasterItem = Cast<AMasterItem>(OutHit.GetActor())) {
			InteractWidget->ItemName->SetText(FText::FromString(MasterItem->ItemData.Name));
		}
	}
}
