#include "Enemy.h"
#include <Engine/SkeletalMesh.h>
#include <Components/SceneComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/SphereComponent.h>
#include <Kismet/KismetSystemLibrary.h>
#include "Components/WidgetComponent.h"
#include "EnemyFSM.h"
#include <Camera/CameraComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <GameFramework/PlayerController.h>
#include "UI_EnemyHP.h"
#include <Particles/ParticleSystem.h>
#include "EnemyAnim.h"
#include <Particles/ParticleSystemComponent.h>
#include <Sound/SoundCue.h>
#include "InGameMode.h"
#include "WinPanel.h"
#include <UMG/Public/Blueprint/WidgetBlueprintLibrary.h>

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Modelings/Soldier/SM_Soldier.SM_Soldier'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocation(FVector(0,0,-85));
		GetMesh()->SetRelativeRotation(FRotator(0,-90,0));
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));

	fsm = CreateDefaultSubobject<UEnemyFSM>(TEXT("FSM"));

	enemyHPwidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyHpWidget"));
	enemyHPwidget->SetupAttachment(GetCapsuleComponent());
	enemyHPwidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	enemyHPwidget->SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> tempDamageEffect(TEXT("/Script/Engine.ParticleSystem'/Game/Particle/InProject/P_Blood_BG.P_Blood_BG'"));
	if (tempDamageEffect.Succeeded()) {
		damageEffect = tempDamageEffect.Object;
	}

	ConstructorHelpers::FClassFinder<UEnemyAnim> tempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/ABP_Enemy.ABP_Enemy_C'"));
	if (tempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnim.Class);
	}

	gunMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("gunMesh"));
	gunMeshComp->SetupAttachment(GetMesh(), TEXT("GunPos"));
	gunMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempGunMesh(TEXT("/Script/Engine.StaticMesh'/Game/Modelings/M416/M4A1.M4A1'"));
	if (tempGunMesh.Succeeded()) {
		gunMeshComp->SetStaticMesh(tempGunMesh.Object);
	}

	gunMeshComp->SetRelativeLocation(FVector(-16.816826, -27.965630, 1.319249));
	gunMeshComp->SetRelativeRotation(FRotator(-4.699833, -106.534025, 101.128250));
	gunMeshComp->SetRelativeScale3D(FVector(0.881841, 0.811883, 0.862825));

	shootPos = CreateDefaultSubobject<USceneComponent>(TEXT("shootPos"));
	shootPos->SetupAttachment(gunMeshComp);
	shootPos->SetRelativeLocation(FVector(60.534064, 0.293241, 27.286787));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> tempShotPosEffect(TEXT("/Script/Engine.ParticleSystem'/Game/Particle/InProject/P_shotPoint_BG.P_shotPoint_BG'"));
	if (tempShotPosEffect.Succeeded()) {
		shotPosEffect = tempShotPosEffect.Object;
	}

	particleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("particleComp"));
	particleComp->SetupAttachment(gunMeshComp);
	particleComp->SetRelativeLocation(FVector(62.654387, 0.160988, 24.570430));
	particleComp->SetRelativeRotation(FRotator(-90,0,0));

	ConstructorHelpers::FObjectFinder<USoundBase> tempShotSound(TEXT("/Script/Engine.SoundWave'/Game/Sounds/m416-Single-shot-sound.m416-Single-shot-sound'"));
	if (tempShotSound.Succeeded()) {
		shotSound = tempShotSound.Object;
	}

	ConstructorHelpers::FObjectFinder<USoundCue> tempShotSoundCue(TEXT("/Script/Engine.SoundCue'/Game/Sounds/Kar98_Cue.Kar98_Cue'"));
	if (tempShotSoundCue.Succeeded()) {
		shotSoundCue = tempShotSoundCue.Object;
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	bool b = false;
	
	currHP = FullHp;

	enemyHpUI = Cast<UUI_EnemyHP>(enemyHPwidget->GetWidget());
	//GetWorldTimerManager().SetTimer(TimerHandle_UpdateWidgetRotation, this, &AEnemy::UpdateWidgetRotation, 0.1f, true);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (enemyHpUI !=nullptr) {
		enemyHpUI->UpdateCurrHP(currHP, FullHp);
	}

	//CheckCreatureCollision();
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::CheckCreatureCollision()
{
	//if (fsm->target) {
		EmptyActorsToIgnore.Add(this);
		bool bResult = UKismetSystemLibrary::SphereTraceMulti(
			GetWorld(),
			GetActorLocation(),
			GetActorLocation(),
			checkEnemyCollRadius,
			UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5),
			true,
			EmptyActorsToIgnore,
			EDrawDebugTrace::None,
			OutHits,
			true
		);

		for (int32 i = 0; i < OutHits.Num(); i++)
		{
			if (fsm == nullptr)return;
			if (OutHits[i].GetActor()->GetName().Contains(TEXT("Person"))) {
				fsm->target = fsm->playerPointer;
				break;
			}
			else if (OutHits[i].GetActor()->GetName().Contains(TEXT("Enemy"))) {
				fsm->target = Cast<ACharacter>(OutHits[i].GetActor());
				break;
			}
			else {
				fsm->target = fsm->playerPointer;
			}
		}

	//}
}

float AEnemy::Damaged(float damage)
{
	if (fsm->bDie)return 0;
	currHP -= damage;
	if (currHP <= 0) {
		EnemyDieEvent();
	}
	return currHP;
}

void AEnemy::UpdateWidgetRotation()
{
	CameraLocation = GetPlayerCameraLocation();
	CameraRotation = GetPlayerCameraRotation();
	if (enemyHPwidget)
	{
		WidgetRotation = UKismetMathLibrary::FindLookAtRotation(enemyHPwidget->GetComponentLocation(), CameraLocation);
		enemyHPwidget->SetWorldRotation(WidgetRotation);
	}
}

FVector AEnemy::GetPlayerCameraLocation()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	if (PlayerController != nullptr)
	{
		UCameraComponent* MainCamera = PlayerController->GetPawn()->FindComponentByClass<UCameraComponent>();

		if (MainCamera != nullptr)
		{
			return MainCamera->GetComponentLocation();
		}
	}

	return FVector::ZeroVector;
}

FRotator AEnemy::GetPlayerCameraRotation()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	if (PlayerController != nullptr)
	{
		UCameraComponent* MainCamera = PlayerController->GetPawn()->FindComponentByClass<UCameraComponent>();

		if (MainCamera != nullptr)
		{
			return MainCamera->GetComponentRotation();
		}
	}

	return FRotator::ZeroRotator;
}

void AEnemy::EnemyDieEvent()
{
	if (fsm->bDie == true) return;
	AInGameMode* gameMode = Cast<AInGameMode>(GetWorld()->GetAuthGameMode());
	if (gameMode != NULL) {
		gameMode->enemyCount--;
		gameMode->InGamePlayerCount--;
		if (gameMode->enemyCount <= 0) {
			TArray<UUserWidget*> FoundWidgets;
			UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UUserWidget::StaticClass());
			for (int32 i = 0; i < FoundWidgets.Num(); i++)
			{
				if (FoundWidgets[i]->GetName().Contains(TEXT("Player_HP"))) {
					FoundWidgets[i]->SetVisibility(ESlateVisibility::Collapsed);
				}
				if (FoundWidgets[i]->GetName().Contains(TEXT("TopUI"))) {
					FoundWidgets[i]->SetVisibility(ESlateVisibility::Collapsed);
				}

			}

			GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);

			gameMode->bWin = true;
			gameMode->winWidgetInstance->SetVisibility(ESlateVisibility::Visible);

		}
	}
	fsm->ChangeState(EEnemyState::Die);
}
