// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedurallyGeneratedMap.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"

// Sets default values
AProcedurallyGeneratedMap::AProcedurallyGeneratedMap()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh Component"));
	PerlinScale = 1000.0f;
	PerlinRoughness = 0.1f;
	bRegenerateMap = false;
}

// Called when the game starts or when spawned
void AProcedurallyGeneratedMap::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProcedurallyGeneratedMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRegenerateMap)
	{
		ClearMap();
		GenerateMap();
		bRegenerateMap = false;
	}

	//This will generate walls that span on each edge of the map.
	if(bGenerateWalls)
	{
		GenerateWalls();
		bGenerateWalls = false;
	}
}

bool AProcedurallyGeneratedMap::ShouldTickIfViewportsOnly() const
{
	return true;
}

void AProcedurallyGeneratedMap::GenerateMap()
{
	float PerlinOffset = FMath::RandRange(-10000.0f, 10000.0f);

	for (int Row = 0; Row < Height; Row++)
	{
		for (int Col = 0; Col < Width; Col++)
		{
			float X = Col * GridSize;
			float Y = Row * GridSize;
			float Z = FMath::PerlinNoise2D(FVector2D(float(Col) * PerlinRoughness + PerlinOffset, float(Row) * PerlinRoughness + PerlinOffset)) * PerlinScale;
			Vertices.Add(FVector(X, Y, Z));
			UVCoords.Add(FVector2D(Col, Row));
		}
	}

	for (int Row = 0; Row < Height - 1; Row++)
	{
		for (int Col = 0; Col < Width - 1; Col++)
		{
			Triangles.Add(Row * Width + Col);
			Triangles.Add((Row + 1) * Width + Col);
			Triangles.Add((Row * Width) + Col + 1);
			Triangles.Add((Row * Width) + Col + 1);
			Triangles.Add((Row + 1) * Width + Col);
			Triangles.Add((Row + 1) * Width + Col + 1);
		}
	}

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVCoords, Normals, Tangents);

	MeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UVCoords, TArray<FColor>(), Tangents, true);

	if (AIManager)
	{
		AIManager->GenerateNodes(Vertices, Width, Height);
	}
}

void AProcedurallyGeneratedMap::GenerateWalls()
{
	MapWalls.Empty(); //Empties list references

	//Destroys existing instances of walls
	for (TActorIterator<AWallActor> It(GetWorld()); It; ++It)
	{
		It->Destroy();
	}

	FVector SpawnPosition;
	FVector SpawnScale = FVector(Width, 1, WallHeight);

	//First Wall
	SpawnPosition = FVector((Width * GridSize) / 2, 0 - (GridSize / 2), 0);
	AWallActor *FirstWall = GetWorld()->SpawnActor<AWallActor>(WallToSpawn, SpawnPosition, FRotator(0.f, 0.f, 0.f));
	FirstWall->SetLocalScale(SpawnScale);

	//Second Wall
	SpawnPosition = FVector((Width * GridSize) - (GridSize/2), (Width * GridSize) / 2, 0);
	AWallActor *SecondWall = GetWorld()->SpawnActor<AWallActor>(WallToSpawn, SpawnPosition, FRotator(0.f, 90.f, 0.f));
	SecondWall->SetLocalScale(SpawnScale);

	//Third Wall
	SpawnPosition = FVector((Width * GridSize) / 2, (Width * GridSize) - (GridSize / 2), 0);
	AWallActor *ThirdWall = GetWorld()->SpawnActor<AWallActor>(WallToSpawn, SpawnPosition, FRotator(0.f, 0.f, 0.f));
	ThirdWall->SetLocalScale(SpawnScale);

	//Fourth Wall
	SpawnPosition = FVector(0 - (GridSize / 2), (Width * GridSize) / 2, 0);
	AWallActor *FourthWall = GetWorld()->SpawnActor<AWallActor>(WallToSpawn, SpawnPosition, FRotator(0.f, 90.f, 0.f));
	FourthWall->SetLocalScale(SpawnScale);

	MapWalls.Add(FirstWall);
	MapWalls.Add(SecondWall);
	MapWalls.Add(ThirdWall);
	MapWalls.Add(FourthWall);
}

void AProcedurallyGeneratedMap::ClearMap()
{
	Vertices.Empty();
	Triangles.Empty();
	UVCoords.Empty();
	MapWalls.Empty();
	MeshComponent->ClearAllMeshSections();
}
