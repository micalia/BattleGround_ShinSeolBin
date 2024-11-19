// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LosePanel.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGROUND_API ULosePanel : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* losePanel = nullptr;
};
