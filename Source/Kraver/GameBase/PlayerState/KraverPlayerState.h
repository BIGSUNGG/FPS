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

public:
	AKraverPlayerState();

protected:
	// Delegate
	UFUNCTION()
	void OnPawnSetEvent(APlayerState* Player, APawn* NewPawn, APawn* OldPawn); // 새로운 Player 캐릭터가 생성되었을때 호출됨

public:
	FNewKraverPlayer OnNewPlayer;
	FNewKraverPlayer OnNewLocalPlayer;

protected:
	class AKraverPlayer* LocalPlayer = nullptr;
	class AKraverPlayerController* LocalController = nullptr;
	class AKraverHUD* HUD;
};
