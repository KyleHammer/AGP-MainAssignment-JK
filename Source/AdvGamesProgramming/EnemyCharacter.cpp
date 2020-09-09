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
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Handles related decision values
	DetermineCuriosity();
	DetermineThreat();
	GroundValues();

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
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Lost"))
		bCanSeeActor = false;
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

void AEnemyCharacter::CalculateCuriosity()
{
	FVector StimuliDiretion = DetectedActor->GetActorLocation() - GetActorLocation();
	float DotResult = FVector::DotProduct(GetActorForwardVector().GetSafeNormal(1), StimuliDiretion.GetSafeNormal(1));
	
	//Check  within  peripheral view
	if (DotResult >= 0.7)
	{
		TotalCuriosity += 0.5 * CuriositySensitivity;
	} else
	{ 
		//Check within focused distance
		float Distance = FVector::Dist(DetectedActor->GetActorLocation(), GetActorLocation());

		if (Distance < AgentCritialAwarenessDistance)
		{
			TotalCuriosity += 0.4 * CuriositySensitivity;
			return;
		}

		TotalCuriosity += 0.05 * CuriositySensitivity;
	}

	return;
}

void AEnemyCharacter::DetermineThreat()
{
	if (bCanSeeActor) {
		CalculateThreat();

		IsThreatened = TotalThreat > ThreatThreshold ? true : false;

		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT(">> EnemyCharacter: Total Threat at = %f"), TotalThreat));
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, FString::Printf(TEXT(">> EnemyCharacter: Is Threat = %s"), IsThreatened ? TEXT("True") : TEXT("False")));
	} 
	else 
	{
		TotalThreat -= 1;
		IsThreatened = false;
	}
}

void AEnemyCharacter::CalculateThreat()
{
	FVector StimuliDiretion = DetectedActor->GetActorLocation() - GetActorLocation();
	float DotResult = FVector::DotProduct(GetActorForwardVector().GetSafeNormal(1), StimuliDiretion.GetSafeNormal(1));

	if (DotResult >= 0.8)
	{
		//Check within focused distance
		float Distance = FVector::Dist(DetectedActor->GetActorLocation(), GetActorLocation());

		if (Distance < AgentCritialAwarenessDistance)
		{
			TotalCuriosity -= 1;
			TotalThreat += 0.8;
			return;
		}

		TotalThreat += 0.2;
	}
}

void AEnemyCharacter::GroundValues()
{
	if (TotalCuriosity < 0) 
	{
		TotalCuriosity = 0;
	} 
	else if (TotalCuriosity > 100) 
	{
		TotalCuriosity = 100;
	}
	
	if (TotalThreat < 0)
	{
		TotalThreat = 0;
	}
	else if (TotalThreat > 100)
	{
		TotalThreat = 100;
	}
}

// Summary: Basic function for calculating distance
//
float AEnemyCharacter::FindDistance(FVector InitialLocation, FVector EndLocation)
{
	return FVector::Dist(InitialLocation, EndLocation);
}

