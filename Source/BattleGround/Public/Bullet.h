// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class BATTLEGROUND_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	class UParticleSystem* damageEffect;

	UPROPERTY(EditAnywhere)
	float power = 1;

	UFUNCTION()
	void OnOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

public: 
	// 발사체 형태의 물리 기능을 제공
	UPROPERTY(VisibleAnywhere, Category=Movement)
	class UProjectileMovementComponent* movementComp;
	// 충돌체 컴포넌트
	UPROPERTY(VisibleAnywhere, Category=Collision)
	class USphereComponent* collisionComp;
	// 스태틱메시 컴포넌트
	UPROPERTY(VisibleAnywhere, Category= BodyMesh)
	class UStaticMeshComponent* bodyMeshComp;

};
