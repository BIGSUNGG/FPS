// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/PlayerController.h"
#include "KraverPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AKraverPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void SetupInputComponent() override;

protected:
	void ShowReturnToMainMenu();

protected:
	// Return
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Widget", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UPauseMenuWidget> PauseMenuWidgetClass;
	UPauseMenuWidget* PauseMenuWidget;

};
