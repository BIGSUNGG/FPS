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

	TTuple<bool, ETeam> TryControlTeamTuple = CalculateControllingTeam(); // ���� ���� �õ����� ��

	if (TryControlTeamTuple.Key == false) // �ƹ��� ������ ���� �ʴٸ�
	{
		// ���� ����Ʈ ����
		ControlGameState->DecreaseControlPoint(DeltaSeconds);
		if (ControlGameState->CurControlPoint <= 0.f) // ���� ����Ʈ �ʱ�ȭ ���״ٸ�
			ControlGameState->TryControlTeam = ETeam::NONE;
	}
	else // ������ ������ �ִٸ�
	{
		if (TryControlTeamTuple.Value == ETeam::NONE) // ���� �õ����� �� ���� ���� ���ٸ�
		{
			// DO NOTHING
		}
		else if (TryControlTeamTuple.Value == ControlGameState->CurControlTeam) // �������� ���� �̹� ������ �����س��ٸ�
		{
			// ���� ����Ʈ ����
			ControlGameState->DecreaseControlPoint(DeltaSeconds);
			if (ControlGameState->CurControlPoint <= 0.f) // ���� ����Ʈ �ʱ�ȭ ���״ٸ�
				ControlGameState->TryControlTeam = ETeam::NONE;

		}
		else // ���� �õ����� ���� �ٸ� ���� ������ �������̶�� 
		{
			if (ControlGameState->TryControlTeam == TryControlTeamTuple.Value)
			{
				// ���� ����Ʈ ����
				ControlGameState->IncreaseControlPoint(DeltaSeconds);
				if (ControlGameState->CurControlPoint >= 1.f) // ���� ����Ʈ�� �� ä���ٸ�
				{
					// ������ ������ �� ����
					ControlGameState->CurControlTeam = TryControlTeamTuple.Value;
					ControlGameState->TryControlTeam = ETeam::NONE;
					ControlGameState->CurControlPoint = 0.f;

					GetWorldTimerManager().ClearTimer(PointDelayTimer);
					GetWorldTimerManager().SetTimer(PointDelayTimer, this, &ThisClass::PointDelayTimerEvent, PointDelay, true);
				}
			}
			else
			{
				// ���� ����Ʈ ����
				ControlGameState->DecreaseControlPoint(DeltaSeconds);
				if (ControlGameState->CurControlPoint <= 0.f) // ���� ����Ʈ �ʱ�ȭ ���״ٸ�
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

	// �� ���� ���ϱ�
	const TArray<APlayerState*>& BlueTeamArr = ControlGameState->GetTeamArray(ETeam::BLUE);
	const TArray<APlayerState*>& RedTeamArr = ControlGameState->GetTeamArray(ETeam::RED);

	// �ο��� �� ���� ���� ����
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

	// ���� �õ����� ���� �ִ���
	if (ControlArea->GetRedTeamCount() > 0 || ControlArea->GetBlueTeamCount() > 0)
		Result.Key = true;

	// ��� ���� ���� ������
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

	// �������� ���� ���� ����
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
