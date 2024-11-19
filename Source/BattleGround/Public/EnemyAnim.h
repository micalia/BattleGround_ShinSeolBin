// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyFSM.h"
#include "EnemyAnim.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGROUND_API UEnemyAnim : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeBeginPlay() override;

	UPROPERTY()
	AEnemy* me;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=FSM)
	EEnemyState animState;

	bool bShotTiming;

	UPROPERTY()
	class AInGameMode* gameMode;
UFUNCTION()
	void AnimNotify_Shot();
};
