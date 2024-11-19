// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameTopUI.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGROUND_API UInGameTopUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* PlayersCountTxt = nullptr;

	void UpdatePlayerCountTxt(int32 playerCount);
};
