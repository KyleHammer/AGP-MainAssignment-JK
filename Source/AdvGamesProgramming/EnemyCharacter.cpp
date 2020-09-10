// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include <string>
#include "EngineUtils.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetState(AgentState::PATROL);

	PathfindingNodeAccuracy = 100.0f;
	StartledDelay = 1.0f;
	StartledTurnSpeed = 550.0f;

	// Bool setup for states
	bPreviouslySeenPlayer = false;
	bIsDead = false;
	ResetStuckTimer();
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	PerceptionComponent = FindComponentByClass<UAIPerceptionComponent>();
	if (!PerceptionComponent) { UE_LOG(LogTemp, Error, TEXT("NO PERCEPTION COMPONENT FOUND")) }
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyCharacter::SensePlayer);
	
	HealthComponent = FindComponentByClass<UHealthComponent>();
	DetectedActor = nullptr;
	bCanSeeActor = false;
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckHealthForDeath();

	// If the enemy takes damage and hasn't seen the player in the past
	// switch to the startled state
	if(!bPreviouslySeenPlayer && HealthComponent->HealthPercentageRemaining() < 1.0f)
	{
		SetState(AgentState::STARTLED);
	}

	if(CurrentAgentState == AgentState::DEAD)
	{
		AgentDead();
		return;
	}
	
	if (CurrentAgentState == AgentState::PATROL)
	{
		AgentPatrol();
		if (bCanSeeActor && HealthComponent->HealthPercentageRemaining() >= 0.4f)
		{
			SetState(AgentState::ENGAGE);
			Path.Empty();
		}
		else if (bCanSeeActor && HealthComponent->HealthPercentageRemaining() < 0.4f)
		{
			SetState(AgentState::EVADE);
			Path.Empty();
		}
	}
	else if (CurrentAgentState == AgentState::ENGAGE)
	{
		AgentEngage();
		if (!bCanSeeActor && HealthComponent->HealthPercentageRemaining() >= 0.4f)
		{
			SetState(AgentState::CHASE);
		}
		else if(!bCanSeeActor)
		{
			SetState(AgentState::PATROL);
		}
		else if (bCanSeeActor && HealthComponent->HealthPercentageRemaining() < 0.4f)
		{
			SetState(AgentState::EVADE);
			Path.Empty();
		}
	}
	else if (CurrentAgentState == AgentState::EVADE)
	{
		AgentEvade();
		if (!bCanSeeActor)
		{
			SetState(AgentState::PATROL);
		}
		else if (bCanSeeActor && HealthComponent->HealthPercentageRemaining() >= 0.4f)
		{
			SetState(AgentState::ENGAGE);
			Path.Empty();
		}
	}
	else if (CurrentAgentState == AgentState::STARTLED)
	{
		AgentStartled();
		return;
	}
	else if (CurrentAgentState == AgentState::CHASE)
	{
		AgentChase();
		return;
	}
	else if (CurrentAgentState == AgentState::ENGAGEPIVOT)
	{
		AgentEngagePivot();
		return;
	}
	else if (CurrentAgentState == AgentState::INVESTIGATE)
	{
		return;
	}
	else if (CurrentAgentState == AgentState::RETRACESTEPS)
	{
		AgentRetraceSteps();
		return;
	}
	else if (CurrentAgentState == AgentState::MOVETOCLOSESTNODE)
	{
		AgentMoveToClosestNode();
	}
	MoveAlongPath();
}

// Whenever a state is changed, print the new state too
void AEnemyCharacter::SetState(AgentState NewState)
{
	CurrentAgentState = NewState;

	ResetStuckTimer();

	FString StateToString;
	switch (CurrentAgentState)
	{
	case 0:
		StateToString = "PATROL";
		break;
	case 1:
		StateToString = "ENGAGE";
		break;
	case 2:
		StateToString = "EVADE";
		break;
	case 3:
		StateToString = "DEAD";
		break;
	case 4:
		StateToString = "STARTLED";
		break;
	case 5:
		StateToString = "CHASE";
		break;
	case 6:
		StateToString = "ENGAGEPIVOT";
		break;
	case 7:
		StateToString = "INVESTIGATE";
		break;
	case 8:
		StateToString = "RETRACESTEPS";
		break;
	case 9:
		StateToString = "MOVETOCLOSESTNODE";
		break;
	default:
		StateToString = "UNKNOWN";
		break;
	}
		
	// Print new state to the screen
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("State switched to " + StateToString));
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::AgentPatrol()
{
	if (Path.Num() == 0 && Manager != NULL)
	{
		ResetStuckTimer();
		Path = Manager->GeneratePath(CurrentNode, Manager->AllNodes[FMath::RandRange(0, Manager->AllNodes.Num() - 1)]);
	}
	ReduceStuckTimer();
}

void AEnemyCharacter::AgentEngage()
{
	if (bCanSeeActor)
	{
		LastSeenLocation = DetectedActor->GetActorLocation();
		FVector DirectionToTarget = LastSeenLocation - GetActorLocation();
		Fire(DirectionToTarget);
		if (Path.Num() == 0)
		{
			Path = Manager->GeneratePath(CurrentNode, Manager->FindNearestNode(DetectedActor->GetActorLocation()));
		}
	}
	
}

void AEnemyCharacter::AgentEvade()
{
	if (bCanSeeActor)
	{
		FVector DirectionToTarget = DetectedActor->GetActorLocation() - GetActorLocation();
		Fire(DirectionToTarget);
		if (Path.Num() == 0)
		{
			Path = Manager->GeneratePath(CurrentNode, Manager->FindFurthestNode(DetectedActor->GetActorLocation()));
		}
	}
}

// Called in a couple different states to switch to evade when at low health
void AEnemyCharacter::EvadeAtLowHealth()
{
	if(HealthComponent->HealthPercentageRemaining() < 0.4f)
	{
		SetState(AgentState::EVADE);
	}
}

void AEnemyCharacter::AgentDead()
{
	// Death check. This ensures the following code is only run once
	if(!bIsDead)
	{
		bIsDead = true;
		
		// Remove enemy perception upon their death
		PerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &AEnemyCharacter::SensePlayer);
		
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("ENEMY HAS DIED"));

		PlayDeathAnimation();
	}
}

// Check of the enemy health is below 0
void AEnemyCharacter::CheckHealthForDeath()
{
	// No need to check health if the enemy is already dead
	if(CurrentAgentState == AgentState::DEAD)
	{
		return;
	}
	
	if(HealthComponent->HealthPercentageRemaining() <= 0.0f)
	{
		SetState(AgentState::DEAD);
	}
}

void AEnemyCharacter::AgentStartled()
{
	// If the enemy has not seen the player in the past, the following startled behaviour occurs
	if(!bPreviouslySeenPlayer)
	{
		// The enemy now identifies the player to be seen
		bPreviouslySeenPlayer = true;
		
		// Do a little jump to resemble being startled
		Jump();

		// Freeze the character for a certain time before switching out of startled state
		FTimerHandle MemberTimerHandle;
		GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AEnemyCharacter::ExitStartled, StartledDelay, false);
	}

	// Do a panicked rotation
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorRotation.Yaw += GetWorld()->GetDeltaSeconds() * StartledTurnSpeed;
	SetActorRotation(CurrentActorRotation);
}

// Enter engage state after exiting startled state
void AEnemyCharacter::ExitStartled()
{
	SetState(AgentState::ENGAGE);
}

void AEnemyCharacter::AgentChase()
{
	EvadeAtLowHealth();
	
	if(bCanSeeActor)
	{
		SetState(AgentState::ENGAGEPIVOT);
	}
	else
	{
		// Find the direction of the player's last seen location and move towards it
		FVector WorldDirection = LastSeenLocation - GetActorLocation();
		WorldDirection.Normalize();
		AddMovementInput(WorldDirection, 1.0f);

		FaceDirectionOfTravel(WorldDirection);
		
		if(FVector::Distance(GetActorLocation(), LastSeenLocation) < 100.0f)
		{
			SetState(AgentState::RETRACESTEPS);
		}
	}

	ReduceStuckTimer();
}

void AEnemyCharacter::AgentEngagePivot()
{
	EvadeAtLowHealth();
	
	if(bCanSeeActor)
	{
		LastSeenLocation = DetectedActor->GetActorLocation();
		FVector DirectionToTarget = LastSeenLocation - GetActorLocation();
		Fire(DirectionToTarget);
	}
	else
	{
		LocationBeforeChasing = GetActorLocation();
		SetState(AgentState::CHASE);
	}
	
}

void AEnemyCharacter::AgentRetraceSteps()
{
	// If the player is detected while retracing steps, initiate attack
	if(bCanSeeActor)
	{
		SetState(AgentState::ENGAGEPIVOT);
	}
	
	// Find the direction of their last location before chasing and move towards it
	FVector WorldDirection = LocationBeforeChasing - GetActorLocation();
	WorldDirection.Normalize();
	AddMovementInput(WorldDirection, 1.0f);

	FaceDirectionOfTravel(WorldDirection);

	// Find the closest node after retracing their steps
	if(FVector::Distance(GetActorLocation(), LocationBeforeChasing) < 100.0f)
	{
		// Empty the path before generating the new path
		Path.Empty();
		SetState(AgentState::MOVETOCLOSESTNODE);
	}

	ReduceStuckTimer();
}

void AEnemyCharacter::AgentMoveToClosestNode()
{
	// Find the closest node in the path
	if (Path.Num() == 0 && Manager != NULL)
	{
		Path = Manager->GeneratePath(CurrentNode, Manager->FindNearestNode(GetActorLocation()));
		
	}

	// Since the enemy is already moving towards the closest node, we
	// might as well enter engage state instead of engage pivot state
	if(bCanSeeActor)
	{
		SetState(AgentState::ENGAGE);
	}

	ReduceStuckTimer();
}

void AEnemyCharacter::SensePlayer(AActor* SensedActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		// Print message to the screen
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Player Detected"));
		
		DetectedActor = SensedActor;
		bCanSeeActor = true;
	
		bPreviouslySeenPlayer = true;
	}
	else
	{
		// Print message to the screen
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Player Lost"));
		
		bCanSeeActor = false;
	}
}

void AEnemyCharacter::MoveAlongPath()
{
	if (Path.Num() > 0 && Manager != NULL)
	{
		//UE_LOG(LogTemp, Display, TEXT("Current Node: %s"), *CurrentNode->GetName())
		if ((GetActorLocation() - CurrentNode->GetActorLocation()).IsNearlyZero(PathfindingNodeAccuracy))
		{
			//UE_LOG(LogTemp, Display, TEXT("At Node %s"), *CurrentNode->GetName())
			CurrentNode = Path.Pop();
			if(CurrentAgentState == AgentState::MOVETOCLOSESTNODE)
			{
				SetState(AgentState::PATROL);
			}
		}
		else
		{
			FVector WorldDirection = CurrentNode->GetActorLocation() - GetActorLocation();
			WorldDirection.Normalize();
			AddMovementInput(WorldDirection, 1.0f);

			FaceDirectionOfTravel(WorldDirection);
		}
	}
}

void AEnemyCharacter::FaceDirectionOfTravel(FVector WorldDirection)
{
	//Get the AI to face in the direction of travel.
	FRotator FaceDirection = WorldDirection.ToOrientationRotator();
	FaceDirection.Roll = 0.f;
	FaceDirection.Pitch = 0.f;
	SetActorRotation(FaceDirection);
}

void AEnemyCharacter::ReduceStuckTimer()
{
	// If a certain amount of time passes and the enemy hasn't switched their movement state
	// (i.e. they get stuck) switch states
	StuckTimer -= GetWorld()->GetDeltaSeconds();

	// Print stuck timer to the screen
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::White, FString::Printf(TEXT("Stuck Timer: %f"), StuckTimer));
	
	if(StuckTimer <= 0.0f)
	{
		ResetStuckTimer();
		
		UE_LOG(LogTemp, Warning, TEXT("Stuck timer ran out, switching states"));
		
		// Find the closest node to move to if stuck
		if(CurrentAgentState != AgentState::MOVETOCLOSESTNODE)
		{
			SetState(AgentState::MOVETOCLOSESTNODE);
		}
		// If stuck on finding the closest node, choose a random node through patrol instead
		else
		{
			SetState(AgentState::PATROL);
		}
	}
}

void AEnemyCharacter::ResetStuckTimer()
{
	StuckTimer = 7.0f;
}

