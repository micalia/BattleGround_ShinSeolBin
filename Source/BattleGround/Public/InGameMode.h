// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "InGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameOverDel);
/**
 * 
 */
UCLASS()
class BATTLEGROUND_API AInGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	FGameOverDel gameoverDel;

	AInGameMode();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Widget")
		TSubclassOf<class UUserWidget> winWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Widget")
		TSubclassOf<class UUserWidget> loseWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Widget")
		TSubclassOf<class UInGameTopUI> InGameTopUIClass;

	UPROPERTY(EditAnywhere)
		class UWinPanel* winWidgetInstance;
	UPROPERTY(EditAnywhere)
		class ULosePanel* loseWidgetInstance;
	UPROPERTY(EditAnywhere)
		class UInGameTopUI* InGameTopUiWidgetInstance;

	int32 enemyCount = 0;
	TArray<class AEnemy*> ActiveEnemies;

	bool bWin;

	UPROPERTY(EditAnywhere)
	float winCurrTime;
	float winFadeInTime = 0.8;

	UPROPERTY(BlueprintReadWrite)
	bool bLose;

	UFUNCTION(BlueprintCallable)
	void PlayerDie();

	UPROPERTY(EditAnywhere)
	float loseCurrTime;
	float loseFadeInTime = 0.8;

	UPROPERTY()
	class ABattleGroundCharacter* Player;

	/*UPROPERTY(EditAnywhere)
	FTransform setLoseCameraTranform;*/
	UPROPERTY(EditAnywhere)
	FVector SetLocation = FVector(-80,0, 370);
	UPROPERTY(EditAnywhere)
	FRotator SetRotation  = FRotator(-70,0,0);

	FTransform moveLoseCameraTransform;
	FTransform originLoseCameraTransform;

	APlayerCameraManager* cameraManager;
	float loseCameraMoveCurrTime;
	UPROPERTY(EditAnywhere)
	float loseCameraMoveTime = 3;

	bool bEndGame;

	int32 InGamePlayerCount;

	
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* bulletHitHelmetSound;

	void PlayHitHelmetSound();

	UPROPERTY(EditAnywhere)
	class UParticleSystem* HellmetHitEffect;
};
