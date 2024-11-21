// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MasterItem.h"
#include "SB_GroundSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGROUND_API USB_GroundSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	FItemData ItemData;

	UPROPERTY(BlueprintReadWrite)
	class ABattleGroundCharacter* TPCRef;

	UPROPERTY(BlueprintReadWrite)
	class AMasterItem* MasterItemRef;

};
