// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "MasterItem.generated.h"

UENUM(BlueprintType)
enum class EItemEnum : uint8
{
	Weapon,
	Helmet,
	Consumalbes
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Name"))
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Description"))
	FString Description;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Image"))
	TArray<UTexture2D*> Image;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Weight", MakeStructureDefaultValue="0.000000"))
	double Weight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Category"))
	TEnumAsByte<EItemEnum> Category;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Amount", MakeStructureDefaultValue="0"))
	int32 Amount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="IsStackAble", MakeStructureDefaultValue="False"))
	bool IsStackAble;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ItemClass", MakeStructureDefaultValue="None"))
	TObjectPtr<UClass> ItemClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="IsEquip", MakeStructureDefaultValue="False"))
	bool IsEquip;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector StaticMeshScale;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UStaticMesh* Mesh;
};


UCLASS()
class BATTLEGROUND_API AMasterItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMasterItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemData ItemData;

	UPROPERTY(EditAnywhere)
	class USceneComponent* RootComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* SM_Comp;
	
	UPROPERTY(EditAnywhere)
	class UMaterialInterface* OutLineMat;

	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereCollComp;

	UPROPERTY()
	class UMaterialInstanceDynamic* OutlineDynamicMat;

	/*UPROPERTY(EditAnywhere)
	class UWidgetComponent* WidgetComp;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UInteractWidget> InteractWidgetFactory;

	UPROPERTY(EditAnywhere)
	class UMaterialInterface* World3DPassThrough;*/

public:
UFUNCTION()
	void OnBeginOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

UFUNCTION()
	void OnEndOverlap(
			UPrimitiveComponent* OverlappedComponent, 
			AActor* OtherActor, 
			UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex);

};
