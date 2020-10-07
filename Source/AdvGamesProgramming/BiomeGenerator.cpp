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

	MaxAltitude = 200;
	NoisePowerValue = 2;
	SeaLevel = -10;
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
		
		//GenerateSecondNoiseMap(RowSize, ColSize, ProceduralGeneratedMap->PerlinRoughness, ProceduralGeneratedMap->PerlinScale);

		TestNoiseMapPositions(MapVerticies, ProceduralGeneratedMap->Width, ProceduralGeneratedMap->Height);
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

void ABiomeGenerator::TestNoiseMapPositions(TArray<FVector> Verticies, int Width, int Height)
{
	for (TActorIterator<APrimitiveObject> It(GetWorld()); It; ++It)
	{
		It->Destroy();
	}

	for (int Row = 0; Row < Height; Row++)
	{
		for (int Col = 0; Col < Width; Col++)
		{
			if (CheckValidSlopePosition(Verticies, Width, Height, Row, Col))
			{
				FVector Location = Verticies[Row * Width + Col];
				APrimitiveObject *PrimitiveSphere = GetWorld()->SpawnActor<APrimitiveObject>(Location, FRotator::ZeroRotator, FActorSpawnParameters());
				DetermineBiomePrimitive(PrimitiveSphere, Location.Z, Location.Z);
				SpawnedPrimitives.Add(PrimitiveSphere);
			}

			/*FVector Location = Verticies[Row * Width + Col];
			APrimitiveObject *PrimitiveSphere = GetWorld()->SpawnActor<APrimitiveObject>(Location, FRotator::ZeroRotator, FActorSpawnParameters());
			DetermineBiomePrimitive(PrimitiveSphere, Location.Z, Location.Z);
			SpawnedPrimitives.Add(PrimitiveSphere);*/
		}
	}

	/*for (int MapPoint = 0; MapPoint < Verticies.Num() - 1; MapPoint++)
	{
		FVector Location = FVector(Verticies[MapPoint].X, Verticies[MapPoint].Y, Verticies[MapPoint].Z); // Currently is displaying the second generated map
		


		//First check for valid angle between neighboiring nodes
		
		APrimitiveObject* PrimitiveSphere = GetWorld()->SpawnActor<APrimitiveObject>(Location, FRotator::ZeroRotator, FActorSpawnParameters());
		DetermineBiomePrimitive(PrimitiveSphere, Location.Z, Location.Z);
		SpawnedPrimitives.Add(PrimitiveSphere);
	}*/
}

void ABiomeGenerator::DetermineBiomePrimitive(APrimitiveObject* PrimitiveSphere, float NoisePointOne, float NoisePointTwo)
{
	//UE_LOG(LogTemp, Warning, TEXT("first: %f, secnd: %f"), NoisePointOne, NoisePointTwo);

	//Show blue indicating sea level
	if (NoisePointOne <= SeaLevel) {
		PrimitiveSphere->SetMaterial(SeaLevelMaterial);
		return;
	}

	//Show light blue indicating high altitude
	if (NoisePointOne >= MaxAltitude) {
		PrimitiveSphere->SetMaterial(ColdMaterial);
		//UE_LOG(LogTemp, Warning, TEXT("first: %f, secnd: %f"), NoisePointOne, NoisePointTwo);
	}
	else
	{
		PrimitiveSphere->SetMaterial(WarmMaterial);
	}
}

// Summary: like
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
	else {
		if (CheckValidAngle(Verticies[Row * Width + Col], Verticies[Row * Width + (Col + 1)], Verticies[(Row - 1) * Width + Col]))
		{
			return CheckValidAngle(Verticies[Row * Width + Col], Verticies[Row * Width + (Col - 1)], Verticies[(Row + 1) * Width + Col]);
		}
	}

	return false;
}

// Summary checks the angle from the current to the next node
bool ABiomeGenerator::CheckValidAngle(FVector CurrentNode, FVector ToRightNode, FVector ToForwardNode)
{
	FVector Direction = CurrentNode - ToRightNode;
	FVector ForwardDirection = CurrentNode - ToForwardNode;

	Direction.Normalize();
	ForwardDirection.Normalize();

	UE_LOG(LogTemp, Warning, TEXT("Identified Direction At: %f"), Direction.Z);

	//Checks for the angle between nodes in both positive and negative
	bool RightDirectionCheck = Direction.Z < AllowedSlopeAngle && Direction.Z > AllowedSlopeAngle * -1.0f;
	bool ForwardDirectionCheck = ForwardDirection.Z < AllowedSlopeAngle && ForwardDirection.Z > AllowedSlopeAngle * -1.0f;

	if (RightDirectionCheck && ForwardDirectionCheck)
	{
		return true;
	}

	return false;
}


void ABiomeGenerator::ClearMaps()
{
	SpawnedPrimitives.Empty();
	InitialHeightMap.Empty();
	SecondHeightMap.Empty();
}

