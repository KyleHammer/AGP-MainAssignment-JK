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
		TestNoiseMapPositions();
		bRegenerateMaps = false;
	}
}

bool ABiomeGenerator::ShouldTickIfViewportsOnly() const
{
	return true;
}

void ABiomeGenerator::GenerateSecondNoiseMap(float MapSize)
{

}

void ABiomeGenerator::GenerateTempuratureMap()
{

}

void ABiomeGenerator::CreateTempuraturePoints()
{

}

void ABiomeGenerator::TestNoiseMapPositions()
{
	TArray<FVector> MapVerticies = ProceduralGeneratedMap->Vertices; //Creates copy of the vertices


	for (TActorIterator<APrimitiveObject> It(GetWorld()); It; ++It)
	{
		It->Destroy();
	}

	for (int MapPoint = 0; MapPoint < MapVerticies.Num()-1; MapPoint++)
	{
		APrimitiveObject* PrimitiveSphere = GetWorld()->SpawnActor<APrimitiveObject>(BasicSphere, MapVerticies[MapPoint], FRotator::ZeroRotator);
		SpawnedPrimitives.Add(PrimitiveSphere);
	}
}

void ABiomeGenerator::SpawnPrimitive(FVector SpawnPosition)
{
	
}

void ABiomeGenerator::ClearMaps()
{
	InitialHeightMap.Empty();
}

