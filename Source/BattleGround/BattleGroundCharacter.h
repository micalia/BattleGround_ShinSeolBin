// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MasterItem.h"
#include "SB_InventoryMain.h"
#include "BattleGroundCharacter.generated.h"

UENUM(BlueprintType)
enum class EAttackState : uint8
{
	NoWeapon,
	Weapon1,
	Weapon2
};

class AMasterItem;

UCLASS(config = Game)
class ABattleGroundCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds);

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:
	ABattleGroundCharacter();
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SM_Helmet;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SM_Weapon1;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SM_Weapon2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FItemData Weapon1Data;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FItemData Weapon2Data;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* SK_UpperWear;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* SK_LowerWear;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* SK_Shoes;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UAnimInstance> ManABPClass;

	UPROPERTY()
	class UAnimMontage* AM_DrawGun;
	UPROPERTY()
	class UAnimMontage* AM_PickUp;

	FTimerHandle DrawGunDelay;
	float DrawGunDelayTime = 0.4f;

	FTimerHandle PickUpItemDelay;
	float PickUpItemDelayTime = 0.5f;

	UPROPERTY()
	class USoundBase* PickUpSound;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AMasterItem> MasterItemFactory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UUserWidget* CrossHairUI;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FullHp = 1000;
	UPROPERTY(BlueprintReadWrite)
	float currHp = 0;

public:
	void InputFire();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UInteractWidget> InteractWidgetFactory;

	UPROPERTY()
	class UInteractWidget* InteractWidget;

	UPROPERTY(EditAnywhere)
	float TraceMaxDistance = 420;

	void SetOverlapItemCount(int32 InOverlapItemCnt);
	FORCEINLINE int32 GetOverlapItemCount() { return OverlapItemCnt; }

	void TraceItem();

	UPROPERTY()
	AMasterItem* tempItem;

	UFUNCTION()
	void InputFKey();
	UFUNCTION()
	void Input1Key();
	UFUNCTION()
	void Input2Key();
	UFUNCTION()
	void InputXKey();

	UPROPERTY(BlueprintReadWrite)
	TArray<FItemData> ItemArr;

	UPROPERTY(BlueprintReadWrite)
	class USB_InventoryMain* InventoryRef;

	UFUNCTION(BlueprintCallable)
	void ItemToInventory(AMasterItem* InItem);

	UPROPERTY(BlueprintReadWrite)
	TArray<class AMasterItem*> MultiItemRefs;

	UFUNCTION(BlueprintImplementableEvent)
	void AlertAlreadyEquipItem();

public: // **** 장비 아이템 착용 여부 ****//
	
	UPROPERTY(BlueprintReadWrite)
	bool bEquippingHelmet = false;
	UPROPERTY(BlueprintReadWrite)
	bool bEquippingUpperWear = false;
	UPROPERTY(BlueprintReadWrite)
	bool bEquippingLowerWear = false;
	UPROPERTY(BlueprintReadWrite)
	bool bEquippingShoes = false;
	UPROPERTY(BlueprintReadWrite)
	bool bEquippingWeapon1 = false;
	UPROPERTY(BlueprintReadWrite)
	bool bEquippingWeapon2 = false;

	UPROPERTY(BlueprintReadOnly)
	EAttackState CurrAttackState = EAttackState::NoWeapon;
	void ChangeAttackState(EAttackState InAttackState);

	void ToggleBoolEquippingItem(bool& InEquippingVal);

	UPROPERTY(BlueprintReadWrite)
	bool IsArmed = false;

	UFUNCTION(BlueprintCallable)
	int32 GetInvenItemWeight();
private:
	bool AddItem(AMasterItem* InItem);
	int32 FindIndex(AMasterItem* InItem);
	void IncreaseAmount(AMasterItem* InItem, int32 Index);
	bool HasItemOnce(AMasterItem* InItem);

	UFUNCTION(BlueprintCallable)
	void DropItem(FItemData InItem, int32 InGunSlotIdx = 0);

	int32 RemoveFindIndex(FItemData InItem);

public:
	UFUNCTION(BlueprintNativeEvent)
	FVector GetDropItemSpawnPos();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void MultiTrace();

private:
	int32 MaxWeight = 40;
	int32 OverlapItemCnt;

};

