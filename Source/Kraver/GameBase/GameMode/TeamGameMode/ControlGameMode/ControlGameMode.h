// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include TeamGameMode_h
#include "ControlGameMode.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AControlGameMode : public ATeamGameMode
{
	GENERATED_BODY()
	
public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void DivideTeam(AController* InPlayer) override;
	virtual TTuple<bool, ETeam> CalculateControllingTeam(); // ���� �������� �� ���ϱ�

	virtual void PointDelayTimerEvent();

protected:
	TObjectPtr<class AControlGameState> ControlGameState;
	class AControlArea* ControlArea; // ���� ����

	// Point
	float PointDelay = 0.5f; // ���� �� ����Ʈ�� ��� �ӵ�
	FTimerHandle PointDelayTimer; // ���� ����Ʈ�� ��� Ÿ�̸�
};
