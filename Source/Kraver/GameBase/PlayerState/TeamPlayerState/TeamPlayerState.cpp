// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamPlayerState.h"
#include KraverPlayer_h

void ATeamPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamPlayerState, PlayerTeam);
}

void ATeamPlayerState::OnPawnSetEvent(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	Super::OnPawnSetEvent(Player, NewPawn, OldPawn);

	if (this != Player)
		return;

	SetPlayerTeam(PlayerTeam);
}

void ATeamPlayerState::SetPlayerTeam(ETeam InTeam)
{
	PlayerTeam = InTeam;
	OwnerPlayer = OwnerPlayer ? OwnerPlayer : Cast<AKraverPlayer>(GetOwner());
	if(OwnerPlayer)
		OwnerPlayer->CombatComponent->SetTeam(InTeam);
}
