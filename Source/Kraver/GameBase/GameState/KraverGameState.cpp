// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverGameState.h"

void AKraverGameState::CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, ACreature* AttackerCreature, AController* AttackerController, FKraverDamageResult const& DamageResult)
{
	Multicast_CreatureDeath(DeadCreature, VictimController, AttackerCreature, AttackerController, DamageResult);
}

void AKraverGameState::GameFinish(ETeam WinTeam)
{
	Multicast_GameFinish(WinTeam);
}

void AKraverGameState::Multicast_CreatureDeath_Implementation(class ACreature* VictimCreature, class AController* VictimController, ACreature* AttackerCreature, AController* AttackerController, FKraverDamageResult const& DamageResult)
{
	OnCreatureDeath.Broadcast(VictimCreature, VictimController, AttackerCreature, AttackerController, DamageResult);
}

void AKraverGameState::Multicast_GameFinish_Implementation(ETeam WinTeam)
{
	OnGameFinish.Broadcast(WinTeam);
}
