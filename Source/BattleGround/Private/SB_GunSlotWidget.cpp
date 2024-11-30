// Fill out your copyright notice in the Description page of Project Settings.


#include "SB_GunSlotWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "UMG/Public/Components/TextBlock.h"

void USB_GunSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	GunImage = Cast<UImage>(GetWidgetFromName(TEXT("GunImage1")));
}

void USB_GunSlotWidget::SetDefaulImage()
{ 
	if (noWeaponImage) {
		noWeaponTexture = Cast<UTexture2D>(noWeaponImage->StaticClass());
		GunImage->SetBrushFromTexture(noWeaponTexture);
	}
}
