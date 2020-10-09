// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/Material.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "PrimitiveObject.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API APrimitiveObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APrimitiveObject();

	UPROPERTY(EditAnywhere, Category="Mesh")
	UStaticMeshComponent* SphereMesh;

	USceneComponent *LocationComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetMaterial(UMaterial* PrimitiveMaterial);

};
