// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenuWidget.h"
#include ReturnMainMenuWidget_h

void UPauseMenuWidget::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	ReturnMainMenu->MenuSetup();
}

void UPauseMenuWidget::MenuTearDown()
{
	RemoveFromParent();
	SetVisibility(ESlateVisibility::Hidden);

	ReturnMainMenu->MenuTearDown();
}
