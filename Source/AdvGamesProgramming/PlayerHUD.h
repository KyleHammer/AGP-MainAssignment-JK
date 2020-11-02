// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

/**
* 
*/
UCLASS()
class ADVGAMESPROGRAMMING_API APlayerHUD : public AHUD
{
	GENERATED_BODY()
	
	public:
	APlayerHUD();

	/**
	* Changes the percent of the progress bar health component to reflect the given percentage
	* @param Percent: The percent that the health bar will be set to. 
	*/
	void SetPlayerHealthBarPercent(float Percent);

	/**
	* Will set the ammo text block to reflect the amound of ammo left in the magazine and the magazine size
	* @param RoundsRemaining: The number of rounds left in the magazine
	* @param MagazineSize: The number of rounds in a full magazine
	*/
	UFUNCTION(BlueprintCallable)
    void SetAmmoText(int32 RoundsRemaining, int32 MagazineSize);

	/**
	* Will set the mana text block to reflect the amount of mana left in the gun
	* @param ManaRemaining: The remaining mana in the gun
	* @param ManaPoolSize: The max capacity for the gun's mana
	*/
	UFUNCTION(BlueprintCallable)
    void SetManaText(float ManaRemaining, float ManaPoolSize);
	
	/**
	* Will set all of the remaining weapon stats for the HUD (Rarity, bullet damage, ability type, ability level and ability persistence)
	*/
	UFUNCTION(BlueprintCallable)
	void SetWeaponStatsText(float FireRate, float AbilityFireRate, float BulletDamage, float WeaponAccuracy, FString AbilityType, int32 AbilityLevel, float AbilityPersistence);

	/**
	* Hides or shows the widgets in the hud.
	* @param bIsHidden: Whether the widgets should be hidden (true) or shown (false)
	*/
	void SetHideWidgets(bool bIsHidden);

	private:

	TSubclassOf<class UUserWidget> PlayerHUDClass;
	UUserWidget* CurrentPlayerHUDWidget;
	class UProgressBar* HealthProgressBar;
	class UTextBlock* AmmoTextBlock;
	class UTextBlock* ManaTextBlock;
	class UImage* CrosshairImageBlock;
	
	class UTextBlock* FireRateTextBlock;
	class UTextBlock* BulletDamageTextBlock;
	class UTextBlock* WeaponAccuracyTextBlock;
	
	class UTextBlock* CurrentAbilityTypeTextBlock;
	class UTextBlock* AbilityLevelTextBlock;
	class UTextBlock* AbilityPersistenceTextBlock;
};
