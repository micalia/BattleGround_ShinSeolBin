// Fill out your copyright notice in the Description page of Project Settings.


#include "GunWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UGunWidget::NativeConstruct()
{
	Super::NativeConstruct();
	GunImage = Cast<UImage>(GetWidgetFromName(TEXT("GunImage")));

	/*ConstructorHelpers::FObjectFinder<UTexture2D> tempDefaultWeaponImg(TEXT("/Script/Engine.Texture2D'/Game/Inventory/GunIcon/NoWeapon.NoWeapon'"));
	if (tempDefaultWeaponImg.Succeeded()) {
		GunImage->SetBrushFromTexture(tempDefaultWeaponImg.Object);
	}*/
}

void UGunWidget::SetDefaulImage()
{ 
	if (noWeaponImage) {
		noWeaponTexture = Cast<UTexture2D>(noWeaponImage->StaticClass());
		GunImage->SetBrushFromTexture(noWeaponTexture);
	//	//GunImage->SetBrushFromTexture(noWeaponImage);

	}
}
