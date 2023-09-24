// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayerController.h"
#include PauseMenuWidget_h

void AKraverPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

}

void AKraverPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent == nullptr) return;

	InputComponent->BindAction("Menu", IE_Pressed, this, &AKraverPlayerController::ShowReturnToMainMenu);

}

void AKraverPlayerController::ShowReturnToMainMenu()
{
	if (PauseMenuWidgetClass == nullptr) 
		return;

	if (PauseMenuWidget == nullptr)
		PauseMenuWidget = CreateWidget<UPauseMenuWidget>(this, PauseMenuWidgetClass);

	if (PauseMenuWidget)
	{
		bPauseMenuOpen = !bPauseMenuOpen;
		if (bPauseMenuOpen)
		{
			PauseMenuWidget->MenuSetup();
		}
		else
		{
			PauseMenuWidget->MenuTearDown();
		}
	}
}
