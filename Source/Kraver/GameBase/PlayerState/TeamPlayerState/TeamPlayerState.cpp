// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamPlayerState.h"
#include KraverPlayer_h

void ATeamPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamPlayerState, PlayerTeam);
}

void ATeamPlayerState::OnNewPawn(APawn* NewPawn)
{
	Super::OnNewPawn(NewPawn);

	SetPlayerTeam(PlayerTeam);
}

void ATeamPlayerState::SetPlayerTeam(ETeam InTeam)
{
	PlayerTeam = InTeam;
	OwnerPlayer = OwnerPlayer ? OwnerPlayer : Cast<AKraverPlayer>(GetOwner());
	if(OwnerPlayer)
		OwnerPlayer->CombatComponent->SetTeam(InTeam);
}
