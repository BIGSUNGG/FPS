// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlGameState.h"

void AControlGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AControlGameState, RedTeamArray);
	DOREPLIFETIME(AControlGameState, BlueTeamArray);
	DOREPLIFETIME(AControlGameState, CurControlTeam);
	DOREPLIFETIME(AControlGameState, CurControlPoint);
	DOREPLIFETIME(AControlGameState, BlueTeamPoint);
	DOREPLIFETIME(AControlGameState, RedTeamPoint);
}

const TArray<APlayerState*>& AControlGameState::GetTeamArray(ETeam FindTeam)
{
	switch (FindTeam)
	{
	case ETeam::RED:
		return RedTeamArray;
		break;
	case ETeam::BLUE:
		return BlueTeamArray;
		break;
	}

	return Super::GetTeamArray(FindTeam);
}

void AControlGameState::AddTeamPlayer(ETeam InTeam, class APlayerState* InPlayer)
{
	switch (InTeam)
	{
	case ETeam::BLUE:
		BlueTeamArray.Emplace(InPlayer);
		break;
	case ETeam::RED:
		RedTeamArray.Emplace(InPlayer);
		break;
	}
}
