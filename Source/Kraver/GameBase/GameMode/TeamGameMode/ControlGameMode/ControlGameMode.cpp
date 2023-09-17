// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlGameMode.h"
#include ControlGameState_h
#include ControlArea_h
#include TeamPlayerState_h
#include KraverPlayer_h
#include ControlArea_h

void AControlGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	ControlGameState = GetGameState<AControlGameState>();
}

void AControlGameMode::BeginPlay()
{
	Super::BeginPlay();

	DivideTeam(UGameplayStatics::GetPlayerController(this, 0));

	TArray<AActor*> ControlAreas;
	UGameplayStatics::GetAllActorsOfClass(this, AControlArea::StaticClass(), ControlAreas);
	if (ControlAreas.Num() != 1)
	{
		KR_LOG(Error, TEXT("ControlArea is not one"));
	}
	if (ControlAreas.Num() > 0)
	{
		ControlArea = static_cast<AControlArea*>(ControlAreas[0]);
	}
}

void AControlGameMode::Tick(float DeltaSeconds)
{
	ETeam ControllingTeam = CalculateControllingTeam();

	if(ControllingTeam != ETeam::NONE)
	{
		if (ControlGameState->CurControlTeam == ControllingTeam)
		{
			if (ControlGameState->CurControlPoint < 1.f)
			{
				ControlGameState->CurControlPoint += ControlIncreaseSpeed * DeltaSeconds;
				if (ControlGameState->CurControlPoint >= 1.f)
				{
					ControlGameState->CurControlPoint = 1.f;
					GetWorldTimerManager().SetTimer(PointDelayTimer, this, &ThisClass::PointDelayTimerEvent, PointDelay, true);
				}
			}
		}
		else
		{
			ControlGameState->CurControlPoint -= ControlDecreaseSpeed * DeltaSeconds;
			if (ControlGameState->CurControlPoint <= 0.f)
			{
				ControlGameState->CurControlPoint = -ControlGameState->CurControlPoint;
				ControlGameState->CurControlTeam = ControllingTeam;
				GetWorldTimerManager().ClearTimer(PointDelayTimer);
			}
		}
	}
	else if(ControlArea->GetRedTeamCount() == 0)
	{
		if (GetWorldTimerManager().IsTimerActive(PointDelayTimer))
		{
			ControlGameState->CurControlPoint += ControlIncreaseSpeed * DeltaSeconds;
			if (ControlGameState->CurControlPoint >= 1.f)
			{
				ControlGameState->CurControlPoint = 1.f;
			}
		}
		else
		{
			ControlGameState->CurControlPoint -= ControlDecreaseSpeed * DeltaSeconds;
			if (ControlGameState->CurControlPoint <= 0.f)
			{
				ControlGameState->CurControlPoint = 0.f;
			}
		}
	}
}

void AControlGameMode::DivideTeam(APlayerController* InPlayer)
{
	if (!InPlayer)
		return;

	Super::DivideTeam(InPlayer);

	ControlGameState = ControlGameState ? ControlGameState : GetGameState<AControlGameState>();

	if (!ControlGameState)
	{
		GetWorldTimerManager().SetTimerForNextTick([=]() { DivideTeam(InPlayer); });
		return;
	}

	const TArray<APlayerState*>& BlueTeamArr = ControlGameState->GetTeamArray(ETeam::BLUE);
	const TArray<APlayerState*>& RedTeamArr = ControlGameState->GetTeamArray(ETeam::RED);

	ETeam NewTeam;
	if (BlueTeamArr.Num() >= RedTeamArr.Num())
		NewTeam = ETeam::RED;
	else
		NewTeam = ETeam::BLUE;

	if (ATeamPlayerState* TeamPlayerState = InPlayer->GetPlayerState<ATeamPlayerState>())
	{
		TeamPlayerState->SetPlayerTeam(NewTeam);
		ControlGameState->AddTeamPlayer(NewTeam, InPlayer->GetPlayerState<APlayerState>());
	}
}

ETeam AControlGameMode::CalculateControllingTeam()
{
	if (ControlArea->GetRedTeamCount() == ControlArea->GetBlueTeamCount())
		return ETeam::NONE;

	if (ControlArea->GetRedTeamCount() > ControlArea->GetBlueTeamCount())
		return ETeam::RED;

	return ETeam::BLUE;
}

void AControlGameMode::PointDelayTimerEvent()
{
	switch (ControlGameState->CurControlTeam)
	{
	case ETeam::RED:
		++ControlGameState->RedTeamPoint;
		KR_LOG(Error, TEXT("R %d"), ControlGameState->RedTeamPoint);
		break;
	case ETeam::BLUE:
		++ControlGameState->BlueTeamPoint;
		KR_LOG(Error, TEXT("B %d"), ControlGameState->BlueTeamPoint);
		break;
	default:
		break;
	}
}
