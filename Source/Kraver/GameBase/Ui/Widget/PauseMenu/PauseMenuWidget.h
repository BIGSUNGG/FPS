// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void MenuSetup();
	void MenuTearDown();

protected:
	// Delegate
	UFUNCTION()
	virtual void OnSettingMenuButtonClickEvent();

protected:
	// Widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UReturnMainMenuWidget* ReturnMainMenu;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UWeaponSelectWidget* WeaponSelectMenu;

	// Setting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* SettingMenuButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class USettingMenuWidget* SettingMenu;

};
