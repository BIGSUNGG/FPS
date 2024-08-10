// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include TeamGameState_h
#include "ControlGameState.generated.h"



/**
 * 
 */
UCLASS()
class KRAVER_API AControlGameState : public ATeamGameState
{
	GENERATED_BODY()
	friend class AControlGameMode;

public:
	virtual void Tick(float DeltaSeconds) override;
	// Replicate되는 값을 보간하기 위해 호출되는 틱
	virtual void InterpTick(float DeltaSeconds);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Getter Seeter
	virtual const TArray<APlayerState*>& GetTeamArray(ETeam FindTeam) override;
	ETeam GetTryControlTeam() { return TryControlTeam; }
	ETeam GetCurControlTeam() { return CurControlTeam; }
	float GetCurControlPoint() { return CurControlPoint; }
	int GetBlueTeamPoint() { return BlueTeamPoint; }
	int GetRedTeamPoint() { return RedTeamPoint; }

	virtual void AddTeamPlayer(class ATeamPlayerState* InPlayer) override;
	virtual void RemoveTeamPlayer(class ATeamPlayerState* InPlayer) override;

	virtual void IncreaseControlPoint(float DeletaTime);
	virtual void DecreaseControlPoint(float DeletaTime);

protected:
	UPROPERTY(Replicated)
	TArray<APlayerState*> BlueTeamArray; // 블루 팀 목록
	UPROPERTY(Replicated)
	TArray<APlayerState*> RedTeamArray; // 레드 팀 목록

	// Control
	UPROPERTY(ReplicatedUsing = OnRep_TryControlTeam)
	ETeam TryControlTeam = ETeam::NONE; // 전에 점령중인 팀
	UFUNCTION()
	virtual void OnRep_TryControlTeam(ETeam Prev);

	UPROPERTY(ReplicatedUsing = OnRep_CurControlTeam)
	ETeam CurControlTeam = ETeam::NONE; // 현재 점령중인 팀
	UFUNCTION()
	virtual void OnRep_CurControlTeam(ETeam Prev);

	UPROPERTY(ReplicatedUsing = OnRep_CurControlPoint)
	float CurControlPoint = 0.f; // 현재 얼마나 점령을 했는지
	UFUNCTION()
	virtual void OnRep_CurControlPoint(float Prev);
	
	// Interp
	ETeam TargetCurControlTeam = ETeam::NONE;
	float TargetControlPoint = 0.f; // 클라이언트에서 CurControlPoint를 부드럽게 보간하기위해 사용되는 변수

	float ControlIncreaseSpeed = 0.5f; // 점령 속도
	float ControlDecreaseSpeed = 0.5f; // 점령 감소 속도

	// Point
	UPROPERTY(Replicated)
	int BlueTeamPoint = 0; // 블루팀 점수
	UPROPERTY(Replicated)
	int RedTeamPoint = 0; // 레드팀 점수

};
