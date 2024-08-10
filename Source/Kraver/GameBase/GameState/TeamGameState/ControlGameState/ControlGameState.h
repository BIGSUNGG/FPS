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
	// Replicate�Ǵ� ���� �����ϱ� ���� ȣ��Ǵ� ƽ
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
	TArray<APlayerState*> BlueTeamArray; // ��� �� ���
	UPROPERTY(Replicated)
	TArray<APlayerState*> RedTeamArray; // ���� �� ���

	// Control
	UPROPERTY(ReplicatedUsing = OnRep_TryControlTeam)
	ETeam TryControlTeam = ETeam::NONE; // ���� �������� ��
	UFUNCTION()
	virtual void OnRep_TryControlTeam(ETeam Prev);

	UPROPERTY(ReplicatedUsing = OnRep_CurControlTeam)
	ETeam CurControlTeam = ETeam::NONE; // ���� �������� ��
	UFUNCTION()
	virtual void OnRep_CurControlTeam(ETeam Prev);

	UPROPERTY(ReplicatedUsing = OnRep_CurControlPoint)
	float CurControlPoint = 0.f; // ���� �󸶳� ������ �ߴ���
	UFUNCTION()
	virtual void OnRep_CurControlPoint(float Prev);
	
	// Interp
	ETeam TargetCurControlTeam = ETeam::NONE;
	float TargetControlPoint = 0.f; // Ŭ���̾�Ʈ���� CurControlPoint�� �ε巴�� �����ϱ����� ���Ǵ� ����

	float ControlIncreaseSpeed = 0.5f; // ���� �ӵ�
	float ControlDecreaseSpeed = 0.5f; // ���� ���� �ӵ�

	// Point
	UPROPERTY(Replicated)
	int BlueTeamPoint = 0; // ����� ����
	UPROPERTY(Replicated)
	int RedTeamPoint = 0; // ������ ����

};
