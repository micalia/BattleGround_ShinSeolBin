// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MasterItem.h"
#include "SB_ItemSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGROUND_API USB_ItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	FItemData ItemData;

	UPROPERTY(BlueprintReadWrite)
	int32 Index;

	UPROPERTY(BlueprintReadWrite)
	class ABattleGroundCharacter* TPC;
};
