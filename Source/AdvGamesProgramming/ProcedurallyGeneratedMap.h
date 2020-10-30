// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "ProceduralMeshComponent.h"
#include "WallActor.h"
#include "AIManager.h"
#include "BiomeGenerator.h"
#include "KismetProceduralMeshLibrary.h"
#include "ProcedurallyGeneratedMap.generated.h"

class ABiomeGenerator;

UCLASS()
class ADVGAMESPROGRAMMING_API AProcedurallyGeneratedMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProcedurallyGeneratedMap();

protected:
	// Called when the game starts or when spawnedw
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override;

	UPROPERTY(VisibleAnywhere, Category="Wall Generation")
	TArray<AWallActor*> MapWalls;

	UPROPERTY(EditAnywhere, Category = "Wall Generation")
	TSubclassOf<AWallActor> WallToSpawn;

	UPROPERTY(EditAnywhere, Category="Wall Generation")
	float WallHeight;

	UPROPERTY(EditAnywhere, Category = "Wall Generation")
	bool bGenerateWalls;

	UPROPERTY(EditAnywhere)
	ABiomeGenerator* BiomeGenerator;

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	int32 Width;

	UPROPERTY(EditAnywhere)
	int32 Height;

	UPROPERTY(EditAnywhere)
	float GridSize;

	UPROPERTY(VisibleAnywhere)
	TArray<FVector> Vertices;

	UPROPERTY(VisibleAnywhere)
	TArray<int32> Triangles;

	UPROPERTY(VisibleAnywhere)
	TArray<FVector2D> UVCoords;

	UPROPERTY(VisibleAnywhere)
	TArray<FVector> Normals;

	UPROPERTY(VisibleAnywhere)
	TArray<FProcMeshTangent> Tangents;

	UPROPERTY(EditAnywhere)
	float PerlinScale;

	UPROPERTY(EditAnywhere)
	float PerlinRoughness;

	UFUNCTION(BlueprintCallable)
	void GenerateMap();

	void ClearMap();

	void GenerateWalls();
	void GenerateAINodes();
	void ClearWalls();

	UPROPERTY(EditAnywhere)
	bool bRegenerateMap;

	UPROPERTY(EditAnywhere)
	bool bGenerateAINodes;

	UPROPERTY(EditAnywhere)
	AAIManager *AIManager;
};
