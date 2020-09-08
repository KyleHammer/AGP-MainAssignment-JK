// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentAgentState = AgentState::PATROL;
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

	//LatestStimuli = nullptr;
	LastKnownPosition = FVector::ZeroVector;
	Age = 0;
	LastDistFromStimuli = 0;

	CurrentCuriosity = 0;

	//Should remove later on
	CuriositySensitivity = 1;
	ThreatSensitivity = 1;
	MinimumAwarenessDist = 400;
	LatestAge = 10;
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DetermineCuriosity();

	/*if (CurrentAgentState == AgentState::PATROL)
	{
		AgentPatrol();
		if (bCanSeeActor && HealthComponent->HealthPercentageRemaining() >= 0.4f)
		{
			CurrentAgentState = AgentState::ENGAGE;
			Path.Empty();
		}
		else if (bCanSeeActor && HealthComponent->HealthPercentageRemaining() < 0.4f)
		{
			CurrentAgentState = AgentState::EVADE;
			Path.Empty();
		}
	}
	else if (CurrentAgentState == AgentState::ENGAGE)
	{
		AgentEngage();
		if (!bCanSeeActor)
		{
			CurrentAgentState = AgentState::PATROL;
		}
		else if (bCanSeeActor && HealthComponent->HealthPercentageRemaining() < 0.4f)
		{
			CurrentAgentState = AgentState::EVADE;
			Path.Empty();
		}
	}
	else if (CurrentAgentState == AgentState::EVADE)
	{
		AgentEvade();
		if (!bCanSeeActor)
		{
			CurrentAgentState = AgentState::PATROL;
		}
		else if (bCanSeeActor && HealthComponent->HealthPercentageRemaining() >= 0.4f)
		{
			CurrentAgentState = AgentState::ENGAGE;
			Path.Empty();
		}
	}
	MoveAlongPath();*/
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

void AEnemyCharacter::SensePlayer(AActor* SensedActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Detected"))
		DetectedActor = SensedActor;
		bCanSeeActor = true;
		LastKnownPosition = Stimulus.StimulusLocation;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Lost"))
		bCanSeeActor = false;
		LatestStimuli = Stimulus;
		LastKnownPosition = Stimulus.StimulusLocation;
	}
}

void AEnemyCharacter::MoveAlongPath()
{
	if (Path.Num() > 0 && Manager != NULL)
	{
		//UE_LOG(LogTemp, Display, TEXT("Current Node: %s"), *CurrentNode->GetName())
		if ((GetActorLocation() - CurrentNode->GetActorLocation()).IsNearlyZero(500.0f))
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



/////////////////////////////////////////////////////////////////////////////
// RELATED METHODS FOR SENSE ADJUSTMENT

void AEnemyCharacter::DetermineCuriosity()
{
	if (bCanSeeActor) 
	{
		//UE_LOG(LogTemp, Display, TEXT(">> EnemyCharacter: PlayerDetected"))
		CalculateCuriosity();
		//UE_LOG(LogTemp, Display, TEXT(">> EnemyCharacter: Curiosity at = %f"), CurrentCuriosity)
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Blue, FString::Printf(TEXT(">> EnemyCharacter: Curiosity at = %f"), CurrentCuriosity));
	} 
	else 
	{
		CurrentCuriosity = 0;
	}

	//TotalCuriosity = (((CurrentCuriosity * CuriositySensitivity)) * CalculateDistRatioToLastStimuli()) / 100;
	//UE_LOG(LogTemp, Display, TEXT(">> EnemyCharacter: Total Curiosity at = %s"), TotalCuriosity)
}

void AEnemyCharacter::CalculateCuriosity()
{
	FVector StimuliDiretion = DetectedActor->GetActorLocation() - GetActorLocation();
	float DotResult = FVector::DotProduct(GetActorForwardVector().GetSafeNormal(1), StimuliDiretion.GetSafeNormal(1));
	
	//Check  within  peripheral view
	if (DotResult >= 0.7)
	{
		CurrentCuriosity += 0.2;

		float Distance = FVector::Dist(DetectedActor->GetActorLocation(), GetActorLocation());
			//Check within focused distance
	} else {
		CurrentCuriosity += 0.1;
	}


	//UE_LOG(LogTemp, Display, TEXT(">> EnemyCharacter: Dot product at = %f"), DotResult)
	return;
}

void AEnemyCharacter::CalculateThreat()
{

}


void AEnemyCharacter::DetermineThreat() 
{

}

// Summary: Finds the ratio from character's initial distance to its current to the last stimuli source
//
float AEnemyCharacter::CalculateDistRatioToLastStimuli()
{
	return (FVector::Dist(LastKnownPosition, GetActorLocation()) / LastDistFromStimuli);
}

// Summary: Returns the current dist from the player last location detected by the perception module.
//
void AEnemyCharacter::FindLastDistToStimuli()
{
	LastDistFromStimuli = FVector::Dist(LastKnownPosition, GetActorLocation());
}

// Summary: Basic function for calculating distance
//
float AEnemyCharacter::FindDistance(FVector InitialLocation, FVector EndLocation)
{
	return FVector::Dist(InitialLocation, EndLocation);
}

