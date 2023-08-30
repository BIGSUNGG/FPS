// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverGameState.h"

void AKraverGameState::CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult)
{
	Multicast_CreatureDeath(DeadCreature, VictimController, AttackerActor, AttackerController, DamageResult);
}

void AKraverGameState::Multicast_CreatureDeath_Implementation(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult)
{
	OnCreatureDeath.Broadcast(DeadCreature, VictimController, AttackerActor, AttackerController, DamageResult);
}
