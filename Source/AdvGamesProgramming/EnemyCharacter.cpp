// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include <string>
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetState(AgentState::PATROL);

	PathfindingNodeAccuracy = 100.0f;
	StartledDelay = 1.0f;
	StartledTurnSpeed = 550.0f;
	InvestigationDelay = 3.0f;
	InvestigateTurnSpeed = 250.0f;

	// Bool setup for states
	bCanBeStartled = true;
	bIsDead = false;
	bStartingInvestigation = false;
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
	LastFrameHealth = HealthComponent->CurrentHealth;
	DetectedActor = nullptr;
	bCanSeeActor = false;
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Handles related decision values
	DetermineCuriosity();
	DetermineThreat();
	ClampValues();

	CheckHealthForDeath();

	LastFrameDamageCheck();

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
		AgentInvestigate();
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
	case AgentState(0):
		StateToString = "PATROL";
		break;
	case AgentState(1):
		StateToString = "ENGAGE";
		break;
	case AgentState(2):
		StateToString = "EVADE";
		break;
	case AgentState(3):
		StateToString = "DEAD";
		break;
	case AgentState(4):
		StateToString = "STARTLED";
		break;
	case AgentState(5):
		StateToString = "CHASE";
		break;
	case AgentState(6):
		StateToString = "ENGAGEPIVOT";
		break;
	case AgentState(7):
		StateToString = "INVESTIGATE";
		break;
	case AgentState(8):
		StateToString = "RETRACESTEPS";
		break;
	case AgentState(9):
		StateToString = "MOVETOCLOSESTNODE";
		break;
	default:
		StateToString = "UNKNOWN";
		break;
	}
		
	// Print new state to the screen and ue_log
	UE_LOG(LogTemp, Display, TEXT("Enemy States >> Switched to %s"), *StateToString);
	
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("State switched to " + StateToString));
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
		FireIfThreatened(DirectionToTarget);
		
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
		// FVector DirectionToTarget = DetectedActor->GetActorLocation() - GetActorLocation();
		// FireIfThreatened(DirectionToTarget);
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
	if(bCanBeStartled)
	{
		bCanBeStartled = false;
		
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
	bCanBeStartled = true;
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
		MoveTowardsPoint(LastSeenLocation);
		
		if(FVector::Distance(GetActorLocation(), LastSeenLocation) < 100.0f)
		{
			bStartingInvestigation = true;
			SetState(AgentState::INVESTIGATE);
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
		FireIfThreatened(DirectionToTarget);
	}
	else
	{
		LocationBeforeChasing = GetActorLocation();
		SetState(AgentState::CHASE);
	}
	
}

void AEnemyCharacter::AgentInvestigate()
{
	
	if(bStartingInvestigation)
	{
		// Freeze the character for a certain time before switching out of startled state
		FTimerHandle MemberTimerHandle;
		GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AEnemyCharacter::ExitInvestigation, InvestigationDelay, true);

		bStartingInvestigation = false;
	}

	// Start attacking if player is found
	if(bCanSeeActor)
	{
		SetState(AgentState::ENGAGEPIVOT);
	}

	// Do a slow turn investigation
	FRotator CurrentActorRotation = GetActorRotation();
	CurrentActorRotation.Yaw += GetWorld()->GetDeltaSeconds() * InvestigateTurnSpeed;
	SetActorRotation(CurrentActorRotation);
}

void AEnemyCharacter::ExitInvestigation()
{
	// Only set the state if still investigating
	if(CurrentAgentState == AgentState::INVESTIGATE)
	{
		SetState(AgentState::RETRACESTEPS);
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
	MoveTowardsPoint(LocationBeforeChasing);

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
		DetectedActor = SensedActor;

		//Checks if stimulus contains tag gun
		if(Stimulus.Tag.ToString() == "Gun")
		{
			UE_LOG(LogTemp, Display, TEXT("Stimulus: %s"), Stimulus.Tag.ToString().Contains("Gun") ? TEXT("True") : TEXT("False"));
			ProcessSoundEvent(Stimulus);
		}
		else
		{
			if(GEngine) {
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Player Seen"));
			}
			
			bCanSeeActor = true;
		}
	}
	else
	{
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Player Lost"));
		
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
			MoveTowardsPoint(CurrentNode->GetActorLocation());
		}
	}
}


void AEnemyCharacter::MoveTowardsPoint(FVector LocationToMoveTo)
{
	//Find the direction of travel
	FVector WorldDirection = LocationToMoveTo - GetActorLocation();
	WorldDirection.Normalize();
	AddMovementInput(WorldDirection, 1.0f);

	RotateTowardsPoint(WorldDirection);
}

void AEnemyCharacter::RotateTowardsPoint(FVector faceDirection)
{
	//Get the AI to face in the direction of travel.
	FRotator FaceDirection = faceDirection.ToOrientationRotator();
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

void AEnemyCharacter::LastFrameDamageCheck()
{
	if(LastFrameHealth != HealthComponent->CurrentHealth)
	{
		// If player isn't seen, the enemy can still be startled and has a low threat level
		if(!bCanSeeActor && bCanBeStartled && TotalThreat < 10.0f)
		{
			SetState(AgentState::STARTLED);
		}
		// Turn around using the investigate state if player cannot be seen
		else if(!bCanSeeActor)
		{
			bStartingInvestigation = true;
			SetState(AgentState::INVESTIGATE);
		}
	}
	LastFrameHealth = HealthComponent->CurrentHealth;
}
/////////////////////////////////////////////////////////////////////////////
// RELATED METHODS FOR SENSE ADJUSTMENT


//Called to run determination and calculation of curiosity
void AEnemyCharacter::DetermineCuriosity()
{
	if (DetectedActor == nullptr)
		return;

	if (bCanSeeActor)
	{
		CalculateCuriosity();
		IsCurious = TotalCuriosity > CuriousityThreshold ? true : false;

		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT(">> EnemyCharacter: Total Curiosity at = %f"), TotalCuriosity));
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::White, FString::Printf(TEXT(">> EnemyCharacter: Is Curious = %s"), IsCurious ? TEXT("True") : TEXT("False")));
	}
	else
	{
		TotalCuriosity -= 1;
		IsCurious = false;
	}
}

// Calculates the curiosity metric for the AI
void AEnemyCharacter::CalculateCuriosity()
{
	FVector StimuliDirection = DetectedActor->GetActorLocation() - GetActorLocation();
	float DotResult = FVector::DotProduct(GetActorForwardVector().GetSafeNormal(1), StimuliDirection.GetSafeNormal(1));

	// Displays the dot product on screen (1 = Forward,  -1 = Back)
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, FString::Printf(TEXT("Dot Product: %f"), DotResult));

	if (DotResult <= 0)
		return;

	// Check within peripheral view
	if (DotResult >= 0.7)
	{
		TotalCuriosity += 0.5 * CuriositySensitivity;
		return;
	}

	// Check within focus distance
	float Distance = FVector::Dist(DetectedActor->GetActorLocation(), GetActorLocation());
	if (Distance < MinimumFocusRadius)
	{
		TotalCuriosity += 0.4 * CuriositySensitivity;
		return;
	}

	//Base increment for curiosity when stimuli is detected
	TotalCuriosity += 0.05 * CuriositySensitivity;
	return;
}


//Called to determine threat
void AEnemyCharacter::DetermineThreat()
{
	if (DetectedActor == nullptr) 
		return;

	if (bCanSeeActor)
	{
		CalculateThreat();
		IsThreatened = TotalThreat > ThreatThreshold ? true : false;

		//Prints messages to screen
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT(">> EnemyCharacter: Total Threat at = %f"), TotalThreat));
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, FString::Printf(TEXT(">> EnemyCharacter: Is Threat = %s"), IsThreatened ? TEXT("True") : TEXT("False")));
	}
	else
	{
		TotalThreat -= 1;
		IsThreatened = false;
	}
}


// Called to Calculate Threat
void AEnemyCharacter::CalculateThreat()
{
	FVector StimuliDirection = DetectedActor->GetActorLocation() - GetActorLocation();
	float DotResult = FVector::DotProduct(GetActorForwardVector().GetSafeNormal(1), StimuliDirection.GetSafeNormal(1));
	
	if (DotResult <= 0)
		return;

	// Check within focused sight
	if (DotResult >= 0.8)
	{
		// Check within focused distance
		float Distance = FVector::Dist(DetectedActor->GetActorLocation(), GetActorLocation());
		if (Distance < MinimumFocusRadius)
		{
			TotalCuriosity -= 1;
			TotalThreat += 0.8;
			return;
		}

		TotalThreat += 0.2;
	}
}

// Called to clamp metric values
void AEnemyCharacter::ClampValues()
{
	TotalCuriosity = FMath::Clamp(TotalCuriosity, float(0), float(100));
	TotalThreat = FMath::Clamp(TotalThreat, float(0), float(100));
}

// Called to process incoming sound events
void AEnemyCharacter::ProcessSoundEvent(FAIStimulus Stimulus)
{
	if (Stimulus.Tag.ToString() == "Gun")
	{
		TotalThreat += 70;
		TotalCuriosity += 30;

		//Prints message to the output log
		UE_LOG(LogTemp, Display, TEXT(">> Enemy Character: Sound is a gun"))
		UE_LOG(LogTemp, Display, TEXT(">> Enemy Character: Sound location at x:%f, y:%f"), Stimulus.StimulusLocation.X, Stimulus.StimulusLocation.Y)
	}
}

void AEnemyCharacter::FireIfThreatened(FVector DirectionToTarget)
{
	if(IsThreatened && bCanSeeActor)
	{
		Fire(DirectionToTarget);
	}
	else
	{
		RotateTowardsPoint(DetectedActor->GetActorLocation() - GetActorLocation());
	}
}

