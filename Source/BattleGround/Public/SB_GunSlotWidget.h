// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MasterItem.h"
#include "SB_EquipSlotWidget.h"
#include "SB_GunSlotWidget.generated.h"
/**
 * 
 */
UCLASS()
class BATTLEGROUND_API USB_GunSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, meta=(BindWidget), BlueprintReadWrite)
	class UImage* GunImage = nullptr;
	UPROPERTY(EditAnywhere, meta = (BindWidget), BlueprintReadWrite)
	class UTextBlock* GunName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemData ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SlotNum = 0;
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTexture2D> noWeaponImage;

	UFUNCTION(BlueprintCallable)
	void SetDefaulImage();

	UPROPERTY()
	class UTexture2D* noWeaponTexture;
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void NotEquipState(int32 InGunSlotIndex);
};
