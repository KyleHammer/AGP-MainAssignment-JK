// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AdvGamesProgramming/EnemyCharacter.h"
#include "AIVision.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVGAMESPROGRAMMING_API UAIVision : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAIVision();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vision")
	float NormalViewAngle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vision")
	float NormalViewDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vision")
	float FocusedViewDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vision")
	float PeripheralViewAngle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vision")
	float PeripheralViewDistance;

	UPROPERTY(VisibleAnywhere)
	AEnemyCharacter* EnemyCharacterComponent;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ConductSearch();
	void TraceRayLine();
	void CreateNormalViewCone();

	//Responsible for curiosity
	UFUNCTION()
	void DetermineCuriosity();
	void CalculateCuriosity();

	//Responsible for curiosity
	UFUNCTION()
	void DetermineThreat();
	void CalculateThreat();

	UFUNCTION()
	void GroundValues();
	void IncrementValue();
	float FindDistance(FVector InitialLocation, FVector EndLocation);

	void FindLastDistToStimuli();
	float CalculateDistRatioToLastStimuli();

};
