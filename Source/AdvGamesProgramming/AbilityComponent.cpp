// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityComponent.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Engine/Engine.h"

/**
* Used to assign an ability level based on what's passed in from WeaponPickup
* @WeaponPickupRarity Rarity: An enum of type WeaponPickupRarity that determines the ability level
* @URandArrayShuffler* Shuffler: The shuffling script that determines which stats are good and bad
*/
void UAbilityComponent::OnGenerate(WeaponPickupRarity Rarity, URandArrayShuffler* Shuffler)
{
	//Will populate the RandBoolArray with a shuffled set of boolean values depending on the ability rarity.
	TArray<bool> RandBoolArray;
	if (Rarity == WeaponPickupRarity::LEGENDARY || Rarity == WeaponPickupRarity::CURSED)
	{
		RandBoolArray = Shuffler->GenerateRandBooleanArray(4, 4);
	}
	else if (Rarity == WeaponPickupRarity::TABOO)
	{
		RandBoolArray = Shuffler->GenerateRandBooleanArray(4, 3);
	}
	else if (Rarity == WeaponPickupRarity::MASTERFUL)
	{
		RandBoolArray = Shuffler->GenerateRandBooleanArray(4, FMath::RandRange(2, 3));
	}
	else //if Rarity is powerless, staunch or common
	{
		RandBoolArray = Shuffler->GenerateRandBooleanArray(4, 0);
	}

	//Temperature is kept separate from the good and bad characteristics
	Temperature = FMath::RandRange(-50, 150);
	
	//Assign the good or bad ability characteristics based on the result of the random boolean array.
	AbilityLevel = (RandBoolArray[0] ? FMath::RandRange(5, 10) : FMath::RandRange(1, 5));
	ManaPool = (float)(RandBoolArray[1] ? FMath::RandRange(50, 100) : FMath::RandRange(10, 50));
	AbilityPersistence = (RandBoolArray[2] ? FMath::RandRange(2.0f, 5.0f) : FMath::RandRange(0.3, 2.0f));
	AbilityFireRate = (RandBoolArray[3] ? FMath::RandRange(0.01f, 0.25f) : FMath::RandRange(0.25f, 1.0f));

	//If the weapon is powerless, make it unable to use it's ability
	//A powerless weapon will have it's fire ability strengthened
	if(Rarity == WeaponPickupRarity::POWERLESS)
	{
		ManaPool = 0;
	}
}

/**
* Prints out the ability stats to the screen if called by WeaponPickup
*/
void UAbilityComponent::PrintAbilityStats()
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString::Printf(TEXT("Temperature: %d"), Temperature));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString::Printf(TEXT("Effectiveness level %d"), AbilityLevel));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString::Printf(TEXT("Mana Pool: %f"), ManaPool));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString::Printf(TEXT("Persistence: %f"), AbilityPersistence));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString::Printf(TEXT("Fire Rate: %f"), AbilityFireRate)); 
	}
}

