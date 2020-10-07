// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityComponent.h"
#include "Kismet/KismetArrayLibrary.h"

void UAbilityComponent::OnGenerate(WeaponPickupRarity Rarity, URandArrayShuffler* Shuffler)
{
	//Will populate the RandBoolArray with a shuffled set of boolean values depending on the ability rarity.
	TArray<bool> RandBoolArray;
	if (Rarity == WeaponPickupRarity::LEGENDARY || Rarity == WeaponPickupRarity::CURSED)
	{
		Shuffler->GenerateRandBooleanArray(4, 4, RandBoolArray);
	}
	else if (Rarity == WeaponPickupRarity::TABOO)
	{
		Shuffler->GenerateRandBooleanArray(4, 3, RandBoolArray);
	}
	else if (Rarity == WeaponPickupRarity::MASTERFUL)
	{
		Shuffler->GenerateRandBooleanArray(4, FMath::RandRange(2, 3), RandBoolArray);
	}
	else //if Rarity is powerless, staunch or common
	{
		Shuffler->GenerateRandBooleanArray(4, 0, RandBoolArray);
	}

	//Temperature is kept separate from the good and bad characteristics
	Temperature = FMath::RandRange(-50, 150);
	
	//Assign the good or bad ability characteristics based on the result of the random boolean array.
	EffectivenessLevel = (RandBoolArray[0] ? FMath::RandRange(5, 10) : FMath::RandRange(1, 5));
	ManaPool = (RandBoolArray[1] ? FMath::RandRange(50, 100) : FMath::RandRange(10, 50));
	Persistence = (RandBoolArray[2] ? FMath::RandRange(2.0f, 5.0f) : FMath::RandRange(0.3, 2.0f));
	FireRate = (RandBoolArray[3] ? FMath::RandRange(0.01f, 0.25f) : FMath::RandRange(0.25f, 1.0f));

	//If the weapon is powerless, make it unable to use it's ability
	//A powerless weapon will have it's fire ability strengthened
	if(Rarity == WeaponPickupRarity::POWERLESS)
	{
		ManaPool = 0;
	}
}

