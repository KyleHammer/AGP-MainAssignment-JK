// Fill out your copyright notice in the Description page of Project Settings.

#include "AIVision.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

/////////////////////////////////////////////////////////////////////
// AI Vision

// Sets default values for this component's properties
UAIVision::UAIVision()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UAIVision::BeginPlay()
{
	Super::BeginPlay();
	//EnemyCharacterComponent = GetOwner()->FindComponentByClass<AEnemyCharacter>();
}


// Called every frame
void UAIVision::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (EnemyCharacterComponent == nullptr) return;

}

/////////////////////////////////////////////////////////////////////////////
// RELATED METHODS FOR SENSE ADJUSTMENT

void UAIVision::DetermineCuriosity()
{
	if (EnemyCharacterComponent->bCanSeeActor)
	{
		CalculateCuriosity();

		EnemyCharacterComponent->IsCurious = EnemyCharacterComponent->TotalCuriosity > EnemyCharacterComponent->CuriousityThreshold ? true : false;

		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT(">> EnemyCharacter: Total Curiosity at = %f"), EnemyCharacterComponent->TotalCuriosity));
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::White, FString::Printf(TEXT(">> EnemyCharacter: Is Curious = %s"), EnemyCharacterComponent->IsCurious ? TEXT("True") : TEXT("False")));
	}
	else
	{
		EnemyCharacterComponent->TotalCuriosity -= 1;
		EnemyCharacterComponent->IsCurious = false;
	}
}

void UAIVision::CalculateCuriosity()
{
	FVector StimuliDiretion = EnemyCharacterComponent->DetectedActor->GetActorLocation() - GetOwner()->GetActorLocation();
	float DotResult = FVector::DotProduct(GetOwner()->GetActorForwardVector().GetSafeNormal(1), StimuliDiretion.GetSafeNormal(1));

	//Check  within  peripheral view
	if (DotResult >= 0.7)
	{
		EnemyCharacterComponent->TotalCuriosity += 0.5 * EnemyCharacterComponent->CuriositySensitivity;
	}
	else
	{
		//Check within focused distance
		float Distance = FVector::Dist(EnemyCharacterComponent->DetectedActor->GetActorLocation(), GetOwner()->GetActorLocation());

		if (Distance < EnemyCharacterComponent->AgentCritialAwarenessDistance)
		{
			EnemyCharacterComponent->TotalCuriosity += 0.4 * EnemyCharacterComponent->CuriositySensitivity;
			return;
		}

		EnemyCharacterComponent->TotalCuriosity += 0.05 * EnemyCharacterComponent->CuriositySensitivity;
	}

	return;
}

void UAIVision::DetermineThreat()
{
	if (EnemyCharacterComponent->bCanSeeActor)
	{
		CalculateThreat();

		EnemyCharacterComponent->IsThreatened = EnemyCharacterComponent->TotalThreat > EnemyCharacterComponent->ThreatThreshold ? true : false;

		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT(">> EnemyCharacter: Total Threat at = %f"), EnemyCharacterComponent->TotalThreat));
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, FString::Printf(TEXT(">> EnemyCharacter: Is Threat = %s"), EnemyCharacterComponent->IsThreatened ? TEXT("True") : TEXT("False")));
	}
	else
	{
		EnemyCharacterComponent->TotalThreat -= 1;
		EnemyCharacterComponent->IsThreatened = false;
	}
}

void UAIVision::CalculateThreat()
{
	FVector StimuliDiretion = EnemyCharacterComponent->DetectedActor->GetActorLocation() - GetOwner()->GetActorLocation();
	float DotResult = FVector::DotProduct(GetOwner()->GetActorForwardVector().GetSafeNormal(1), StimuliDiretion.GetSafeNormal(1));

	if (DotResult >= 0.8)
	{
		//Check within focused distance
		float Distance = FVector::Dist(EnemyCharacterComponent->DetectedActor->GetActorLocation(), GetOwner()->GetActorLocation());

		if (Distance < EnemyCharacterComponent->AgentCritialAwarenessDistance)
		{
			EnemyCharacterComponent->TotalCuriosity -= 1;
			EnemyCharacterComponent->TotalThreat += 0.8;
			return;
		}

		EnemyCharacterComponent->TotalThreat += 0.2;
	}
}

void UAIVision::GroundValues()
{
	if (EnemyCharacterComponent->TotalCuriosity < 0)
	{
		EnemyCharacterComponent->TotalCuriosity = 0;
	}
	else if (EnemyCharacterComponent->TotalCuriosity > 100)
	{
		EnemyCharacterComponent->TotalCuriosity = 100;
	}

	if (EnemyCharacterComponent->TotalThreat < 0)
	{
		EnemyCharacterComponent->TotalThreat = 0;
	}
	else if (EnemyCharacterComponent->TotalThreat > 100)
	{
		EnemyCharacterComponent->TotalThreat = 100;
	}
}

// Summary: Finds the ratio from character's initial distance to its current to the last stimuli source
//
float UAIVision::CalculateDistRatioToLastStimuli()
{
	return (FVector::Dist(EnemyCharacterComponent->LastKnownPosition, GetOwner()->GetActorLocation()) / EnemyCharacterComponent->LastDistFromStimuli);
}

// Summary: Returns the current dist from the player last location detected by the perception module.
//
void UAIVision::FindLastDistToStimuli()
{
	EnemyCharacterComponent->LastDistFromStimuli = FVector::Dist(EnemyCharacterComponent->LastKnownPosition, GetOwner()->GetActorLocation());
}

// Summary: Basic function for calculating distance
//
float UAIVision::FindDistance(FVector InitialLocation, FVector EndLocation)
{
	return FVector::Dist(InitialLocation, EndLocation);
}
