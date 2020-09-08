// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAgentBrain.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Math/UnrealMathUtility.h"

/////////////////////////////////////////////////////////////////////
// AI Agent Brain

// Sets default values for this component's properties
UAIAgentBrain::UAIAgentBrain()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UAIAgentBrain::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UAIAgentBrain::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}