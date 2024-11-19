// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_EnemyHP.h"
#include <UMG/Public/Components/TextBlock.h>


void UUI_EnemyHP::NativeConstruct()
{
	Super::NativeConstruct();

	enemyHP = Cast<UTextBlock>(GetWidgetFromName(TEXT("enemyHP")));
}

void UUI_EnemyHP::UpdateCurrHP(float currHP, float maxHP)
{
	FString strHP = FString::Printf(TEXT("%d%/%d"), (int32)(currHP), (int32)(maxHP));
	enemyHP->SetText(FText::FromString(strHP));
}

