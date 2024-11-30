// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Move,
	Rotate,
	Attack,
	Die,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEGROUND_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

UPROPERTY(EditAnywhere)
	class UAnimMontage* damageMontage;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY()
	class AEnemy* me;

	UPROPERTY(EditAnywhere)
	class UEnemyAnim* anim;

	UPROPERTY()
	class AAIController* ai;

	bool bTrace;
		FVector dir;

		UPROPERTY()
		class ACharacter* playerPointer;
	//Ÿ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class ACharacter* target;

	UPROPERTY(VisibleAnywhere)
	float currTime = 0;

	UPROPERTY(EditAnywhere)
	float idleDelayTime = 1.3;
	UPROPERTY(EditAnywhere)
	float moveDelayTime = 2;

//ȸ��
UPROPERTY(EditAnywhere)
float rotMinTime = 0.6;
UPROPERTY(EditAnywhere)
float rotMaxTime = 1;
UPROPERTY(EditAnywhere) // ȸ���ϴ� �ð�
float rotatingTime = 0.7;
FRotator currRot;
FRotator destRot;

//����
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEnemyState currState = EEnemyState::Move;

	void ChangeState(EEnemyState state);

	void UpdateIdle();
	void UpdateMove();
	void UpdateRotate();
	void UpdateAttack();
	void UpdateDie();

	bool IsWaitComplete(float delayTime);

	bool bAttack;

	//�Ѿ� �� �� �ִ� ����
	bool bHit;
	UPROPERTY(EditAnywhere)
	float traceRange = 900;
	bool IsTargetTrace();

	UPROPERTY(EditAnywhere)
	float attackCool =1;
	UPROPERTY(VisibleAnywhere)
	float currAtkTime=0;

	//���ݹ���
	FVector startPos;
	FVector endPos;
	FVector lineDir;
	FVector NewEndPos;
	FVector randPos;
	UPROPERTY(EditAnywhere)
	float extensionLength = 700;
	UPROPERTY(EditAnywhere)
	float attackRange = 800;
	UPROPERTY(EditAnywhere)
	float attackErrorRange = 20;

//����
	bool bDie;
	UPROPERTY(EditAnywhere)
	float DieDelayTime = 6;
	float dieCurrTime = 0;

	UPROPERTY()
	class AInGameMode* gameMode;

	UFUNCTION()
	void FuncGameOverDel();
};
