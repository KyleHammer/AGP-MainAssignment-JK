// Fill out your copyright notice in the Description page of Project Settings.


#include "WallActor.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/EngineTypes.h"

// Sets default values
AWallActor::AWallActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Location Component"));
	LocationComponent->SetWorldScale3D(FVector(1, 1, 1));
	RootComponent = LocationComponent;

	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube Mesh"));
	CubeMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	//Assigns mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/EngineMeshes/Cube"));

	if (CubeMeshAsset.Succeeded())
	{
		CubeMesh->SetStaticMesh(CubeMeshAsset.Object);
	}
}

// Called when the game starts or when spawned
void AWallActor::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void AWallActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWallActor::SetLocalScale(FVector Scale)
{
	LocationComponent->SetWorldScale3D(Scale);
}
