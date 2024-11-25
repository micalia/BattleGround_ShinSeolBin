// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MasterItem.h"
#include "SB_EquipSlotWidget.generated.h"
UENUM(BlueprintType)
enum class EItemEquipSlot : uint8
{
	Helmet,
	Consumeables,
	UpperWear,
	LowerWear,
	Shoes,
	Weapon1,
	Weapon2
};

UCLASS()
class BATTLEGROUND_API USB_EquipSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FItemData ItemData;

	UPROPERTY(BlueprintReadWrite)
	class ABattleGroundCharacter* TPRef;

	UPROPERTY(BlueprintReadWrite)
	class AMasterItem* MasterItemRef;

	UPROPERTY(meta=(BindWidget))
	class UImage* EquipItemImg;

	UPROPERTY(BlueprintReadWrite)
	int32 Index;

public:
	void NotEquipState();
};
