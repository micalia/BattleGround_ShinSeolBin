// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterItem.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../BattleGroundCharacter.h"

// Sets default values
AMasterItem::AMasterItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	SM_Comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Comp"));
	SM_Comp->SetupAttachment(RootComp);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> tempOutLineMat(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Inventory/Materials/M_Outline_Inst.M_Outline_Inst'"));

	if (tempOutLineMat.Succeeded())
	{
		OutLineMat = tempOutLineMat.Object;
	}

	SM_Comp->SetCollisionProfileName(TEXT("MasterItem"));

	SphereCollComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollComp"));
	SphereCollComp->SetupAttachment(RootComp);
	SphereCollComp->SetSphereRadius(160);
	SphereCollComp->SetCollisionProfileName(TEXT("ItemCheckRange"));

}

// Called when the game starts or when spawned
void AMasterItem::BeginPlay()
{
	Super::BeginPlay();
	
	SphereCollComp->OnComponentBeginOverlap.AddDynamic(this, &AMasterItem::OnBeginOverlap);
	SphereCollComp->OnComponentEndOverlap.AddDynamic(this, &AMasterItem::OnEndOverlap);

	OutlineDynamicMat = UMaterialInstanceDynamic::Create(OutLineMat, this);
	SM_Comp->SetOverlayMaterial(OutlineDynamicMat);
}

// Called every frame
void AMasterItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMasterItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABattleGroundCharacter* Player = Cast<ABattleGroundCharacter>(OtherActor)) {
		if (Player->IsLocallyControlled()) {
			OutlineDynamicMat->SetScalarParameterValue(TEXT("Thickness"), 2);
			Player->SetOverlapItemCount(1);
		}
	}
}

void AMasterItem::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABattleGroundCharacter* Player = Cast<ABattleGroundCharacter>(OtherActor)) {
		if (Player->IsLocallyControlled()) {
			OutlineDynamicMat->SetScalarParameterValue(TEXT("Thickness"), 0);
			Player->SetOverlapItemCount(-1);
		}
	}
}

