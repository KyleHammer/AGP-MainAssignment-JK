// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponPickup.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Engine/Engine.h"

/**
* Generates the rarity values of the weapons using a series of RandRange() statements.
* Stats to the weapon are then allocated based on the rarity.
*/
void AWeaponPickup::OnGenerate()
{
	APickup::OnGenerate();

	//Find components for the abilities and shuffler algorithm
	AbilityComponent = FindComponentByClass<UAbilityComponent>();
	Shuffler = FindComponentByClass<URandArrayShuffler>();

	//Value that is used to determine the rarity value
	int32 RandomRarityValue = FMath::RandRange(1, 100);
	//Will populate the RandBoolArray with a shuffled set of boolean values depending on the weapon rarity.
	TArray<bool> RandBoolArray;

	//Pick a weapon rarity tier:
	if (RandomRarityValue <= 4)
	{
		Rarity = WeaponPickupRarity::LEGENDARY;
		RandBoolArray = Shuffler->GenerateRandBooleanArray(5, 5);
	}
	else if (RandomRarityValue <= 14)
	{
		Rarity = WeaponPickupRarity::MASTERFUL;
		RandBoolArray = Shuffler->GenerateRandBooleanArray(5, FMath::RandRange(3, 4));
	}
	else if (RandomRarityValue <= 29)
	{
		Rarity = WeaponPickupRarity::STAUNCH;
		RandBoolArray = Shuffler->GenerateRandBooleanArray(5, 4);
	}
	else if (RandomRarityValue <= 44)
	{
		Rarity = WeaponPickupRarity::TABOO;
		RandBoolArray = Shuffler->GenerateRandBooleanArray(5, 0);
	}
	else if (RandomRarityValue <= 52)
	{
		Rarity = WeaponPickupRarity::CURSED;
		RandBoolArray = Shuffler->GenerateRandBooleanArray(5, 0);
	}
	else if(RandomRarityValue <= 60)
	{
		Rarity = WeaponPickupRarity::POWERLESS;
		RandBoolArray = Shuffler->GenerateRandBooleanArray(5, 5);
	}
	else
	{
		Rarity = WeaponPickupRarity::COMMON;
		RandBoolArray = Shuffler->GenerateRandBooleanArray(5, 0);
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

/**
* Prints out the weapon stats to the screen if called by WeaponPickup, then calls PrintAbilityStats to print
* ability stats too. Does not print if bPrintWeaponStats is false
*/
void AWeaponPickup::PrintWeaponStats()
{
	if(GEngine && bPrintWeaponStats)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Bullet Damage: %f"), BulletDamage));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Muzzle Velocity: %f"), MuzzleVelocity));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Magazine Size: %d"), MagazineSize));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Weapon Accuracy: %f"), WeaponAccuracy));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Fire Rate: %f"), FireRate));
		AbilityComponent->PrintAbilityStats();
	}
	
}