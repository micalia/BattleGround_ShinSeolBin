// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class BATTLEGROUND_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = Effect)
	class UParticleSystem* damageEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FullHp = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float currHP = 0;

	UPROPERTY(EditAnywhere)
		float power = 1;

	//모델링
	/*UPROPERTY()
	ABattleGroundCharacter* */

	UPROPERTY(EditAnywhere)
	class UEnemyFSM* fsm;

	//사격 포인트 (라인트레이스 방식)
	UPROPERTY(EditAnywhere)
	class USceneComponent* shootPos;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* enemyHPwidget;

	TArray<AActor*> EmptyActorsToIgnore;
	TArray<FHitResult> OutHits;

	UPROPERTY(EditAnywhere)
	float checkEnemyCollRadius = 500;
	void CheckCreatureCollision();
	/*UPROPERTY(EditAnywhere)
	class USphereComponent* checkEnemyColl;*/

	UFUNCTION(BlueprintCallable)
	float Damaged(float damage);

	void UpdateWidgetRotation();

	FTimerHandle TimerHandle_UpdateWidgetRotation;
	FVector GetPlayerCameraLocation();
	FRotator GetPlayerCameraRotation();

	UPROPERTY()
	class UUI_EnemyHP* enemyHpUI;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* gunMeshComp;

	UPROPERTY(EditAnywhere, Category = Effect)
	class UParticleSystem* shotPosEffect;

	UPROPERTY(EditAnywhere)
	class UParticleSystemComponent* particleComp;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* shotSound;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundCue* shotSoundCue;

	UFUNCTION(BlueprintCallable)
	void EnemyDieEvent();

private:
	FVector CameraLocation;
	FRotator CameraRotation;
	FRotator WidgetRotation;
};
