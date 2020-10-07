// Fill out your copyright notice in the Description page of Project Settings.


#include "RandArrayShuffler.h"

void URandArrayShuffler::GenerateRandBooleanArray(int32 ArrayLength, int32 NumTrue, TArray<bool>& RandBoolArray)
{
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
}
