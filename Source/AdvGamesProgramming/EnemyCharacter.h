// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationNode.h"
#include "GameFramework/Character.h"
#include "Engine/Engine.h"
#include "AIManager.h"
#include "Perception/AIPerceptionComponent.h"
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
	ENGAGEPIVOT
};

UCLASS()
class ADVGAMESPROGRAMMING_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

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
	UPROPERTY(VisibleAnywhere)
	bool bPreviouslySeenPlayer;
	UPROPERTY(VisibleAnywhere)
	bool bIsDead;
	UPROPERTY(VisibleAnywhere)
	float StartledDelay;
	UPROPERTY(VisibleAnywhere)
	float StartledTurnSpeed;

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
	UFUNCTION()
	void ExitStartled();
	UFUNCTION()
	void AgentChase();
	UFUNCTION()
    void AgentEngagePivot();
	UFUNCTION()
	void SensePlayer(AActor* ActorSensed, FAIStimulus Stimulus);
	UFUNCTION(BlueprintImplementableEvent)
	void Fire(FVector FireDirection);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FVector LastSeenLocation;
	ANavigationNode* LastSeenLocationNode;
	
	UPROPERTY(EditAnywhere, meta=(UIMin = "10.0", UIMax = "1000.0", ClampMin = "10.0", ClampMax = "1000.0"))
	float PathfindingNodeAccuracy;

private:

	void SetState(AgentState NewState);
	void MoveAlongPath();
	void CheckHealthForDeath();

};
