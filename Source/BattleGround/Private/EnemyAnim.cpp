// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"
#include <Kismet/GameplayStatics.h>
#include "Enemy.h"
#include <Particles/ParticleSystem.h>
#include <Particles/ParticleSystemComponent.h>
#include "../BattleGroundCharacter.h"
#include <Sound/SoundCue.h>
#include "InGameMode.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/PlayerController.h>
#include <UMG/Public/Blueprint/WidgetBlueprintLibrary.h>
#include "EnemyFSM.h"

void UEnemyAnim::NativeBeginPlay()
{
	me = Cast<AEnemy>(TryGetPawnOwner());
	gameMode = Cast<AInGameMode>(GetWorld()->GetAuthGameMode());
	if (me->fsm == nullptr) {
		me->fsm = me->GetComponentByClass<UEnemyFSM>();
	}
}

void UEnemyAnim::AnimNotify_Shot(){
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Cast<USoundBase>(me->shotSoundCue), me->shootPos->GetComponentLocation(), 1.f, 1.f,  0.f, nullptr, nullptr, nullptr);
	bShotTiming = true;

	FRotator shotPosRot = FRotator(90, 0, 0);
	me->particleComp->SetActive(true);

	FHitResult hitInfoShot;
	FCollisionQueryParams paramShot;
	paramShot.AddIgnoredActor(me);

	bool bDamage = GetWorld()->LineTraceSingleByChannel(
		hitInfoShot,
		me->fsm->startPos,
		me->fsm->NewEndPos,
		ECC_Visibility,
		paramShot);
		DrawDebugLine(GetWorld(), me->fsm->startPos, me->fsm->NewEndPos, FColor::Blue, false, 3, 0, 3);
		if (bDamage) {
				auto HitCompName = hitInfoShot.GetComponent()->GetName();
				if (HitCompName.Contains(TEXT("Helmet"))) {
					gameMode->PlayHitHelmetSound();

					auto NewRotation = hitInfoShot.ImpactNormal.Rotation();
					NewRotation.Roll += 90;
					NewRotation.Yaw += 270;

					auto sparkEffect = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), gameMode->HellmetHitEffect, hitInfoShot.ImpactPoint);
					sparkEffect->SetWorldScale3D(FVector(0.2));
					sparkEffect->SetWorldRotation(NewRotation);
					sparkEffect->bAutoDestroy;
					return;
				}
			ABattleGroundCharacter* player = Cast<ABattleGroundCharacter>(hitInfoShot.GetActor());
			if (player) {
				auto NewRotation = hitInfoShot.ImpactNormal.Rotation();
				NewRotation.Roll += 90;
				NewRotation.Yaw += 270;
				
				auto bloodEffect = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), me->damageEffect, hitInfoShot.ImpactPoint);
				bloodEffect->SetWorldRotation(NewRotation);
				player->currHp--;
				if (player->currHp <= 0) {
					
					if (gameMode != NULL && gameMode->bLose == false) {
						gameMode->PlayerDie();
					}
				}

				me->fsm->bAttack = false;

			}
			else {
				AEnemy* enemy = Cast<AEnemy>(hitInfoShot.GetActor());
				if (enemy) {
					int32 enemyHP = enemy->Damaged(me->power);
					auto NewRotation = hitInfoShot.ImpactNormal.Rotation();
					NewRotation.Roll += 90;
					NewRotation.Yaw += 270;

					auto bloodEffect = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), me->damageEffect, hitInfoShot.ImpactPoint);
					bloodEffect->SetWorldRotation(NewRotation);

					if (enemyHP <= 0) {
						me->fsm->target = me->fsm->playerPointer;
					}

					me->fsm->bAttack = false;

				}

			}

		}
}
