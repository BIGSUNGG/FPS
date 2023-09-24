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
	void MenuSetup();
	void MenuTearDown();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UReturnMainMenuWidget* ReturnMainMenu;

};
