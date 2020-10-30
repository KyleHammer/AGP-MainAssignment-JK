// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float NormalMovementSpeed;
	float SprintMovementSpeed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void Strafe(float Value);
	void LookUp(float Value);
	void Turn(float Value);

	void SprintStart();
	void SprintEnd();

	class UHealthComponent* HealthComponent;

	/**
	 * Will adjust the movement speed of the server character to sprinting
	 */
	UFUNCTION(Server, Reliable)
	void ServerSprintStart();

	/**
	 * Will adjust the movement speed of the server character to normal walking speed
	 */
	UFUNCTION(Server, Reliable)
	void ServerSprintEnd();

private:
	UPROPERTY(EditInstanceOnly, meta = (ClampMin="0.0", ClampMax="2.0", UIMin ="0.0", UIMax="2.0"))
	float LookSensitivity;

	UPROPERTY(EditInstanceOnly)
	float SprintMultiplier;

	UCameraComponent* Camera;
};
