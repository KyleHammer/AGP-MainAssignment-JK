// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MainGameInstance.generated.h"

/**
* 
*/
UCLASS()
class ADVGAMESPROGRAMMING_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	public:
	UMainGameInstance(const FObjectInitializer& ObjectInitialize);

	UFUNCTION(BlueprintCallable)
    void LoadMenu();

	virtual void Init();

	void CreateSession(FName SessionName);
	void DestroySession(FName SessionName);
	void FindSession();
	void JoinSession();

	private:
	TSubclassOf<class UUserWidget> MainMenuWidgetClass;
	class UMainMenuWidget* Menu;

	IOnlineSubsystem* Subsystem;
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	void OnCreateSessionComplete(FName SessionName, bool bSuccess);
	void OnDestroySessionComplete(FName SessionName, bool bSuccess);
	void OnFindSessionComplete(bool bSuccess);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinType);
};
