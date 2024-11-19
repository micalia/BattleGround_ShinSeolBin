// Fill out your copyright notice in the Description page of Project Settings.


#include "LosePanel.h"
#include <UMG/Public/Components/CanvasPanel.h>

void ULosePanel::NativeConstruct()
{
	Super::NativeConstruct();
	losePanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("losePanel")));
}
