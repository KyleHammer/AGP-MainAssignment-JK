// Fill out your copyright notice in the Description page of Project Settings.


#include "RandArrayShuffler.h"

/**
* Used to assign an ability level based on what's passed in from WeaponPickup
* @WeaponPickupRarity Rarity: An enum of type WeaponPickupRarity that determines the ability level
* @URandArrayShuffler* Shuffler: The shuffling script that determines which stats are good and bad
* Return @TArray<Bool>: A boolean array that has NumTrue true instances in random places of the array
*/
TArray<bool> URandArrayShuffler::GenerateRandBooleanArray(int32 ArrayLength, int32 NumTrue)
{
    TArray<bool> RandBoolArray;
    
    for (int32 i = 0; i < ArrayLength; i++)
    {
        //Ternary Condition
        RandBoolArray.Add(i < NumTrue ? true : false);
    }

    //Card Shuffling Algorithm
    for (int32 i = 0; i < RandBoolArray.Num(); i++)
    {
        int32 RandIndex = FMath::RandRange(0, RandBoolArray.Num() - 1);
        bool Temp = RandBoolArray[i];
        RandBoolArray[i] = RandBoolArray[RandIndex];
        RandBoolArray[RandIndex] = Temp;
    }

    return RandBoolArray;
}
