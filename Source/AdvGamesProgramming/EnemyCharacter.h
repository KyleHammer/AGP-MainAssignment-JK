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
	float TotalCuriosity; //Likely between 0 - 100
	UPROPERTY(VisibleAnywhere)
	bool IsCurious;

	UPROPERTY(VisibleAnywhere)
	float TotalThreat; //Likely between 0 - 100
	UPROPERTY(VisibleAnywhere)
	bool IsThreatened;

	UPROPERTY(EditAnywhere)
	float CuriousityThreshold;
	UPROPERTY(EditAnywhere)
	float ThreatThreshold;
	UPROPERTY(EditAnywhere)
	float MinimumAwarenessRadius;

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

private:

	void MoveAlongPath();

};
