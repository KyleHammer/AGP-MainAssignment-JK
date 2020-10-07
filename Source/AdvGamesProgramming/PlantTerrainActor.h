// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/Material.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "PlantTerrainActor.generated.h"

UENUM()
enum class PlantType : uint8 
{
	TREE
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ADVGAMESPROGRAMMING_API APlantTerrainActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlantTerrainActor();

	UPROPERTY(EditAnywhere)
	PlantType SelectedPlantType;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *PlantMesh;

	UPROPERTY(EditAnywhere)
	USceneComponent *LocationComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetPosition(FVector NewPosition);
	void SetScale(FVector NewScale);
	
};
