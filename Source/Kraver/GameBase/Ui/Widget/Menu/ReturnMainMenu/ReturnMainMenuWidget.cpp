// Fill out your copyright notice in the Description page of Project Settings.


#include "ReturnMainMenuWidget.h"
#include KraverPlayerController_h

void UReturnMainMenuWidget::MenuSetup()
{
	SetVisibility(ESlateVisibility::Visible);
	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameAndUI InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}
	if (ReturnButton)
	{
		ReturnButton->OnClicked.AddDynamic(this, &UReturnMainMenuWidget::OnReturnButtonClickEvent);
	}
}

void UReturnMainMenuWidget::MenuTearDown()
{
	SetVisibility(ESlateVisibility::Hidden);
	UWorld* World = GetWorld();
	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
	if (ReturnButton && ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.RemoveDynamic(this, &UReturnMainMenuWidget::OnReturnButtonClickEvent);
	}
}

bool UReturnMainMenuWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	return true;
}

void UReturnMainMenuWidget::OnReturnButtonClickEvent()
{
	ReturnButton->SetIsEnabled(false);

	AKraverPlayerController* PC = Cast<AKraverPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PC) return;

	PC->ReturnToMainMenu();

}