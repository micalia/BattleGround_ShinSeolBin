// Fill out your copyright notice in the Description page of Project Settings.


#include "GunWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UGunWidget::NativeConstruct()
{
	Super::NativeConstruct();
	GunImage = Cast<UImage>(GetWidgetFromName(TEXT("GunImage1")));
}

void UGunWidget::SetDefaulImage()
{ 
	if (noWeaponImage) {
		noWeaponTexture = Cast<UTexture2D>(noWeaponImage->StaticClass());
		GunImage->SetBrushFromTexture(noWeaponTexture);
	}
}
