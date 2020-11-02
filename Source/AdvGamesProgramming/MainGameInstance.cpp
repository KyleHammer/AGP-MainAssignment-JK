// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

UMainGameInstance::UMainGameInstance(const FObjectInitializer& ObjectInitialize)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MainMenuWidgetObject(TEXT("/Game/Widgets/MainMenuWidget"));
	MainMenuWidgetClass = MainMenuWidgetObject.Class;
}

void UMainGameInstance::LoadMenu()
{
	if (MainMenuWidgetClass)
	{
		Menu = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass);

		if (Menu)
		{
			Menu->AddToViewport();
		}
	}

	//Set up the UI input mode for the main menu
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(Menu->TakeWidget());
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}
}

void UMainGameInstance::Init()
{
	Subsystem = IOnlineSubsystem::Get();
	
	if (Subsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Online Subsystem: %s"), *Subsystem->GetSubsystemName().ToString())
	
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMainGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMainGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMainGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMainGameInstance::OnJoinSessionComplete);
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find Online Subsystem"))
	}
	
	//CreateSession(TEXT("Test Session"));
}

void UMainGameInstance::CreateSession(FName SessionName)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Session"));
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bIsLANMatch = true;
		SessionSettings.NumPublicConnections = 3;
		SessionSettings.bShouldAdvertise = true;
	
		SessionInterface->CreateSession(0, SessionName, SessionSettings);
	}
}

void UMainGameInstance::DestroySession(FName SessionName)
{
	UE_LOG(LogTemp, Warning, TEXT("Destroying Session"));
	if (SessionInterface.IsValid())
	{
		SessionInterface->DestroySession(SessionName);
	}
}

void UMainGameInstance::FindSession()
{
	UE_LOG(LogTemp, Warning, TEXT("Finding Sessions"));
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		SessionSearch->bIsLanQuery = true;
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

bool UMainGameInstance::JoinSession(ULocalPlayer *LocalPlayer, const FOnlineSessionSearchResult &SearchResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Joining Session"));
	if (SessionInterface.IsValid() && SessionSearch.IsValid())
	{
		//If there is a session in the list to join.
		if (SessionSearch->SearchResults.Num() > 0)
		{
			SessionInterface->JoinSession(0, TEXT("Test Session"), SessionSearch->SearchResults[0]);
		}

		return true;
	}

	return false;
}

void UMainGameInstance::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
	if (bSuccess)
	{
		UWorld* World = GetWorld();
		
		UE_LOG(LogTemp, Warning, TEXT("Session Created Successfully: %s - Joining Session"), *SessionName.ToString())

		if (World)
		{
			FInputModeGameOnly InputState;
			GetFirstLocalPlayerController()->SetInputMode(InputState);
			World->ServerTravel(TEXT("/Game/Levels/MultiplayerMap?listen"));
		}
		//FindSession();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session was not created"))
		DestroySession(SessionName);
	}
}

void UMainGameInstance::OnDestroySessionComplete(FName SessionName, bool bSuccess)
{
	// if (bSuccess && SessionInterface.IsValid())
	// {
	// 	CreateSession(SessionName);
	// }
}

void UMainGameInstance::OnFindSessionComplete(bool bSuccess)
{
	if (bSuccess)
	{
		
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
		FOnlineSessionSearchResult ResultSearch;

		UE_LOG(LogTemp, Warning, TEXT("Found %i Sessions"), SearchResults.Num())
		for (const FOnlineSessionSearchResult& SearchResult : SearchResults)
		{
			ResultSearch = SearchResult;
			UE_LOG(LogTemp, Warning, TEXT("Found Session: %s"), *SearchResult.GetSessionIdStr())
		}
		
		APlayerController *PlayerController = GetFirstLocalPlayerController();

		JoinSession(PlayerController->GetLocalPlayer(), ResultSearch);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Sessions was not successful"))
	}
}

void UMainGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult)
{
	if (SessionInterface.IsValid())
	{
		FString Address;
		SessionInterface->GetResolvedConnectString(SessionName, Address);

		APlayerController* PlayerController = GetFirstLocalPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputState;
			GetFirstLocalPlayerController()->SetInputMode(InputState);
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}
