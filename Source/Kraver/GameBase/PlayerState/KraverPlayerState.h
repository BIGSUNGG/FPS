// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/PlayerState.h"
#include "KraverPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AKraverPlayerState : public APlayerState
{
	GENERATED_BODY()
	AKraverPlayerState();

protected:
	UFUNCTION()
		void OnPawnSetEvent(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

protected:
	class AKraverPlayer* KraverPlayer = nullptr;
};
