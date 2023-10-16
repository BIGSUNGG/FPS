// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayerController.h"
#include "MultiplayerSessionsSubsystem.h"
#include PauseMenuWidget_h

void AKraverPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

}

void AKraverPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent == nullptr) return;

	InputComponent->BindAction("Menu", IE_Pressed, this, &AKraverPlayerController::ShowPauseMenu);

}

void AKraverPlayerController::ReturnToMainMenu()
{
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>();
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &AKraverPlayerController::OnDestroySessionEvent_ReturnToMainMenu);
		MultiplayerSessionsSubsystem->DestroySession();
	}
}

void AKraverPlayerController::ExitGame()
{
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>();
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &AKraverPlayerController::OnDestroySessionEvent_ExitGame);
		MultiplayerSessionsSubsystem->DestroySession();
	}
}

void AKraverPlayerController::OnDestroySessionEvent_ReturnToMainMenu(bool bWasSuccessful)
{
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>();
	if (!MultiplayerSessionsSubsystem) return;

	MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &AKraverPlayerController::OnDestroySessionEvent_ReturnToMainMenu);

	UWorld* World = GetWorld();
	if (World)
	{
		AGameModeBase* GameMode = World->GetAuthGameMode<AGameModeBase>();
		if (GameMode)
		{
			GameMode->ReturnToMainMenuHost();
		}
		else
		{
			ClientReturnToMainMenuWithTextReason(FText());
		}
	}

}

void AKraverPlayerController::OnDestroySessionEvent_ExitGame(bool bWasSuccessful)
{
	KR_LOG(Log, TEXT("Request Exit Game"));
	ConsoleCommand("quit");

}

void AKraverPlayerController::ShowPauseMenu()
{
	if (PauseMenuWidgetClass == nullptr) 
		return;

	if (PauseMenuWidget == nullptr)
		PauseMenuWidget = CreateWidget<UPauseMenuWidget>(this, PauseMenuWidgetClass);

	if (PauseMenuWidget)
	{
		if (PauseMenuWidget->GetVisibility() != ESlateVisibility::Visible)
		{
			PauseMenuWidget->MenuSetup();
		}
		else
		{
			PauseMenuWidget->MenuTearDown();
		}
	}
}
