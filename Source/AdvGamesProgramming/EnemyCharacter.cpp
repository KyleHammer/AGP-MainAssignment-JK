// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "EngineUtils.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetState(AgentState::PATROL);
	PreviousAgentState = CurrentAgentState;

	PathfindingNodeAccuracy = 100.0f;
	StartledDelay = 1.0f;
	StartledTurnSpeed = 550.0f;
	bPreviouslySeenPlayer = false;
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

	PreviousAgentState = CurrentAgentState;

	CheckHealthForDeath();

	// If the enemy takes damage and hasn't seen the player in the past
	// switch to the startled state
	if(!bPreviouslySeenPlayer && HealthComponent->HealthPercentageRemaining() < 1.0f)
	{
		bPreviouslySeenPlayer = true;
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
		if (!bCanSeeActor)
		{
			SetState(AgentState::PATROL);
			//SetState(AgentState::CHASE);
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
	MoveAlongPath();
}

// Whenever a state is changed, print the new state too
void AEnemyCharacter::SetState(AgentState NewState)
{
	CurrentAgentState = NewState;

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
	default:
		StateToString = "UNKNOWN";
		break;
	}
		
	// Print new state to the screen
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("State switched to " + StateToString));
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


// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::AgentPatrol()
{
	if (Path.Num() == 0 && Manager != NULL)
	{
		Path = Manager->GeneratePath(CurrentNode, Manager->AllNodes[FMath::RandRange(0, Manager->AllNodes.Num() - 1)]);
	}
}

void AEnemyCharacter::AgentEngage()
{
	if (bCanSeeActor)
	{
		FVector DirectionToTarget = DetectedActor->GetActorLocation() - GetActorLocation();
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

void AEnemyCharacter::AgentDead()
{
	// If the last state wasn't dead, perform the following actions
	// This ensures the following code is only run once
	if(PreviousAgentState != AgentState::DEAD)
	{
		// Remove enemy perception upon their death
		PerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &AEnemyCharacter::SensePlayer);
		
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("ENEMY HAS DIED"));

		PlayDeathAnimation();
	}
}

void AEnemyCharacter::AgentStartled()
{
	// If the last state wasn't startled, perform the following actions
	// This ensures the following code is only run once
	if(PreviousAgentState != AgentState::STARTLED)
	{
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
	if(PreviousAgentState != AgentState::CHASE)
	{
		// ACTIONS HERE
	}
	// OTHER ACTIONS HERE
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
			UE_LOG(LogTemp, Display, TEXT("At Node %s"), *CurrentNode->GetName())
			CurrentNode = Path.Pop();
		}
		else
		{
			FVector WorldDirection = CurrentNode->GetActorLocation() - GetActorLocation();
			WorldDirection.Normalize();
			AddMovementInput(WorldDirection, 1.0f);

			//Get the AI to face in the direction of travel.
			FRotator FaceDirection = WorldDirection.ToOrientationRotator();
			FaceDirection.Roll = 0.f;
			FaceDirection.Pitch = 0.f;
			SetActorRotation(FaceDirection);
		}
	}
}

