// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimitiveObject.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/EngineTypes.h"

// Sets default values
APrimitiveObject::APrimitiveObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Location Component"));
	LocationComponent->SetWorldScale3D(FVector(0.2, 0.2, 0.2));
	RootComponent = LocationComponent;

	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	//FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	SphereMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	//Assigns mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/EngineMeshes/Sphere"));

	if (SphereMeshAsset.Succeeded())
	{
		SphereMesh->SetStaticMesh(SphereMeshAsset.Object);
	}
}

// Called when the game starts or when spawned
void APrimitiveObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APrimitiveObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APrimitiveObject::SetMaterial(UMaterial* PrimitiveMaterial)
{
	UStaticMesh* Mesh = SphereMesh->GetStaticMesh();

	if (Mesh != NULL)
	{
		Mesh->SetMaterial(0, PrimitiveMaterial);
	}
}

