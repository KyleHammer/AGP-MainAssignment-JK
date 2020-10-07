// Fill out your copyright notice in the Description page of Project Settings.


#include "PlantTerrainActor.h"

// Sets default values
APlantTerrainActor::APlantTerrainActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SelectedPlantType = PlantType::TREE;

	LocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Location Component"));

	//LocationComponent = FindComponentByClass<USceneComponent>();
	if (!LocationComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("NO PERCEPTION COMPONENT FOUND"))
	}
	RootComponent = LocationComponent;
	LocationComponent->SetWorldScale3D(FVector(1.5, 1.5, 1.5));
	LocationComponent->bVisualizeComponent = true;

	PlantMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere Mesh"));

	//FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	PlantMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
}

// Called when the game starts or when spawned
void APlantTerrainActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlantTerrainActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlantTerrainActor::SetPosition(FVector NewPosition)
{
	if (LocationComponent == NULL) return;

	LocationComponent->SetWorldLocation(NewPosition);
}

void APlantTerrainActor::SetScale(FVector NewScale)
{
	if (LocationComponent == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("Contains no location component"));
		return;
	}

	LocationComponent->SetWorldScale3D(NewScale);
}

