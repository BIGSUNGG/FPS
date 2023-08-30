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

public:
	void CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult);

protected:
	// Rpc
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult);

	// Delegate
	UFUNCTION()
		void OnPawnSetEvent(APlayerState* Player, APawn* NewPawn, APawn* OldPawn); // 새로운 Player 캐릭터가 생성되었을때 호출됨

public:
	FPlayerStateDeathDele OnCreatureDeath;

	FNewKraverPlayer OnNewPlayer;
	FNewKraverPlayer OnNewLocalPlayer;

protected:
	class AKraverPlayer* LocalPlayer = nullptr;
	class AKraverPlayerController* LocalController = nullptr;
	class AKraverHUD* HUD;
};
