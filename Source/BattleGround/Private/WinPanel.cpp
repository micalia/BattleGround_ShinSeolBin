// Fill out your copyright notice in the Description page of Project Settings.


#include "WinPanel.h"
#include <UMG/Public/Components/CanvasPanel.h>

void UWinPanel::NativeConstruct()
{
	Super::NativeConstruct();
	winPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("WinPanel")));
}
