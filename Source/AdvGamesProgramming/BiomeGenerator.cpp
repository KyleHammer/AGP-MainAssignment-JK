// Fill out your copyright notice in the Description page of Project Settings.

#include "BiomeGenerator.h"
#include "ProcedurallyGeneratedMap.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "EngineUtils.h"

// ---------------------------------------
// 	GENERATING MAPS:
//	1. Generate the procedural map first (IN THE 'ProcedurallyGeneratedMap' class)
//	2. Generate the biome map second
//  3. Generate the AI nodes last
//
// ---------------------------------------

// Sets default values
ABiomeGenerator::ABiomeGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NoiseThreshold = 100;
	PerlinScale = 600;
	PerlinRoughness = 2.0;

	MaxAltitude = 200;
	NoisePowerValue = 2;
	SeaLevel = -10;
	bRegenerateMaps = false;
	MinScale = 1;
	MaxScale = 2;
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

		SpawnPlants(MapVerticies, ProceduralGeneratedMap->Width, ProceduralGeneratedMap->Height);
		bRegenerateMaps = false;
	}

	if (bGenerateSecondNoise)
	{
		ClearOverlayNoise();

		int Height = ProceduralGeneratedMap->Height;
		int Width = ProceduralGeneratedMap->Width;
		GenerateSecondaryNoiseMap(Width, Height);
		bGenerateSecondNoise = false;
	}
}

// Summary: Ensures that the class can run on viewport in editor
//
bool ABiomeGenerator::ShouldTickIfViewportsOnly() const
{
	return true;
}

//Summary: Spawns the plants corresponsing to the verticies making up the procedualmap
//
void ABiomeGenerator::SpawnPlants(TArray<FVector> Verticies, int Width, int Height)
{
	for (TActorIterator<APrimitiveObject> It(GetWorld()); It; ++It)
	{
		It->Destroy();
	}

	for (TActorIterator<APlantTerrainActor> It(GetWorld()); It; ++It)
	{
		//UE_LOG(LogTemp, Warning, TEXT(">> Destroyed plant actor"))
		It->Destroy();
	}

	for (int Row = 0; Row < Height; Row++)
	{
		for (int Col = 0; Col < Width; Col++)
		{
			if (CheckValidSlopePosition(Verticies, Width, Height, Row, Col))
			{
				if (OverlaySecondaryMap(Width, Row, Col)) 
				{
					FVector Location = Verticies[Row * Width + Col];

					if (bGenerateTestSpheres)
						SpawnTestPrimitives(Location);
					if (bGenerateFoliage)
						DetermineBiomeSpawn(Location);

					ValidAvailablePositions.Add(false);
				}
				else {
					ValidAvailablePositions.Add(true);
				}
			} 
			else {
				ValidAvailablePositions.Add(true);
			}
		}
	}
}


// Summary: Spawns objects depending on the type of biome position is in
//
void ABiomeGenerator::DetermineBiomeSpawn(FVector SpawnPosition)
{
	//UE_LOG(LogTemp, Warning, TEXT("first: %f, secnd: %f"), NoisePointOne, NoisePointTwo);

	if (SpawnPosition.Z <= SeaLevel) {
		UE_LOG(LogTemp, Warning, TEXT("This position is below sea level"));
		return;
	}

	if (SpawnPosition.Z >= MaxAltitude) {
		UE_LOG(LogTemp, Warning, TEXT("This position is above max altitude"));
		return;
	}

	//This will spawn the trees within the warm regions
	if (TreeObject == NULL) return;
	float RandScale = FMath::RandRange(MinScale, MaxScale);

	APlantTerrainActor *TerrainPlant = GetWorld()->SpawnActor<APlantTerrainActor>(TreeObject, SpawnPosition, FRotator::ZeroRotator, FActorSpawnParameters());
	TerrainPlant->SetScale(FVector(RandScale, RandScale, RandScale));
	SpawnedTerrainFoliage.Add(TerrainPlant);
}

void ABiomeGenerator::GenerateSecondaryNoiseMap(int Width, int Height)
{
	float PerlinOffset = FMath::RandRange(-10000.0f, 10000.0f);

	for (int Row = 0; Row < Height; Row++)
	{
		for (int Col = 0; Col < Width; Col++)
		{
			float HeightVal = FMath::PerlinNoise2D(FVector2D(float(Col) * PerlinRoughness + PerlinOffset, float(Row) * PerlinRoughness + PerlinOffset)) * PerlinScale;
			SecondNoiseArray.Add(HeightVal);
			UE_LOG(LogTemp, Warning, TEXT("Noise Point At: %f"), HeightVal);
		}
	}
}

bool ABiomeGenerator::OverlaySecondaryMap(int Width, int Row, int Col)
{
	return SecondNoiseArray[Row * Width + Col] > NoiseThreshold;
}

// Summary: Returns boolean if the specifies position is not too steep
//
bool ABiomeGenerator::CheckValidSlopePosition(TArray<FVector> Verticies, int Width, int Height, int Row, int Col)
{
	if (Row == 0 && Col == 0)
	{
		// For corner Row=0 and Col=0
		return CheckValidAngle(Verticies[Row * Width + Col], Verticies[Row * Width + (Col + 1)], Verticies[(Row + 1) * Width + Col]);
	}
	else if (Row == 0 && Col == Width - 1)
	{
		// For corner Row=0 and Col = Width - 1
		return CheckValidAngle(Verticies[Row * Width + Col], Verticies[Row * Width + (Col - 1)], Verticies[(Row + 1) * Width + Col]);
	}
	else if (Row == Height - 1 && Col == 0)
	{
		// For top corner at Row = Height - 1 and Col = 0
		return CheckValidAngle(Verticies[Row * Width + Col], Verticies[Row * Width + (Col - 1)], Verticies[(Row - 1) * Width + Col]);
	}
	else if (Row == Height - 1 && Col == Width - 1)
	{
		//   = Height - 1 and Col = Width - 1
		return CheckValidAngle(Verticies[Row * Width + Col], Verticies[Row * Width + (Col - 1)], Verticies[(Row - 1) * Width + Col]);
	}
	else if (Col == 0)
	{
		//Left edge case
		return CheckValidAngle(Verticies[Row * Width + Col], Verticies[Row * Width + (Col + 1)], Verticies[(Row + 1) * Width + Col]);
	}
	else if (Row == Height - 1)
	{
		//Bottom edge case
		return CheckValidAngle(Verticies[Row * Width + Col], Verticies[Row * Width + (Col + 1)], Verticies[(Row - 1) * Width + Col]);
	}
	else if (Col == Width - 1)
	{
		//Right edge case
		return CheckValidAngle(Verticies[Row * Width + Col], Verticies[Row * Width + (Col - 1)], Verticies[(Row + 1) * Width + Col]);
	}
	else if (Row == 0)
	{
		//Top edge case
		return CheckValidAngle(Verticies[Row * Width + Col], Verticies[Row * Width + (Col + 1)], Verticies[(Row + 1) * Width + Col]);
	}
	else
	{
		if (CheckValidAngle(Verticies[Row * Width + Col], Verticies[Row * Width + (Col + 1)], Verticies[(Row - 1) * Width + Col]))
		{
			return CheckValidAngle(Verticies[Row * Width + Col], Verticies[Row * Width + (Col - 1)], Verticies[(Row + 1) * Width + Col]);
		}
	}

	return false;
}

// Summary: checks the angle from the current to the next node in both right and forward directions
//
bool ABiomeGenerator::CheckValidAngle(FVector CurrentNode, FVector ToRightNode, FVector ToForwardNode)
{
	FVector Direction = CurrentNode - ToRightNode;
	FVector ForwardDirection = CurrentNode - ToForwardNode;

	Direction.Normalize();
	ForwardDirection.Normalize();

	//UE_LOG(LogTemp, Warning, TEXT("Identified Direction At: %f"), Direction.Z);

	//Checks for the angle between nodes in both positive and negative
	bool RightDirectionCheck = Direction.Z < AllowedSlopeAngle && Direction.Z > AllowedSlopeAngle * -1.0f;
	bool ForwardDirectionCheck = ForwardDirection.Z < AllowedSlopeAngle && ForwardDirection.Z > AllowedSlopeAngle * -1.0f;

	if (RightDirectionCheck && ForwardDirectionCheck)
	{
		return true;
	}

	return false;
}

void ABiomeGenerator::SpawnTestPrimitives(FVector SpawnPosition)
{
	APrimitiveObject *PrimitiveSphere = GetWorld()->SpawnActor<APrimitiveObject>(SpawnPosition, FRotator::ZeroRotator);

	if (SpawnPosition.Z <= SeaLevel)
	{
		UE_LOG(LogTemp, Warning, TEXT("This position is below sea level"));
		PrimitiveSphere->SetMaterial(SeaLevelMaterial);
		return;
	}

	if (SpawnPosition.Z >= MaxAltitude)
	{
		UE_LOG(LogTemp, Warning, TEXT("This position is above max altitude"));
		PrimitiveSphere->SetMaterial(ColdMaterial);
		return;
	}
	else
	{
		PrimitiveSphere->SetMaterial(WarmMaterial);
	}

	SpawnedPrimitives.Add(PrimitiveSphere);
}

// Summary: Clears all arrays before regeneration of nodes
//
void ABiomeGenerator::ClearMaps()
{
	SpawnedTerrainFoliage.Empty();
	SpawnedPrimitives.Empty();
	ValidAvailablePositions.Empty();
}

void ABiomeGenerator::ClearOverlayNoise()
{
	SecondNoiseArray.Empty();
}

