// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameTopUI.h"
#include <UMG/Public/Components/TextBlock.h>

void UInGameTopUI::NativeConstruct()
{
	Super::NativeConstruct();

	PlayersCountTxt = Cast<UTextBlock>(GetWidgetFromName(TEXT("PlayersCountTxt")));
}

void UInGameTopUI::UpdatePlayerCountTxt(int32 playerCount)
{
	FString countString = FString::Printf(TEXT("%d"), playerCount);
	PlayersCountTxt->SetText(FText::FromString(countString));
}
