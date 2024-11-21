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

	/*UPROPERTY()
	TSubclassOf<class USB_ItemSlotWidget> ItemSlotFactory;*/

public:
	//void BuildInventory();

	UFUNCTION(BlueprintCallable)
	void BuildInventory();

	UFUNCTION(BlueprintCallable)
	void BuildGroundItems();

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
	class USB_HelmetSlot* UseHelmetSlot;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UImage* EmptyHelmet;

	//UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	//TObjectPtr<class USB_HelmetSlot> GunSlot1;
	//
	//UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	//TObjectPtr<class USB_HelmetSlot> GunSlot2;

	class ABattleGroundCharacter* Player;

};
