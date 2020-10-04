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

	//Defines the sea level and indicates minimum height for generated assets.
	UPROPERTY(EditAnywhere)
	float WaterLevel;

	// Applied to second noise map as: HeightPoint = (NoiseValue.Z)^NoisePowerValue
	UPROPERTY(EditAnywhere)
	int NoisePowerValue;

	//Defines the count density of plant objects in the scene. HIGHER the more detailed
	UPROPERTY(EditAnywhere)
	int NoiseDensity;

	//Defines the maximum height that generated assets will be rendered at.
	UPROPERTY(EditAnywhere)
	float MaxBiomeHeight; 

	UPROPERTY(EditAnywhere)
	bool bRegenerateMaps;

	UPROPERTY(EditAnywhere)
	AProcedurallyGeneratedMap* ProceduralGeneratedMap;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<APrimitiveObject> BasicSphere;

	UPROPERTY(EditAnywhere)
	UMaterial* DefaultMaterial;

	UPROPERTY(EditAnywhere)
	UMaterial* BestGrowthMaterial;

	UPROPERTY(EditAnywhere)
	UMaterial* OrdinaryGrowthMaterial;

	UPROPERTY(EditAnywhere)
	UMaterial* ColdMaterial;

	UPROPERTY(EditAnywhere)
	UMaterial* SeaLevelMaterial;

protected :
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnPrimitive(FVector SpawnPosition);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override;

	void GenerateSecondNoiseMap(int RowSize, int ColSize, float PerlinRoughness, float PerlinScale);

	void GenerateTempuratureMap(TArray<FVector> Verticies);

	// For testing purposes
	void CreateTempuraturePoints(TArray<FVector> Verticies);
	void TestNoiseMapPositions(TArray<FVector> Verticies);

	void ClearMaps();

};