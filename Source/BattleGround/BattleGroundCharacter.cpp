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
#include "Animation/AnimInstance.h"
#include "SB_GunSlotWidget.h"

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

	static ConstructorHelpers::FClassFinder<UInteractWidget> tempInteractWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Inventory/WB_InteractWidget.WB_InteractWidget_C'"));
	if (tempInteractWidget.Succeeded()) {
		InteractWidgetFactory = tempInteractWidget.Class;
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> tempManABPClass(TEXT("/Script/Engine.AnimBlueprint'/Game/Man/Animation/ABP/ABP_Man.ABP_Man_C'"));
	if (tempManABPClass.Succeeded()) {
		ManABPClass = tempManABPClass.Class;
	}

	ConstructorHelpers::FObjectFinder<USoundBase> tempPickUpSound(TEXT("/Script/Engine.SoundWave'/Game/Sounds/EquipItem.EquipItem'"));
	if (tempPickUpSound.Succeeded()) {
		PickUpSound = tempPickUpSound.Object;
	}

	SM_Helmet = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Helmet"));
	SM_Helmet->SetupAttachment(GetMesh(), TEXT("headSocket"));

	SM_Weapon1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Weapon1"));
	SM_Weapon1->SetupAttachment(GetMesh(), TEXT("GunSlot1"));
	SM_Weapon2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Weapon2"));
	SM_Weapon2->SetupAttachment(GetMesh(), TEXT("GunSlot2"));

	SK_UpperWear = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SK_UpperWear"));
	SK_UpperWear->SetupAttachment(GetMesh());

	SK_LowerWear = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SK_LowerWear"));
	SK_LowerWear->SetupAttachment(GetMesh());

	SK_Shoes = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SK_Shoes"));
	SK_Shoes->SetupAttachment(GetMesh());

	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempAM_DrawGun(TEXT("/Script/Engine.AnimMontage'/Game/Animations/Player/Montages/AM_DrawGun.AM_DrawGun'"));
	if (tempAM_DrawGun.Succeeded())
	{
		AM_DrawGun = tempAM_DrawGun.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempAM_PickUp(TEXT("/Script/Engine.AnimMontage'/Game/Animations/Player/Montages/AM_PickUp.AM_PickUp'"));
	if (tempAM_PickUp.Succeeded())
	{
		AM_PickUp = tempAM_PickUp.Object;
	}

	static ConstructorHelpers::FClassFinder<AMasterItem> tempMasterItemFactory(TEXT("/Script/Engine.Blueprint'/Game/Inventory/BP_MasterItem.BP_MasterItem_C'"));
	if (tempMasterItemFactory.Succeeded()) {
		MasterItemFactory = tempMasterItemFactory.Class;
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
		PlayerInputComponent->BindAction(TEXT("Input1"), IE_Pressed, this, &ABattleGroundCharacter::Input1Key);
		PlayerInputComponent->BindAction(TEXT("Input2"), IE_Pressed, this, &ABattleGroundCharacter::Input2Key);
		PlayerInputComponent->BindAction(TEXT("InputX"), IE_Pressed, this, &ABattleGroundCharacter::InputXKey);
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
		ItemToInventory(tempItem);
	}
}

void ABattleGroundCharacter::Input1Key()
{
	GEngine->AddOnScreenDebugMessage(-1, 999, FColor::Purple, FString::Printf(TEXT("%s >> Input1Key"), *FDateTime::UtcNow().ToString(TEXT("%H:%M:%S"))), true, FVector2D(1.5f, 1.5f));
	if (bEquippingWeapon1) {
		ChangeAttackState(EAttackState::Weapon1);
	}
}

void ABattleGroundCharacter::Input2Key()
{
	GEngine->AddOnScreenDebugMessage(-1, 999, FColor::Purple, FString::Printf(TEXT("%s >> Input2Key"), *FDateTime::UtcNow().ToString(TEXT("%H:%M:%S"))), true, FVector2D(1.5f, 1.5f));
	if (bEquippingWeapon2) {
		ChangeAttackState(EAttackState::Weapon2);
	}
}

void ABattleGroundCharacter::InputXKey()
{
	GEngine->AddOnScreenDebugMessage(-1, 999, FColor::Purple, FString::Printf(TEXT("%s >> InputXKey"), *FDateTime::UtcNow().ToString(TEXT("%H:%M:%S"))), true, FVector2D(1.5f, 1.5f));
	if (bEquippingWeapon1 || bEquippingWeapon2) {
		ChangeAttackState(EAttackState::NoWeapon);
	}
}

void ABattleGroundCharacter::ItemToInventory(AMasterItem* InItem)
{
	PlayAnimMontage(AM_PickUp, 1.3, TEXT("PickUp"));

	GetWorldTimerManager().ClearTimer(PickUpItemDelay);
	GetWorld()->GetTimerManager().SetTimer(PickUpItemDelay, FTimerDelegate::CreateLambda([&, InItem]() {
		if (InItem) {
			int32 InItemWeight = (int32)(InItem->ItemData.Weight * InItem->ItemData.Amount);
			bool bPushResult = false;
			if ((InItemWeight + GetInvenItemWeight()) <= MaxWeight) {
				if (InItem->ItemData.IsStackAble) {
					if (HasItemOnce(InItem)) {
						IncreaseAmount(InItem, FindIndex(InItem));
					}
					else {
						bPushResult = AddItem(InItem);
					}
				}
				else {
					bPushResult = AddItem(InItem);
				}
			}

			if (bPushResult) {
				if (InventoryRef != nullptr) {
					InventoryRef->BuildInventory();
					InventoryRef->BuildGroundItems();
				}

				UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
			}
		}
	}), PickUpItemDelayTime, false);
}

void ABattleGroundCharacter::ChangeAttackState(EAttackState InAttackState)
{
	GEngine->AddOnScreenDebugMessage(-1, 999, FColor::Purple, FString::Printf(TEXT("%s >> This iS Call?"), *FDateTime::UtcNow().ToString(TEXT("%H:%M:%S"))), true, FVector2D(1.5f, 1.5f));
	int a = 1;
	switch (InAttackState) {
	case EAttackState::NoWeapon: 
		{
			if (CurrAttackState == EAttackState::Weapon1) {
				FAttachmentTransformRules AttachRule = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
				SM_Weapon1->AttachToComponent(GetMesh(), AttachRule, TEXT("LeftHandSocket"));
				 PlayAnimMontage(AM_DrawGun, 1, TEXT("Draw1Slot"));
			
				GetWorldTimerManager().ClearTimer(DrawGunDelay);
				GetWorld()->GetTimerManager().SetTimer(DrawGunDelay, FTimerDelegate::CreateLambda([&]() {
					FAttachmentTransformRules AttachRule = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
					SM_Weapon1->AttachToComponent(GetMesh(), AttachRule, TEXT("GunSlot1"));
				}), DrawGunDelayTime, false);
			}else if (CurrAttackState == EAttackState::Weapon2) {
				PlayAnimMontage(AM_DrawGun, 1, TEXT("Draw2Slot"));
				
				GetWorldTimerManager().ClearTimer(DrawGunDelay);
				GetWorld()->GetTimerManager().SetTimer(DrawGunDelay, FTimerDelegate::CreateLambda([&]() {
					FAttachmentTransformRules AttachRule = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
					SM_Weapon2->AttachToComponent(GetMesh(), AttachRule, TEXT("GunSlot2"));
				}), DrawGunDelayTime, false);
			}

			IsArmed = false;
			CurrAttackState = EAttackState::NoWeapon;
		}
		break;
		case EAttackState::Weapon1:
		{
			PlayAnimMontage(AM_DrawGun, 1, TEXT("Draw1Slot"));

			GetWorldTimerManager().ClearTimer(DrawGunDelay);
			GetWorld()->GetTimerManager().SetTimer(DrawGunDelay, FTimerDelegate::CreateLambda([&]() {
				if (CurrAttackState == EAttackState::Weapon2) {
					FAttachmentTransformRules AttachRule = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
					SM_Weapon2->AttachToComponent(GetMesh(), AttachRule, TEXT("GunSlot2"));
				}
				FAttachmentTransformRules AttachRule = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
				SM_Weapon1->AttachToComponent(GetMesh(), AttachRule, TEXT("HoldGunSocket"));
				IsArmed = true;
				CurrAttackState = EAttackState::Weapon1;
			}), DrawGunDelayTime, false);
		}
		break;
		case EAttackState::Weapon2:
		{
			PlayAnimMontage(AM_DrawGun, 1, TEXT("Draw2Slot"));

			GetWorldTimerManager().ClearTimer(DrawGunDelay);
			GetWorld()->GetTimerManager().SetTimer(DrawGunDelay, FTimerDelegate::CreateLambda([&]() {
				if (CurrAttackState == EAttackState::Weapon1) {
					FAttachmentTransformRules AttachRule = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
					SM_Weapon1->AttachToComponent(GetMesh(), AttachRule, TEXT("GunSlot1"));
				}
				FAttachmentTransformRules AttachRule = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
				SM_Weapon2->AttachToComponent(GetMesh(), AttachRule, TEXT("HoldGunSocket"));
				IsArmed = true;
				CurrAttackState = EAttackState::Weapon2;
			}), DrawGunDelayTime, false);
		}
		break;
	}
}

void ABattleGroundCharacter::ToggleBoolEquippingItem(bool& InEquippingVal)
{
	InEquippingVal ? InEquippingVal = false : InEquippingVal = true;
}

bool ABattleGroundCharacter::AddItem(AMasterItem* InItem)
{
	switch(InItem->ItemData.Category){
		case EItemEnum::Consumeables:
			break;
		case EItemEnum::Helmet:
		{
			if (!bEquippingHelmet) {
				ToggleBoolEquippingItem(bEquippingHelmet);
				SM_Helmet->SetStaticMesh(InItem->ItemData.Mesh);
				SM_Helmet->SetRelativeScale3D(InItem->ItemData.StaticMeshScale);
			}
			else {
				// TODO: ¹«±â ½½·ÔÀÌ ²ËÃ¡½À´Ï´Ù.
				return false;
			}
		}
			break;
		case EItemEnum::UpperWear:
		{
			if(!bEquippingUpperWear)
			{
				ToggleBoolEquippingItem(bEquippingUpperWear);
				SK_UpperWear->SetSkeletalMeshAsset(InItem->ItemData.SkeletalMesh);
			}
			else {
				// TODO: ¹«±â ½½·ÔÀÌ ²ËÃ¡½À´Ï´Ù.
				return false;
			}
		}
		break;
		case EItemEnum::LowerWear:
		{
			if (!bEquippingLowerWear)
			{
				ToggleBoolEquippingItem(bEquippingLowerWear);
				SK_LowerWear->SetSkeletalMeshAsset(InItem->ItemData.SkeletalMesh);
			}
			else {
				// TODO: ¹«±â ½½·ÔÀÌ ²ËÃ¡½À´Ï´Ù.
				return false;
			}
		}
		break;
		case EItemEnum::Shoes:
		{
			if (!bEquippingShoes)
			{
				ToggleBoolEquippingItem(bEquippingShoes);
				SK_Shoes->SetSkeletalMeshAsset(InItem->ItemData.SkeletalMesh);
			}
			else {
				// TODO: ¹«±â ½½·ÔÀÌ ²ËÃ¡½À´Ï´Ù.
				return false;
			}
		}
		break;
		case EItemEnum::Weapon:
		{ 
			if (!bEquippingWeapon1)
			{
				ToggleBoolEquippingItem(bEquippingWeapon1);
				SM_Weapon1->SetStaticMesh(InItem->ItemData.Mesh);
				SM_Weapon1->SetRelativeScale3D(InItem->ItemData.StaticMeshScale);
				InventoryRef->GunSlot1->ItemData = InItem->ItemData;
			}
			else if (!bEquippingWeapon2) {
				ToggleBoolEquippingItem(bEquippingWeapon2);
				SM_Weapon2->SetStaticMesh(InItem->ItemData.Mesh);
				SM_Weapon2->SetRelativeScale3D(InItem->ItemData.StaticMeshScale);
				InventoryRef->GunSlot2->ItemData = InItem->ItemData;
			}
			else {
				// TODO: ¹«±â ½½·ÔÀÌ ²ËÃ¡½À´Ï´Ù.
				return false;
			}
		}
		break;
	}

	ItemArr.Add(InItem->ItemData);
	InItem->Destroy();

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

void ABattleGroundCharacter::DropItem(FItemData InItem, int32 InGunSlotIdx)
{
	int idx = RemoveFindIndex(InItem);
	if (InItem.IsStackAble) {
		ItemArr[idx].Amount -= 1;
		if (ItemArr[idx].Amount <= 0) {
			ItemArr.RemoveAt(idx);
		}
	}
	else {
		ItemArr.RemoveAt(idx);
	}

	switch (InItem.Category)
	{
		case EItemEnum::Helmet:
		break;
		case EItemEnum::UpperWear:
		break;
		case EItemEnum::LowerWear:
		break;
		case EItemEnum::Shoes:
		break;
		case EItemEnum::Weapon:
			if (InGunSlotIdx == 1) {
				InventoryRef->GunSlot1->NotEquipState(InGunSlotIdx);
				ToggleBoolEquippingItem(bEquippingWeapon1);
				if (CurrAttackState == EAttackState::Weapon1) {
					ChangeAttackState(EAttackState::NoWeapon);
				}
				SM_Weapon1->SetStaticMesh(nullptr);
			}
			else if (InGunSlotIdx == 2) {
				InventoryRef->GunSlot2->NotEquipState(InGunSlotIdx);
				ToggleBoolEquippingItem(bEquippingWeapon2);
				if (CurrAttackState == EAttackState::Weapon2) {
					ChangeAttackState(EAttackState::NoWeapon);
				}
				SM_Weapon2->SetStaticMesh(nullptr);
			}
		break;
	}

	FVector SpawnPos = GetDropItemSpawnPos();

	FActorSpawnParameters spawnConfig;
	spawnConfig.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnConfig.TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;
	spawnConfig.Owner = this;
	auto doFunc = [=](AActor* ObjectToModify)
		{
			AMasterItem* ObjModify = Cast<AMasterItem>(ObjectToModify);
			if (ObjModify)
			{
				ObjModify->ItemData = InItem;
			}
		};

	spawnConfig.CustomPreSpawnInitalization = doFunc;

	AMasterItem* MasterItem = GetWorld()->SpawnActor<AMasterItem>(MasterItemFactory, SpawnPos, GetActorRotation(), spawnConfig);

}

int32 ABattleGroundCharacter::RemoveFindIndex(FItemData InItem)
{
	int32 RemoveIndex = -1;
	for (int i = 0; i < ItemArr.Num(); i++)
	{
		if (InItem.Name == ItemArr[i].Name) {
			RemoveIndex = i;
			break;
		}
	}
	return RemoveIndex;
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
			InteractWidget->ItemNameCanvas->SetRenderOpacity(1);
			tempItem = MasterItem;
			InteractWidget->ItemName->SetText(FText::FromString(MasterItem->ItemData.Name));
		}
	}
	else {
		InteractWidget->ItemNameCanvas->SetRenderOpacity(0);
	}
}

FVector ABattleGroundCharacter::GetDropItemSpawnPos_Implementation() {
	return FVector(0);
}