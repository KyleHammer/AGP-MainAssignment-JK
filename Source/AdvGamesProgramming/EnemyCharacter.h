// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationNode.h"
#include "GameFramework/Character.h"
#include "Engine/Engine.h"
#include "AIManager.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "HealthComponent.h"
#include "TimerManager.h"
#include "EnemyCharacter.generated.h"

UENUM()
enum class AgentState : uint8 
{
	PATROL,
	ENGAGE,
	EVADE,
	DEAD,
	STARTLED,
	CHASE,
	ENGAGEPIVOT,
	INVESTIGATE,
	RETRACESTEPS,
	MOVETOCLOSESTNODE
};

UCLASS()
class ADVGAMESPROGRAMMING_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	//Sensitivity values
	UPROPERTY(EditAnywhere)
	float CuriositySensitivity;
	UPROPERTY(EditAnywhere)
	float ThreatSensitivity;

	//Curiosity metric
	UPROPERTY(VisibleAnywhere)
	float TotalCuriosity; //Likely between 0 - 100
	UPROPERTY(VisibleAnywhere)
	bool IsCurious;

	//Threat metric
	UPROPERTY(VisibleAnywhere)
	float TotalThreat; //Likely between 0 - 100
	UPROPERTY(VisibleAnywhere)
	bool IsThreatened;

	//Threshold values for the metric bool variables
	UPROPERTY(EditAnywhere)
	float CuriousityThreshold;
	UPROPERTY(EditAnywhere)
	float ThreatThreshold;

	//Minimum focus radius
	UPROPERTY(EditAnywhere)
	float MinimumFocusRadius;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AgentState CurrentAgentState;
	
	UPROPERTY(VisibleAnywhere)
	AActor* DetectedActor;
	UPROPERTY(VisibleAnywhere)
	bool bCanSeeActor;

	UHealthComponent* HealthComponent;

	UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
	void PlayDeathAnimation();

	UFUNCTION()
	void AgentPatrol();
	UFUNCTION()
	void AgentEngage();
	UFUNCTION()
	void AgentEvade();
	UFUNCTION()
	void AgentDead();
	UFUNCTION()
    void AgentStartled();
	void ExitStartled();
	UFUNCTION()
	void AgentChase();
	UFUNCTION()
    void AgentEngagePivot();
	UFUNCTION()
    void AgentInvestigate();
	void ExitInvestigation();
	UFUNCTION()
    void AgentRetraceSteps();
	UFUNCTION()
    void AgentMoveToClosestNode();
	UFUNCTION()
	void SensePlayer(AActor* ActorSensed, FAIStimulus Stimulus);
	UFUNCTION(BlueprintImplementableEvent)
	void Fire(FVector FireDirection);

	//Primary method for processing sound stimuli
	void ProcessSoundEvent(FAIStimulus Stimulus);

	//Responsible for managing the curiosity values
	void DetermineCuriosity();
	void CalculateCuriosity();

	//Responsible for managing the threat values
	void DetermineThreat();
	void CalculateThreat();

	//Called to clamp threat and curiosity float values from 0 - 100
	void ClampValues();

	//Called to determine the distance between two vector locations
	float FindDistance(FVector InitialLocation, FVector EndLocation);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	ANavigationNode* LastSeenLocationNode;
	
	UPROPERTY(EditAnywhere, meta=(UIMin = "10.0", UIMax = "1000.0", ClampMin = "10.0", ClampMax = "1000.0"))
	float PathfindingNodeAccuracy;

	UFUNCTION(BlueprintCallable)
	void SetState(AgentState NewState);

private:

	FVector LastSeenLocation;
	FVector LocationBeforeChasing;
	FVector LastFrameEnemyLocation;

	bool bPreviouslySeenPlayer;
	bool bIsDead;
	bool bStartingInvestigation;
	
	float StartledDelay;
	float StartledTurnSpeed;
	float InvestigationDelay;
	float InvestigateTurnSpeed;
	float StuckTimer;
	float LastFrameHealth;
	
	void MoveAlongPath();
	void CheckHealthForDeath();
	void EvadeAtLowHealth();
	void MoveTowardsPoint(FVector LocationToMoveTo);
	void ReduceStuckTimer();
	void ResetStuckTimer();
	void InvestigateOnDamage();
};
