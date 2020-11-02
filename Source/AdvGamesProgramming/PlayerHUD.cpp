// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "PlayerCharacter.h"

APlayerHUD::APlayerHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> PlayerHUDObject(TEXT("/Game/Widgets/PlayerHUDWidget"));

	PlayerHUDClass = PlayerHUDObject.Class;

	//Make sure the PlayerHUD class was found correctly
	if (PlayerHUDClass)
	{
		//Need to check that the widget was created successfully
		CurrentPlayerHUDWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);
		if (CurrentPlayerHUDWidget)
		{
			//Draw the hud to the player controllers viewport
			CurrentPlayerHUDWidget->AddToViewport();
			//Find the health bar and the ammo text block
			HealthProgressBar = Cast<UProgressBar>(CurrentPlayerHUDWidget->GetWidgetFromName(TEXT("ProgHealthBar")));
			AmmoTextBlock = Cast<UTextBlock>(CurrentPlayerHUDWidget->GetWidgetFromName(TEXT("TextAmmo")));
			ManaTextBlock = Cast<UTextBlock>(CurrentPlayerHUDWidget->GetWidgetFromName(TEXT("TextMana")));
			CrosshairImageBlock = Cast<UImage>(CurrentPlayerHUDWidget->GetWidgetFromName(TEXT("ImgCrosshair")));
			
			RarityTextBlock = Cast<UTextBlock>(CurrentPlayerHUDWidget->GetWidgetFromName(TEXT("TextRarity")));
			BulletDamageTextBlock = Cast<UTextBlock>(CurrentPlayerHUDWidget->GetWidgetFromName(TEXT("TextBulletDamage")));
			WeaponAccuracyTextBlock = Cast<UTextBlock>(CurrentPlayerHUDWidget->GetWidgetFromName(TEXT("TextWeaponAccuracy")));
	
			CurrentAbilityTypeTextBlock = Cast<UTextBlock>(CurrentPlayerHUDWidget->GetWidgetFromName(TEXT("TextCurrentAbilityType")));
			AbilityLevelTextBlock = Cast<UTextBlock>(CurrentPlayerHUDWidget->GetWidgetFromName(TEXT("TextAbilityLevel")));
			AbilityPersistenceTextBlock = Cast<UTextBlock>(CurrentPlayerHUDWidget->GetWidgetFromName(TEXT("TextAbilityPersistence")));
		}
	}
}

void APlayerHUD::SetPlayerHealthBarPercent(float Percent)
{
	if (HealthProgressBar) {
		HealthProgressBar->SetPercent(Percent);
	}
}

void APlayerHUD::SetAmmoText(int32 RoundsRemaining, int32 MagazineSize)
{
	if (AmmoTextBlock)
	{
		AmmoTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%i/%i"), RoundsRemaining, MagazineSize)));
	}
}

void APlayerHUD::SetManaText(float ManaRemaining, float ManaPoolSize)
{
	if (ManaTextBlock)
	{
		ManaTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%i/%i"), (int)ManaRemaining, (int)ManaPoolSize)));
	}
}

void APlayerHUD::SetHideWidgets(bool bIsHidden)
{
	if (bIsHidden)
	{
		if (HealthProgressBar) 
			HealthProgressBar->SetVisibility(ESlateVisibility::Hidden);
		if (AmmoTextBlock)
			AmmoTextBlock->SetVisibility(ESlateVisibility::Hidden);
		if (ManaTextBlock)
			ManaTextBlock->SetVisibility(ESlateVisibility::Hidden);
		if (CrosshairImageBlock)
			CrosshairImageBlock->SetVisibility(ESlateVisibility::Hidden);
		
		if (RarityTextBlock)
			RarityTextBlock->SetVisibility(ESlateVisibility::Hidden);
		if (BulletDamageTextBlock)
			BulletDamageTextBlock->SetVisibility(ESlateVisibility::Hidden);
		if (WeaponAccuracyTextBlock)
			WeaponAccuracyTextBlock->SetVisibility(ESlateVisibility::Hidden);
		
		if (CurrentAbilityTypeTextBlock)
			CurrentAbilityTypeTextBlock->SetVisibility(ESlateVisibility::Hidden);
		if (AbilityLevelTextBlock)
			AbilityLevelTextBlock->SetVisibility(ESlateVisibility::Hidden);
		if (AbilityPersistenceTextBlock)
			AbilityPersistenceTextBlock->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		if (HealthProgressBar)
		{
			HealthProgressBar->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Something went wrong with the hud."));
		}
			
		if (AmmoTextBlock)
			AmmoTextBlock->SetVisibility(ESlateVisibility::Visible);
		if (ManaTextBlock)
			ManaTextBlock->SetVisibility(ESlateVisibility::Visible);
		if (CrosshairImageBlock)
			CrosshairImageBlock->SetVisibility(ESlateVisibility::Visible);

		if (RarityTextBlock)
			RarityTextBlock->SetVisibility(ESlateVisibility::Visible);
		if (BulletDamageTextBlock)
			BulletDamageTextBlock->SetVisibility(ESlateVisibility::Visible);
		if (WeaponAccuracyTextBlock)
			WeaponAccuracyTextBlock->SetVisibility(ESlateVisibility::Visible);
		
		if (CurrentAbilityTypeTextBlock)
			CurrentAbilityTypeTextBlock->SetVisibility(ESlateVisibility::Visible);
		if (AbilityLevelTextBlock)
			AbilityLevelTextBlock->SetVisibility(ESlateVisibility::Visible);
		if (AbilityPersistenceTextBlock)
			AbilityPersistenceTextBlock->SetVisibility(ESlateVisibility::Visible);
		
		//Hack way. Having Gun C++ class instead of blueprints could fix this issue
		SetAmmoText(15, 15);
		SetManaText(20.0f, 20.0f);
		SetPlayerHealthBarPercent(1.0f);
	}
}


