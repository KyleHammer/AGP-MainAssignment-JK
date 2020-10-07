// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponPickup.h"
#include "Kismet/KismetArrayLibrary.h"

void AWeaponPickup::OnGenerate()
{
	APickup::OnGenerate();

	//Find components for the abilities and shuffler algortihm
	AbilityComponent = FindComponentByClass<UAbilityComponent>();
	Shuffler = FindComponentByClass<URandArrayShuffler>();
	
	//Pick a weapon rarity tier:
	
	int32 RandomRarityValue = FMath::RandRange(1, 100);
	//Will populate the RandBoolArray with a shuffled set of boolean values depending on the weapon rarity.
	TArray<bool> RandBoolArray;
	if (RandomRarityValue <= 4)
	{
		Rarity = WeaponPickupRarity::LEGENDARY;
		Shuffler->GenerateRandBooleanArray(5, 5, RandBoolArray);
	}
	else if (RandomRarityValue <= 14)
	{
		Rarity = WeaponPickupRarity::MASTERFUL;
		Shuffler->GenerateRandBooleanArray(5, FMath::RandRange(3, 4), RandBoolArray);
	}
	else if (RandomRarityValue <= 29)
	{
		Rarity = WeaponPickupRarity::STAUNCH;
		Shuffler->GenerateRandBooleanArray(5, 4, RandBoolArray);
	}
	else if (RandomRarityValue <= 44)
	{
		Rarity = WeaponPickupRarity::TABOO;
		Shuffler->GenerateRandBooleanArray(5, 0, RandBoolArray);
	}
	else if (RandomRarityValue <= 52)
	{
		Rarity = WeaponPickupRarity::CURSED;
		Shuffler->GenerateRandBooleanArray(5, 0, RandBoolArray);
	}
	else if(RandomRarityValue <= 60)
	{
		Rarity = WeaponPickupRarity::POWERLESS;
		Shuffler->GenerateRandBooleanArray(5, 5, RandBoolArray);
	}
	
	//Assign the good or bad weapon characteristics based on the result of the random boolean array.
	BulletDamage = (RandBoolArray[0] ? FMath::RandRange(15.0f, 30.0f) : FMath::RandRange(2.0f, 15.0f));
	MuzzleVelocity = (RandBoolArray[1] ? FMath::RandRange(10000.0f, 20000.0f) : FMath::RandRange(5000.0f, 10000.0f));
	MagazineSize = (RandBoolArray[2] ? FMath::RandRange(20, 100) : FMath::RandRange(1, 20));
	WeaponAccuracy = (RandBoolArray[3] ? FMath::RandRange(100.0f, 1000.0f) : FMath::RandRange(20.0f, 80.0f));
	FireRate = (RandBoolArray[4] ? FMath::RandRange(0.01f, 0.25f) : FMath::RandRange(0.25f, 1.0f));

	//If the weapon is cursed, make it unable to fire
	//A cursed weapon will have it's ability strengthened
	if(Rarity == WeaponPickupRarity::CURSED)
	{
		MagazineSize = 0;
	}

	AbilityComponent->OnGenerate(Rarity, Shuffler);
}
