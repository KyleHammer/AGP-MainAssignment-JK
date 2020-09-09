// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationNode.h"
#include "GameFramework/Character.h"
#include "AIManager.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "HealthComponent.h"
#include "EnemyCharacter.generated.h"

UENUM()
enum class AgentState : uint8 
{
	PATROL,
	ENGAGE,
	EVADE
};

UCLASS()
class ADVGAMESPROGRAMMING_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	UPROPERTY(EditAnywhere)
	float CuriositySensitivity;
	UPROPERTY(EditAnywhere)
	float ThreatSensitivity;

	UPROPERTY(VisibleAnywhere)
	float TotalCuriosity; //Likely between -1 - 2
	UPROPERTY(VisibleAnywhere)
	bool IsCurious;

	UPROPERTY(VisibleAnywhere)
	float TotalThreat; //Likely between -1 - 2
	UPROPERTY(VisibleAnywhere)
	bool IsThreatened;

	UPROPERTY(VisibleAnywhere)
	FVector LastKnownPosition;
	UPROPERTY(VisibleAnywhere)
	FAIStimulus LatestStimuli;

	UPROPERTY(VisibleAnywhere)
	float Age;
	UPROPERTY(VisibleAnywhere)
	float LastDistFromStimuli;

	UPROPERTY(EditAnywhere)
	float CuriousityThreshold;
	UPROPERTY(EditAnywhere)
	float ThreatThreshold;
	UPROPERTY(EditAnywhere)
	float AgentCritialAwarenessDistance;
	UPROPERTY(EditAnywhere)
	float LatestAge;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	TArray<ANavigationNode*> Path;
	UPROPERTY(VisibleAnywhere)
	ANavigationNode* CurrentNode;
	UPROPERTY(VisibleAnywhere)
	AAIManager* Manager;

	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* PerceptionComponent;

	UPROPERTY(VisibleAnywhere)
	AgentState CurrentAgentState;
	UPROPERTY(VisibleAnywhere)
	AActor* DetectedActor;
	UPROPERTY(VisibleAnywhere)
	bool bCanSeeActor;

	UHealthComponent* HealthComponent;

	UFUNCTION()
	void AgentPatrol();
	UFUNCTION()
	void AgentEngage();
	UFUNCTION()
	void AgentEvade();
	UFUNCTION()
	void SensePlayer(AActor* ActorSensed, FAIStimulus Stimulus);
	UFUNCTION(BlueprintImplementableEvent)
	void Fire(FVector FireDirection);

	//Responsible for curiosity
	void DetermineCuriosity();
	void CalculateCuriosity();

	//Responsible for curiosity
	void DetermineThreat();
	void CalculateThreat();

	void GroundValues();
	void IncrementValue();
	float FindDistance(FVector InitialLocation, FVector EndLocation);

	void FindLastDistToStimuli();
	float CalculateDistRatioToLastStimuli();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	void MoveAlongPath();

};
