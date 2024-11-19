// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_EnemyHP.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGROUND_API UUI_EnemyHP : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void UpdateCurrHP(float currHP, float maxHP);


	UPROPERTY(meta=(BindWidget))
	class UTextBlock* enemyHP;
};
