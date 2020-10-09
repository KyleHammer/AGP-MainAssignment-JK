// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "AbilityComponent.h"
#include "RandArrayShuffler.h"
#include "WeaponPickup.generated.h"

//Rarity is used to determine how many good and bad stats a weapon should have
UENUM(BlueprintType)
enum class WeaponPickupRarity : uint8
{
    //LEGENDARY = 4% chance (Max Weapon Stats, Max Ability Stats)
    LEGENDARY,
    //MASTERFUL = 10% chance (High Weapon Stats, High Ability Stats)
    MASTERFUL,
    //STAUNCH = 15% chance (High Weapon Stats, Low Ability Stats)
    STAUNCH,
    //TABOO = 15% chance (Low Weapon Stats, High Ability Stats)
    TABOO,
    //COMMON = 40% chance (Low Weapon Stats, Low Ability Stats)
    COMMON,
    //CURSED = 8% chance (Disabled Weapon Stats, Max Ability Stats)
    CURSED,
    //POWERLESS = 8% chance (Max Weapon Stats, Disabled Ability Stats)
    POWERLESS
};

/**
* Overrides the OnPickup event to give the actor picking up the weapon new weapon stats.
* The new weapon stats are generated in the OnGenerate function
*/
UCLASS()
class ADVGAMESPROGRAMMING_API AWeaponPickup : public APickup
{
    GENERATED_BODY()
	
    public:

    UFUNCTION(BlueprintCallable)
    void PrintWeaponStats();
    UFUNCTION(BlueprintImplementableEvent)
    void OnPickup(AActor* ActorThatPickedUp) override;
    UFUNCTION(BlueprintCallable)
    void OnGenerate() override;

    UPROPERTY(EditAnywhere)
    bool bPrintWeaponStats;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    WeaponPickupRarity Rarity;

    class UAbilityComponent* AbilityComponent;
    URandArrayShuffler* Shuffler;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float BulletDamage;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float MuzzleVelocity;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 MagazineSize;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float WeaponAccuracy;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float FireRate;
};
