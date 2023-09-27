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
	// Rpc
	UFUNCTION(Server, Reliable)
	virtual void Server_RequestDefaultWeapon(const TArray<TSubclassOf<class AWeapon>>& RequestWeapons);

	// Delegate
	UFUNCTION()
	virtual void OnPawnSetEvent(APlayerState* Player, APawn* NewPawn, APawn* OldPawn) final; // ���ο� Player ĳ���Ͱ� �����Ǿ����� ȣ���

	// Func
	virtual void OnNewPawn(APawn* NewPawn);
	virtual void RequestDefaultWeapon(const TArray<TSubclassOf<class AWeapon>>& RequestWeapons);

public:
	class AKraverPlayer* GetOwnerPLayer() { return OwnerPlayer; }

public:
	FNewKraverPlayer OnNewPlayer;

protected:
	class AKraverPlayer* OwnerPlayer = nullptr;
	class AKraverPlayerController* OwnerController = nullptr;

	// Default Weapon
	bool bSpawnDefaultWeapon = false;

};
