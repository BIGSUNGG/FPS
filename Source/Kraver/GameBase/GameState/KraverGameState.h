// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/GameState.h"
#include "KraverGameState.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AKraverGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	// �÷��̾� ��� �� ȣ��
	void CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult);
	// ���� ���� �� ȣ��
	void GameFinish(ETeam WinTeam);

protected:
	// Rpc
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult);
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_GameFinish(ETeam WinTeam);

public:
	FPlayerStateDeathDele OnCreatureDeath; // �÷��̾� ��� �� ȣ��
	FGameFinish OnGameFinish; // ���� ���� �� ȣ��

};
