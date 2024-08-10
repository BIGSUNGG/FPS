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
	if (!ControlGameState)
		return;

	if (!ControlArea)
		return;

	TTuple<bool, ETeam> TryControlTeamTuple = CalculateControllingTeam(); // 현재 점령 시도중인 팀

	if (TryControlTeamTuple.Key == false) // 아무도 거점에 있지 않다면
	{
		// 점령 포인트 감소
		ControlGameState->DecreaseControlPoint(DeltaSeconds);
		if (ControlGameState->CurControlPoint <= 0.f) // 점령 포인트 초기화 시켰다면
			ControlGameState->TryControlTeam = ETeam::NONE;
	}
	else // 거점에 누군가 있다면
	{
		if (TryControlTeamTuple.Value == ETeam::NONE) // 점령 시도중인 두 팀의 수가 같다면
		{
			// DO NOTHING
		}
		else if (TryControlTeamTuple.Value == ControlGameState->CurControlTeam) // 점령중인 팀이 이미 거점을 점령해놨다면
		{
			// 점령 포인트 감소
			ControlGameState->DecreaseControlPoint(DeltaSeconds);
			if (ControlGameState->CurControlPoint <= 0.f) // 점령 포인트 초기화 시켰다면
				ControlGameState->TryControlTeam = ETeam::NONE;

		}
		else // 거점 시도중인 팀이 다른 팀의 거점을 점령중이라면 
		{
			if (ControlGameState->TryControlTeam == TryControlTeamTuple.Value)
			{
				// 점령 포인트 증가
				ControlGameState->IncreaseControlPoint(DeltaSeconds);
				if (ControlGameState->CurControlPoint >= 1.f) // 점령 포인트를 다 채웠다면
				{
					// 거점을 점령한 팀 변경
					ControlGameState->CurControlTeam = TryControlTeamTuple.Value;
					ControlGameState->TryControlTeam = ETeam::NONE;
					ControlGameState->CurControlPoint = 0.f;

					GetWorldTimerManager().ClearTimer(PointDelayTimer);
					GetWorldTimerManager().SetTimer(PointDelayTimer, this, &ThisClass::PointDelayTimerEvent, PointDelay, true);
				}
			}
			else
			{
				// 점령 포인트 감소
				ControlGameState->DecreaseControlPoint(DeltaSeconds);
				if (ControlGameState->CurControlPoint <= 0.f) // 점령 포인트 초기화 시켰다면
					ControlGameState->TryControlTeam = TryControlTeamTuple.Value;
			}
		}
	}
}

void AControlGameMode::DivideTeam(AController* InPlayer)
{
	if (!InPlayer)
		return;

	Super::DivideTeam(InPlayer);
	if (ATeamPlayerState* PlayerState = InPlayer->GetPlayerState<ATeamPlayerState>())
	{
		if(PlayerState->GetPlayerTeam() != ETeam::NONE)
			return;
	}

	ControlGameState = ControlGameState ? ControlGameState : GetGameState<AControlGameState>();

	// 팀 정보 구하기
	const TArray<APlayerState*>& BlueTeamArr = ControlGameState->GetTeamArray(ETeam::BLUE);
	const TArray<APlayerState*>& RedTeamArr = ControlGameState->GetTeamArray(ETeam::RED);

	// 인원이 더 적은 팀에 배정
	ETeam NewTeam;
	if (BlueTeamArr.Num() > RedTeamArr.Num())
	{
		NewTeam = ETeam::RED;
		KR_LOG(Log, TEXT("Add new red team"));
	}
	else
	{
		NewTeam = ETeam::BLUE;
		KR_LOG(Log, TEXT("Add new blue team"));
	}
	
	if (ATeamPlayerState* TeamPlayerState = InPlayer->GetPlayerState<ATeamPlayerState>())
	{
		TeamPlayerState->SetPlayerTeam(NewTeam);
		ControlGameState->AddTeamPlayer(InPlayer->GetPlayerState<ATeamPlayerState>());
	}
}

TTuple<bool, ETeam> AControlGameMode::CalculateControllingTeam()
{
	TTuple<bool, ETeam> Result{ false, ETeam::NONE };

	// 점령 시도중인 팀이 있는지
	if (ControlArea->GetRedTeamCount() > 0 || ControlArea->GetBlueTeamCount() > 0)
		Result.Key = true;

	// 어느 팀이 점령 중인지
	if(ControlArea->GetRedTeamCount() == ControlArea->GetBlueTeamCount())
		Result.Value = ETeam::NONE;
	else if (ControlArea->GetRedTeamCount() > ControlArea->GetBlueTeamCount())
		Result.Value = ETeam::RED;
	else
		Result.Value = ETeam::BLUE;

	return Result;
}

void AControlGameMode::PointDelayTimerEvent()
{
	if (IsGameFinish)
		return;

	// 점령중인 팀의 점수 증가
	switch (ControlGameState->CurControlTeam)
	{
	case ETeam::RED:
		++ControlGameState->RedTeamPoint;
		if (ControlGameState->RedTeamPoint >= 100)
			GameFinishEvent(ETeam::RED);
		break;
	case ETeam::BLUE:
		++ControlGameState->BlueTeamPoint;
		if (ControlGameState->BlueTeamPoint >= 100)
			GameFinishEvent(ETeam::BLUE);
		break;
	default:
		break;
	}

}
