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
	void OnPawnSetEvent(APlayerState* Player, APawn* NewPawn, APawn* OldPawn); // ���ο� Player ĳ���Ͱ� �����Ǿ����� ȣ���

public:
	FNewKraverPlayer OnNewPlayer;
	FNewKraverPlayer OnNewLocalPlayer;

protected:
	class AKraverPlayer* LocalPlayer = nullptr;
	class AKraverPlayerController* LocalController = nullptr;
	class AKraverHUD* HUD;
};
