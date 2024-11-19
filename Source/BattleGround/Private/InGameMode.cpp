// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMode.h"
#include <Kismet/GameplayStatics.h>
#include "Enemy.h"
#include "WinPanel.h"
#include <UMG/Public/Components/CanvasPanel.h>
#include "LosePanel.h"
#include "../BattleGroundCharacter.h"
#include <Camera/CameraComponent.h>
#include "InGameTopUI.h"
#include <UMG/Public/Blueprint/WidgetBlueprintLibrary.h>
#include <NavigationSystem/Public/NavigationSystem.h>
#include <Particles/ParticleSystem.h>


AInGameMode::AInGameMode() {
	PrimaryActorTick.bCanEverTick = true;

    static ConstructorHelpers::FClassFinder<UUserWidget> tempWinWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UMG/WB_WinWidget.WB_WinWidget_C'"));
    if (tempWinWidgetClass.Succeeded())
    {
        winWidgetClass = tempWinWidgetClass.Class;
    }

	static ConstructorHelpers::FClassFinder<UUserWidget> tempLoseWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UMG/WB_LoseWidget.WB_LoseWidget_C'"));
	if (tempLoseWidgetClass.Succeeded())
	{
		loseWidgetClass = tempLoseWidgetClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UInGameTopUI> tempInGameTopUIClassWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UMG/WB_InGameTopUI.WB_InGameTopUI_C'"));
	if (tempInGameTopUIClassWidgetClass.Succeeded())
	{
		InGameTopUIClass = tempInGameTopUIClassWidgetClass.Class;
	}


	ConstructorHelpers::FObjectFinder<USoundBase> tempBulletHitHelmetSound(TEXT("/Script/Engine.SoundWave'/Game/Sounds/bulletHitHelmetSound.bulletHitHelmetSound'"));
	if (tempBulletHitHelmetSound.Succeeded()) {
		bulletHitHelmetSound = tempBulletHitHelmetSound.Object;
	}

	ConstructorHelpers::FObjectFinder<UParticleSystem> tempHitEffect(TEXT("/Script/Engine.ParticleSystem'/Game/Particle/Realistic_Starter_VFX_Pack_Vol2/Particles/Sparks/P_Sparks_C.P_Sparks_C'"));
	if (tempHitEffect.Succeeded()) {
		HellmetHitEffect = tempHitEffect.Object;
	}
}

void AInGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (winWidgetClass) {
		winWidgetInstance = CreateWidget<UWinPanel>(GetWorld(), winWidgetClass);
		if (winWidgetInstance) {
			winWidgetInstance->AddToViewport();
		}
	}

	if (loseWidgetClass) {
		loseWidgetInstance = CreateWidget<ULosePanel>(GetWorld(), loseWidgetClass);
		if (loseWidgetInstance) {
			loseWidgetInstance->AddToViewport();
		}
	}

	if (InGameTopUIClass) {
		InGameTopUiWidgetInstance = CreateWidget<UInGameTopUI>(GetWorld(), InGameTopUIClass);
		if (InGameTopUiWidgetInstance) {
			InGameTopUiWidgetInstance->AddToViewport();
		}
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		AEnemy* enemy = Cast<AEnemy>(Actor);
		if (enemy) {
			ActiveEnemies.Add(enemy);
		}
	}

	enemyCount = ActiveEnemies.Num();
	InGamePlayerCount = enemyCount + 1;

	Player = Cast<ABattleGroundCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ABattleGroundCharacter::StaticClass()));
}

void AInGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWin) {
		winCurrTime += DeltaTime;

		float opacity = winCurrTime / winFadeInTime;
		if (opacity <= 1) {
			winWidgetInstance->winPanel->SetRenderOpacity(opacity);
		}
	}
	if (bLose) {
		loseCurrTime += DeltaTime;
		loseCameraMoveCurrTime += DeltaTime;

		float opacity = loseCurrTime / loseFadeInTime;
		if (opacity <= 1) {
			loseWidgetInstance->losePanel->SetRenderOpacity(opacity);
		}

		if (Player) {
			if (loseCameraMoveCurrTime < loseCameraMoveTime) {
			float alpha = loseCameraMoveCurrTime / loseCameraMoveTime;
				Player->GetFollowCamera()->SetRelativeLocation(FMath::Lerp(originLoseCameraTransform.GetLocation(), moveLoseCameraTransform.GetLocation(), alpha));
				Player->GetFollowCamera()->SetRelativeRotation(FMath::Lerp(originLoseCameraTransform.GetRotation(), moveLoseCameraTransform.GetRotation(), alpha));
			}
		}

	}

	if (InGameTopUiWidgetInstance) {
		InGameTopUiWidgetInstance->UpdatePlayerCountTxt(InGamePlayerCount);
	}
}

void AInGameMode::PlayerDie()
{
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);

	bEndGame = true;

	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass());
	for (int32 i = 0; i < FoundWidgets.Num(); i++)
	{
		if (FoundWidgets[i]->GetName().Contains(TEXT("HpWidget"))) {
			FoundWidgets[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (FoundWidgets[i]->GetName().Contains(TEXT("Inventory"))) {
			FoundWidgets[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (FoundWidgets[i]->GetName().Contains(TEXT("Player_HP"))) {
			FoundWidgets[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (FoundWidgets[i]->GetName().Contains(TEXT("TopUI"))) {
			FoundWidgets[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (FoundWidgets[i]->GetName().Contains(TEXT("Lose"))) {
			FoundWidgets[i]->SetVisibility(ESlateVisibility::Visible);
		}

	}

	APlayerController* controller = GetWorld()->GetFirstPlayerController();
	Player->DisableInput(controller);
	originLoseCameraTransform = Player->GetFollowCamera()->GetRelativeTransform();

	//FTransform 자료형 빼기 식 성공
	moveLoseCameraTransform = FTransform(
		SetRotation,
		originLoseCameraTransform.GetLocation() + SetLocation,
		FVector(1)
	);
		bLose = true;
	gameoverDel.Broadcast();
}


void AInGameMode::PlayHitHelmetSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), bulletHitHelmetSound);
}

