// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MasterItem.h"
#include "SB_InventoryMain.h"
#include "BattleGroundCharacter.generated.h"

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
	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* SK_Pants;



public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FullHp = 50;
	UPROPERTY(BlueprintReadWrite)
	float currHp = 0;

public:
	void InputFire();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UInteractWidget> InteractWidgetFactory;

	UPROPERTY()
	class UInteractWidget* InteractWidget;

	UPROPERTY(EditAnywhere)
	float TraceMaxDistance = 500;

	void SetOverlapItemCount(int32 InOverlapItemCnt);
	FORCEINLINE int32 GetOverlapItemCount() { return OverlapItemCnt; }

	void TraceItem();

	UPROPERTY()
	AMasterItem* tempItem;

	UFUNCTION()
	void InputFKey();

	UPROPERTY(BlueprintReadWrite)
	TArray<FItemData> ItemArr;

	UPROPERTY(BlueprintReadWrite)
	class USB_InventoryMain* InventoryRef;

	UFUNCTION(BlueprintCallable)
	bool ItemToInventory(AMasterItem* InItem);

	UPROPERTY(BlueprintReadWrite)
	TArray<class AMasterItem*> MultiItemRefs;


private:
	bool AddItem(AMasterItem* InItem);
	void PushHelmetToInven(AMasterItem* InItem);
	int32 GetInvenItemWeight();
	int32 FindIndex(AMasterItem* InItem);
	void IncreaseAmount(AMasterItem* InItem, int32 Index);
	bool HasItemOnce(AMasterItem* InItem);

private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bUseHelmet = false;

	int32 MaxWeight = 40;
	int32 OverlapItemCnt;
};

