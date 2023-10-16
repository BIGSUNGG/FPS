// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Blueprint/UserWidget.h"
#include "ReturnMainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UReturnMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable)
	void MenuSetup();
	UFUNCTION(BlueprintCallable)
	void MenuTearDown();

protected:
	// Delegate
	UFUNCTION()
	void OnReturnButtonClickEvent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* ReturnButton;

	UPROPERTY()
	class APlayerController* PlayerController;
};
