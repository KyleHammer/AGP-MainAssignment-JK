// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "EngineUtils.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentAgentState = AgentState::PATROL;
	PreviousAgentState = CurrentAgentState;

	PathfindingNodeAccuracy = 100.0f;
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

	PrintStateChange();

	PreviousAgentState = CurrentAgentState;

	if (CurrentAgentState == AgentState::PATROL)
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
	MoveAlongPath();
}

// Print out a screen message if the enemy state changes
void AEnemyCharacter::PrintStateChange()
{
	if(PreviousAgentState != CurrentAgentState)
	{
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
		default:
			StateToString = "UNKNOWN";
			break;
		}
		
		// Print new state to the screen
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("State switched to " + StateToString));
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

void AEnemyCharacter::SensePlayer(AActor* SensedActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		// Print message to the screen
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Player Detected"));
		
		DetectedActor = SensedActor;
		bCanSeeActor = true;
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

