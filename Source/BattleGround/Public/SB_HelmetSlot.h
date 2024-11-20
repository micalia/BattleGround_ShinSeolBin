// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MasterItem.h"
#include "SB_HelmetSlot.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGROUND_API USB_HelmetSlot : public UUserWidget
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
	class UImage* HelmetImg;

	UPROPERTY(BlueprintReadWrite)
	int32 Index;

	//UPROPERTY(BlueprintReadWrite)
	//class ABattleGroundCharacter* TPRef;
};
