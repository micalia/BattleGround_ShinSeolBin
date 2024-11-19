// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include <Kismet/KismetMathLibrary.h>
#include "Enemy.h"
#include <AIModule/Classes/AIController.h>
#include "../BattleGroundCharacter.h"
#include <Kismet/GameplayStatics.h>
#include <Components/CapsuleComponent.h>
#include "EnemyAnim.h"
#include <Particles/ParticleSystemComponent.h>
#include "InGameMode.h"
#include <UMG/Public/Blueprint/UserWidget.h>

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UAnimMontage> tempMontage(TEXT("/Script/Engine.AnimMontage'/Game/Animations/Enemy/Montage/AM_SoldierDie.AM_SoldierDie'"));
	if (tempMontage.Succeeded())
	{
		damageMontage = tempMontage.Object;
	}
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	me = Cast<AEnemy>(GetOwner());
	anim = Cast<UEnemyAnim>(me->GetMesh()->GetAnimInstance());
	ai = Cast<AAIController>(me->GetController());
	target = Cast<ABattleGroundCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ABattleGroundCharacter::StaticClass()));
	playerPointer = target;
	gameMode = Cast<AInGameMode>(GetWorld()->GetAuthGameMode());
	gameMode->gameoverDel.AddDynamic(this, &UEnemyFSM::FuncGameOverDel);
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(ai == nullptr) return;
	if (gameMode->bEndGame == true) {
		ChangeState(EEnemyState::Idle);
		return;
	}
	me->CheckCreatureCollision();
	if (target == nullptr)return;
	if (me->OutHits.Num()>0) {
		if (target) {
			dir = target->GetActorLocation() - me->GetActorLocation();
		}
	}

	switch (currState) {
	case EEnemyState::Idle:
		UpdateIdle();
		break;
	case EEnemyState::Move:
		UpdateMove();
		break;
	case EEnemyState::Rotate:
		UpdateRotate();
		break;
	case EEnemyState::Attack:
		UpdateAttack();
		break;
	case EEnemyState::Die:
		UpdateDie();
		break;
	}
}


void UEnemyFSM::ChangeState(EEnemyState state)
{
	/*UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState"), true);
	if (enumPtr != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s -----> %s"),
			*enumPtr->GetNameStringByIndex((int32)currState),
			*enumPtr->GetNameStringByIndex((int32)state));
	}*/

	currState = state;
	anim->animState = state;

	ai->StopMovement();

	switch (currState)
	{
	case EEnemyState::Idle:
		break;
	case EEnemyState::Move:
		break;
	case EEnemyState::Rotate: {
		float ranYaw = UKismetMathLibrary::RandomFloatInRange(-180, 180);
		destRot = FRotator(0, ranYaw, 0);
		currRot = me->GetActorRotation();
		rotatingTime = UKismetMathLibrary::RandomFloatInRange(rotMinTime, rotMaxTime);
	}
							break;
	case EEnemyState::Attack:
		break;
	case EEnemyState::Die:
		bDie = true;
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		me->PlayAnimMontage(damageMontage, 1.0f, FName(TEXT("Die")));
		break;
	}
}

void UEnemyFSM::UpdateIdle()
{
	ai->StopMovement();
	if (IsWaitComplete(idleDelayTime)) {
		ChangeState(EEnemyState::Move);
	}

	if (bTrace) {
		if (dir.Length() < attackRange)
		{
			ChangeState(EEnemyState::Attack);
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Idle!"))
}

void UEnemyFSM::UpdateMove()
{

	if (IsWaitComplete(moveDelayTime)) {
		ChangeState(EEnemyState::Rotate);
	}

	bTrace = IsTargetTrace();

	if (bTrace) {
		if (dir.Length() < attackRange)
		{
			ChangeState(EEnemyState::Attack);
		}
	}
	else
	{
		EPathFollowingRequestResult::Type re = ai->MoveToActor(target);
	}

	//UE_LOG(LogTemp, Warning, TEXT("Move!"))
}

void UEnemyFSM::UpdateRotate()
{
	currTime += GetWorld()->GetDeltaSeconds();

	float alpha = currTime / rotatingTime;
	me->SetActorRotation(FMath::Lerp(currRot, destRot, alpha));
	if (alpha > 1) {
		ChangeState(EEnemyState::Idle);
		currTime = 0;
	}
	ai->StopMovement();
	bTrace = IsTargetTrace();

	if (bTrace) {
		if (dir.Length() < attackRange)
		{
			ChangeState(EEnemyState::Attack);
		}
	}
	else
	{
		EPathFollowingRequestResult::Type re = ai->MoveToActor(target);
	}
	//UE_LOG(LogTemp, Warning, TEXT("Rotate!"))
}

void UEnemyFSM::UpdateAttack()
{
	ai->StopMovement();
	currAtkTime += GetWorld()->GetDeltaSeconds();

	if (currAtkTime > attackCool) {
		AEnemy* checkEnemy = Cast<AEnemy>(target);

		bAttack = true;
		startPos = me->shootPos->GetComponentLocation();
		//End (카메라위치 + 카메라 앞방향 * 거리)
		int32 ranVal = UKismetMathLibrary::RandomIntegerInRange(1, 3);
		if (!checkEnemy) {
			switch (ranVal)
			{
			case 1:
			{
				randPos = target->GetMesh()->GetSocketLocation(TEXT("head")) + target->GetActorRightVector() * UKismetMathLibrary::RandomFloatInRange(-attackErrorRange, attackErrorRange);

				endPos = randPos;

			}
			break;
			case 2: {
				randPos = target->GetMesh()->GetSocketLocation(TEXT("spine_05")) + target->GetActorRightVector() * UKismetMathLibrary::RandomFloatInRange(-attackErrorRange, attackErrorRange);
				endPos = randPos;

			}
				  break;
			case 3: {
				randPos = target->GetMesh()->GetSocketLocation(TEXT("pelvis")) + target->GetActorRightVector() * UKismetMathLibrary::RandomFloatInRange(-attackErrorRange, attackErrorRange);
				endPos = randPos;
			}
				  break;
			}
		}
		else {
			switch (ranVal)
			{
			case 1:
			{
				randPos = target->GetMesh()->GetSocketLocation(TEXT("head")) + target->GetActorRightVector() * UKismetMathLibrary::RandomFloatInRange(-attackErrorRange, attackErrorRange);

				endPos = randPos;

			}
			break;
			case 2: {
				randPos = target->GetMesh()->GetSocketLocation(TEXT("Spine2")) + target->GetActorRightVector() * UKismetMathLibrary::RandomFloatInRange(-attackErrorRange, attackErrorRange);
				endPos = randPos;

			}
				  break;
			case 3: {
				randPos = target->GetMesh()->GetSocketLocation(TEXT("Hips")) + target->GetActorRightVector() * UKismetMathLibrary::RandomFloatInRange(-attackErrorRange, attackErrorRange);
				endPos = randPos;
			}
				  break;
			}
		}

		lineDir = (endPos - startPos).GetSafeNormal();
		NewEndPos = endPos + lineDir * extensionLength;

		currAtkTime = 0;
	}

	float dist = dir.Length();

	bTrace = IsTargetTrace();

	me->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(), target->GetActorLocation()));

	if (!bTrace) {
		ChangeState(EEnemyState::Idle);
	}

	//UE_LOG(LogTemp, Warning, TEXT("Attack!"))
}

void UEnemyFSM::UpdateDie()
{
	if (bDie) {
		dieCurrTime += GetWorld()->GetDeltaSeconds();
		if (dieCurrTime > DieDelayTime) {
			me->Destroy();
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Die!"))
}



bool UEnemyFSM::IsWaitComplete(float delayTime)
{
	currTime += GetWorld()->DeltaTimeSeconds;

	if (currTime > delayTime)
	{
		currTime = 0;
		return true;
	}

	return false;
}

bool UEnemyFSM::IsTargetTrace()
{
	float dotValue = FVector::DotProduct(me->GetActorForwardVector(), dir.GetSafeNormal());

	float angle = UKismetMathLibrary::DegAcos(dotValue);

	if (angle < 30 && dir.Length() < traceRange)
	{
		FHitResult hitInfo;
		FCollisionQueryParams param;
		param.AddIgnoredActor(me);

		bHit = GetWorld()->LineTraceSingleByChannel(
			hitInfo,
			me->GetActorLocation(),
			target->GetActorLocation(),
			ECC_Visibility,
			param);

		if (bHit)
		{
			if (hitInfo.GetActor()->GetName().Contains(TEXT("Person")))
			{
				return true;
			}

			if (hitInfo.GetActor()->GetName().Contains(TEXT("Enemy"))) {
				return true;
			}
		}
		
	}
	
	return false;
}

void UEnemyFSM::FuncGameOverDel()
{
	ChangeState(EEnemyState::Idle);
}