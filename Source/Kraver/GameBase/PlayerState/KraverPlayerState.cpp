// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayerState.h"
#include "Kraver/Character/Creature/Soldier/KraverPlayer/KraverPlayer.h"
#include "Kraver/GameBase/SubSystem/DamageIndicatorSubsystem.h"

AKraverPlayerState::AKraverPlayerState()
{
	OnPawnSet.AddDynamic(this, &ThisClass::OnPawnSetEvent);
}

void AKraverPlayerState::OnPawnSetEvent(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	KraverPlayer = Cast<AKraverPlayer>(NewPawn);
	GetGameInstance()->GetSubsystem<UDamageIndicatorSubsystem>()->OnLocalPlayerBeginPlay(KraverPlayer);

}
