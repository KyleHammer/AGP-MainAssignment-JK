// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponPickup.h"
#include "RandArrayShuffler.h"
#include "Components/ActorComponent.h"
#include "AbilityComponent.generated.h"

//A randomly generated value known as temperature determines the ability type
UENUM(BlueprintType)
enum class AbilityType : uint8
{
	//ICE: Creates an ice wall
	ICE,
	//WATER: Creates a water spout, slowing those who enter
    WATER,
	//STEAM: Creates a healing stream of steam
    STEAM,
	//FIRE: Creates a damaging field of fire
    FIRE 
};

/**
 * Similar to how weapon pickup works, generate a set of ability characteristics based on random values.
 * These random values are affected by the type of rarity of the weapon. See WeaponPickupRarity in
 * WeaponPickup for more details on how these stats are distributed.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVGAMESPROGRAMMING_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()
	public:

	void PrintAbilityStats();
	UFUNCTION(BlueprintCallable)
    void OnGenerate(WeaponPickupRarity Rarity, URandArrayShuffler* Shuffler);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Temperature;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AbilityType Ability;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 AbilityLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ManaPool;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AbilityPersistence;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AbilityFireRate;
};
