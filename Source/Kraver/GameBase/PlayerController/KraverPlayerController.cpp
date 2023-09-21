// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayerController.h"
#include ReturnMainMenuWidget_h

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
	if (ReturnMainMenuWidget == nullptr) 
		return;

	if (ReturnToMainMenu == nullptr)
		ReturnToMainMenu = CreateWidget<UReturnMainMenuWidget>(this, ReturnMainMenuWidget);

	if (ReturnToMainMenu)
	{
		bReturnToMainMenuOpen = !bReturnToMainMenuOpen;
		if (bReturnToMainMenuOpen)
		{
			ReturnToMainMenu->MenuSetup();
		}
		else
		{
			ReturnToMainMenu->MenuTearDown();
		}
	}
}
