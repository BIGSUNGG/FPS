// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlGameState.h"
#include TeamPlayerState_h

void AControlGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AControlGameState::InterpTick(float DeltaSeconds)
{
	if (IS_SERVER())
		return;

	if (TargetControlPoint == CurControlPoint)
	{
		CurControlTeam = TargetCurControlTeam;
	}
	else if (TargetControlPoint > CurControlPoint)
	{
		IncreaseControlPoint(DeltaSeconds * 0.95f);
		if (CurControlPoint > TargetControlPoint)
			CurControlPoint = TargetControlPoint;
	}
	else
	{
		DecreaseControlPoint(DeltaSeconds * 0.95f);
		if (CurControlPoint < TargetControlPoint)
			CurControlPoint = TargetControlPoint;
	}
}

void AControlGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AControlGameState, RedTeamArray);
	DOREPLIFETIME(AControlGameState, BlueTeamArray);
	DOREPLIFETIME(AControlGameState, TryControlTeam);
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

void AControlGameState::AddTeamPlayer(class ATeamPlayerState* InPlayer)
{
	switch (InPlayer->GetPlayerTeam())
	{
	case ETeam::BLUE:
		BlueTeamArray.Emplace(InPlayer);
		break;
	case ETeam::RED:
		RedTeamArray.Emplace(InPlayer);
		break;
	}
}

void AControlGameState::RemoveTeamPlayer(class ATeamPlayerState* InPlayer)
{
	switch (InPlayer->GetPlayerTeam())
	{
	case ETeam::BLUE:
		BlueTeamArray.Remove(InPlayer);
		break;
	case ETeam::RED:
		RedTeamArray.Remove(InPlayer);
		break;
	}
}

void AControlGameState::IncreaseControlPoint(float DeletaTime)
{
	if (CurControlPoint < 1.0f)
	{
		CurControlPoint += ControlIncreaseSpeed * DeletaTime;
		if (CurControlPoint >= 1.0f)
			CurControlPoint = 1.0f;
	}
}

void AControlGameState::DecreaseControlPoint(float DeletaTime)
{
	if (CurControlPoint > 0.0f)
	{
		CurControlPoint -= ControlDecreaseSpeed * DeletaTime;
		if (CurControlPoint <= 0.0f)
			CurControlPoint = 0.0f;
	}
}

void AControlGameState::OnRep_TryControlTeam(ETeam Prev)
{

}

void AControlGameState::OnRep_CurControlTeam(ETeam Prev)
{
	TargetCurControlTeam = CurControlTeam;
	CurControlTeam = Prev;
}

void AControlGameState::OnRep_CurControlPoint(float Prev)
{
	TargetControlPoint	= CurControlPoint;

	if (FMath::Abs(CurControlPoint - Prev) <= 0.25f)
		CurControlPoint = Prev;
}
