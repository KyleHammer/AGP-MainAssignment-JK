// Fill out your copyright notice in the Description page of Project Settings.


#include "BiomeGenerator.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "EngineUtils.h"

// Sets default values
ABiomeGenerator::ABiomeGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MaxBiomeHeight = 200;
	NoisePowerValue = 2;
	WaterLevel = -10;
	bRegenerateMaps = false;
	//static ConstructorHelpers::FClassFinder<UBlueprint> PrimitiveSphereBP(TEXT("/Blueprints/PrimitiveSphere"));
}

// Called when the game starts or when spawned
void ABiomeGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABiomeGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRegenerateMaps)
	{
		ClearMaps();

		TArray<FVector> MapVerticies = ProceduralGeneratedMap->Vertices;
		int RowSize = ProceduralGeneratedMap->Height;
		int ColSize = ProceduralGeneratedMap->Width;
		
		GenerateSecondNoiseMap(RowSize, ColSize, ProceduralGeneratedMap->PerlinRoughness, ProceduralGeneratedMap->PerlinScale);

		TestNoiseMapPositions(MapVerticies);
		bRegenerateMaps = false;
	}
}

bool ABiomeGenerator::ShouldTickIfViewportsOnly() const
{
	return true;
}

void ABiomeGenerator::GenerateSecondNoiseMap(int RowSize, int ColSize, float PerlinRoughness, float PerlinScale)
{
	float PerlinOffset = FMath::RandRange(-10000.0f, 10000.0f);

	for (int X = 0; X < RowSize; X++)
	{
		for (int Y = 0; Y < ColSize; Y++)
		{
			float HeightVal = FMath::PerlinNoise2D(FVector2D(float(Y) * PerlinRoughness + PerlinOffset, float(X) * PerlinRoughness + PerlinOffset)) * PerlinScale;
			SecondHeightMap.Add(HeightVal);
		}
	}
}

void ABiomeGenerator::GenerateTempuratureMap(TArray<FVector> Verticies)
{

}

void ABiomeGenerator::CreateTempuraturePoints(TArray<FVector> Verticies)
{

}

void ABiomeGenerator::TestNoiseMapPositions(TArray<FVector> Verticies)
{
	for (TActorIterator<APrimitiveObject> It(GetWorld()); It; ++It)
	{
		It->Destroy();
	}

	for (int MapPoint = 0; MapPoint < Verticies.Num() - 1; MapPoint++)
	{
		FVector Location = FVector(Verticies[MapPoint].X, Verticies[MapPoint].Y, SecondHeightMap[MapPoint]); // Currently is displaying the second generated map
		APrimitiveObject *PrimitiveSphere = GetWorld()->SpawnActor<APrimitiveObject>(BasicSphere, Location, FRotator::ZeroRotator);
		PrimitiveSphere->SetMaterial(DefaultMaterial);
		SpawnedPrimitives.Add(PrimitiveSphere);
	}
}

void ABiomeGenerator::SpawnPrimitive(FVector SpawnPosition)
{
	
}

void ABiomeGenerator::ClearMaps()
{
	InitialHeightMap.Empty();
	SecondHeightMap.Empty();
}

