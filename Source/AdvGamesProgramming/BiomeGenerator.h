// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "ProcedurallyGeneratedMap.h"
#include "GameFramework/Actor.h"
#include "Materials/Material.h"
#include "PrimitiveObject.h"
#include "PlantTerrainActor.h"
#include "BiomeGenerator.generated.h"

class AProcedurallyGeneratedMap;

UCLASS()
class ADVGAMESPROGRAMMING_API ABiomeGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABiomeGenerator();

	UPROPERTY(VisibleAnywhere, Category = "Biome Noise Generation")
	TArray<int32> SecondNoiseArray;

	UPROPERTY(EditAnywhere, Category = "Biome Noise Generation")
	float NoiseThreshold;

	UPROPERTY(EditAnywhere, Category = "Biome Noise Generation")
	float PerlinScale;

	UPROPERTY(EditAnywhere, Category = "Biome Noise Generation")
	float PerlinRoughness;

	UPROPERTY(EditAnywhere, Category = "Biome Noise Generation")
	bool bGenerateSecondNoise;

	//This indicates areas where the map is not being occupied by an obstructive object
	UPROPERTY(VisibleAnywhere)
	TArray<bool> ValidAvailablePositions;

	// Stores test primitives
	TArray<AActor*> SpawnedPrimitives;

	// Stores terrain plants / foliage objects
	TArray<AActor*> SpawnedTerrainFoliage;

	//Defines the sea level and indicates minimum height for generated assets.
	UPROPERTY(EditAnywhere)
	float SeaLevel;

	//Minimum scale for tree
	UPROPERTY(EditAnywhere)
	float MinScale;
	
	//Maximum scale for tree
	UPROPERTY(EditAnywhere)
	float MaxScale;

	// Applied to second noise map as: HeightPoint = (NoiseValue.Z)^NoisePowerValue
	UPROPERTY(EditAnywhere)
	int NoisePowerValue;

	//Defines the count density of plant objects in the scene. HIGHER the more detailed
	UPROPERTY(EditAnywhere)
	int NoiseDensity;

	//Defines the maximum height that generated assets will be rendered at.
	UPROPERTY(EditAnywhere)
	float MaxAltitude; 

	//Defines the suitable angle to which these objects can spawn in (limits spawning steep slopes)
	UPROPERTY(EditAnywhere)
	float AllowedSlopeAngle;

	UPROPERTY(EditAnywhere)
	bool bRegenerateMaps;

	UPROPERTY(EditAnywhere)
	bool bGenerateFoliage; //When true renders the foliage

	UPROPERTY(EditAnywhere)
	bool bIncludeSecondaryVariation;

	UPROPERTY(EditAnywhere)
	bool bGenerateTestSpheres; //When true renders the biome spheres

	UPROPERTY(EditAnywhere)
	AProcedurallyGeneratedMap* ProceduralGeneratedMap;

	UPROPERTY(EditAnywhere, Category = "Sphere Item")
	TSubclassOf<APrimitiveObject> BasicSphere;

	UPROPERTY(EditAnywhere, Category = "Tree Item")
	TSubclassOf<APlantTerrainActor> TreeObject;

	//THESE ARE UNUSED FOR THE FINAL PRODUCT BUT WHERE USED FOR THE TEST SPHERE SPAWN
	UPROPERTY(EditAnywhere)
	UMaterial* DefaultMaterial;

	UPROPERTY(EditAnywhere)
	UMaterial* WarmMaterial;

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

	void SpawnPlants(TArray<FVector> Verticies, int Width, int Height);
	
	void SpawnTestPrimitives(FVector SpawnPosition);
	void DetermineBiomeSpawn(FVector SpawnPosition);
	void GenerateSecondaryNoiseMap(int Width, int Height);
	bool OverlaySecondaryMap(int Width, int Row, int Col);

	bool CheckValidSlopePosition(TArray<FVector> Verticies, int Width, int Height, int Row, int Col);
	bool CheckValidAngle(FVector CurrentNode, FVector ToRightNode, FVector ToForwardNode);

	void ClearMaps();
	void ClearOverlayNoise();
};