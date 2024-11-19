// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GunWidget.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGROUND_API UGunWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	UPROPERTY(EditAnywhere, meta=(BindWidget), BlueprintReadWrite)
	class UImage* GunImage = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTexture2D> noWeaponImage;

	UFUNCTION(BlueprintCallable)
	void SetDefaulImage();

	UPROPERTY()
	class UTexture2D* noWeaponTexture;
};
