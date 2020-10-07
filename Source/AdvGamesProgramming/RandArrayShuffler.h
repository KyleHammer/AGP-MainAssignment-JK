// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RandArrayShuffler.generated.h"

/**
 * Used to generate the random boolean array that's used in weapon pickup
 * Put into it's own class as it's also used by AbilityComponent
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVGAMESPROGRAMMING_API URandArrayShuffler : public UActorComponent
{
    GENERATED_BODY()
    
    public:
    void GenerateRandBooleanArray(int32 ArrayLength, int32 NumTrue, TArray<bool>& RandBoolArray);
};
