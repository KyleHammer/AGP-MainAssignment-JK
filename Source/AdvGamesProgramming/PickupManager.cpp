// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupManager.h"
#include "Pickup.h"
#include "TimerManager.h"
#include "Engine/GameEngine.h"

// Sets default values
APickupManager::APickupManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APickupManager::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(WeaponSpawnTimer, this, &APickupManager::SpawnWeaponPickup, FrequencyOfWeaponPickupSpawns, true, 0.0f);
}

// Called every frame
void APickupManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickupManager::Init(const TArray<FVector>& SpawnLocations, TSubclassOf<APickup> WeaponPickup, float FrequencyOfSpawn)
{
	PossibleSpawnLocations = SpawnLocations;
	WeaponPickupClass = WeaponPickup;
	FrequencyOfWeaponPickupSpawns = FrequencyOfSpawn;
}

void APickupManager::SpawnWeaponPickup()
{
	//Find a random index in the array of spawn locations.
	int32 RandomIndex = FMath::RandRange(0, PossibleSpawnLocations.Num() - 1);

	//Attempt to spawn in the weapon pickup and write a warning to the log if it was unable to spawn it in.
	if (APickup* WeaponPickup = GetWorld()->SpawnActor<APickup>(WeaponPickupClass, PossibleSpawnLocations[RandomIndex] + FVector(0.0f, 0.0f, 50.0f), FRotator::ZeroRotator))
	{
		WeaponPickup->SetLifeSpan(20.0f);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("Pickup Spawned")));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to spawn weapon pickup."));
	}



}

