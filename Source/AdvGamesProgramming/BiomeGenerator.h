// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProcedurallyGeneratedMap.h"
#include "GameFramework/Actor.h"
#include "Materials/Material.h"
#include "PrimitiveObject.h"
#include "BiomeGenerator.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API ABiomeGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABiomeGenerator();

	//Heightmap of the verticies procedurally generated
	TArray<float> InitialHeightMap;

	//Seperate Heightmap use to overlay over initial heightmap
	TArray<float> SecondHeightMap;

	//Culmulative tempurature map combining both maps
	TArray<float> TempuratureMap;

	TArray<AActor*> SpawnedPrimitives;

	UPROPERTY(EditAnywhere)
	bool bRegenerateMaps;

	UPROPERTY(EditAnywhere)
	AProcedurallyGeneratedMap* ProceduralGeneratedMap;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<APrimitiveObject> BasicSphere;

	UPROPERTY(EditAnywhere)
	UMaterial* DefaultMaterial;

protected :
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnPrimitive(FVector SpawnPosition);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override;

	void GenerateSecondNoiseMap(float MapSize);
	
	void GenerateTempuratureMap();

	// For testing purposes
	void CreateTempuraturePoints();
	void TestNoiseMapPositions();

	void ClearMaps();

};
