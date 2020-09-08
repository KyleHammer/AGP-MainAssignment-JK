// Fill out your copyright notice in the Description page of Project Settings.

#include "AIVision.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Math/UnrealMathUtility.h"

/////////////////////////////////////////////////////////////////////
// AI Vision

// Sets default values for this component's properties
UAIVision::UAIVision()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UAIVision::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UAIVision::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

