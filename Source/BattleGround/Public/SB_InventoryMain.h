// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SB_InventoryMain.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGROUND_API USB_InventoryMain : public UUserWidget
{
	GENERATED_BODY()
	
public:
	USB_InventoryMain(const FObjectInitializer &ObjectInitializer);

	virtual void NativeConstruct();

public:

	UFUNCTION(BlueprintCallable)
	void BuildInventory();

	UFUNCTION(BlueprintCallable)
	void BuildGroundItems();

	UFUNCTION(BlueprintImplementableEvent)
	void SetCurrWeightText();

public:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	class UScrollBox* ItemScrollbox;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	class UScrollBox* GroundItems;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UBorder* GroundDropBoxPanel;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UBorder* InvDropBoxPanel;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UBorder* CancelBoxPanel;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class USB_EquipSlotWidget* EquipHelmetSlot;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class USB_EquipSlotWidget* EquipUpperWearSlot;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class USB_EquipSlotWidget* EquipLowerWearSlot;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class USB_EquipSlotWidget* EquipShoesSlot;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class USB_GunSlotWidget* GunSlot1;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class USB_GunSlotWidget* GunSlot2;

	class ABattleGroundCharacter* Player;
	
	UPROPERTY(BlueprintReadWrite)
	class UUserWidget* ItemSlotHover;
};
