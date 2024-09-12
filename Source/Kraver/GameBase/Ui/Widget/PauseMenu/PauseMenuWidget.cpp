// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenuWidget.h"
#include ReturnMainMenuWidget_h
#include WeaponSelectWidget_h
#include SettingMenuWidget_h

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SettingMenuButton->OnClicked.AddDynamic(this, &ThisClass::OnSettingMenuButtonClickEvent);

}

void UPauseMenuWidget::MenuSetup_Implementation()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	ReturnMainMenu->SetVisibility(ESlateVisibility::Visible);
	SettingMenuButton->SetVisibility(ESlateVisibility::Visible);
	SettingMenu->SetVisibility(ESlateVisibility::Hidden);

	ReturnMainMenu->MenuSetup();
}

void UPauseMenuWidget::MenuTearDown_Implementation()
{
	SetVisibility(ESlateVisibility::Hidden);

	ReturnMainMenu->SetVisibility(ESlateVisibility::Hidden);
	SettingMenuButton->SetVisibility(ESlateVisibility::Hidden);
	SettingMenu->SetVisibility(ESlateVisibility::Hidden);

	SettingMenu->MenuTearDown();
	ReturnMainMenu->MenuTearDown();
}

void UPauseMenuWidget::OnSettingMenuButtonClickEvent_Implementation()
{
	ReturnMainMenu->SetVisibility(ESlateVisibility::Hidden);
	SettingMenuButton->SetVisibility(ESlateVisibility::Hidden);
	SettingMenu->SetVisibility(ESlateVisibility::Visible);

	SettingMenu->MenuSetup();

}
