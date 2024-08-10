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
	virtual TTuple<bool, ETeam> CalculateControllingTeam(); // 현재 점령중인 팀 구하기

	virtual void PointDelayTimerEvent();

protected:
	TObjectPtr<class AControlGameState> ControlGameState;
	class AControlArea* ControlArea; // 점령 구역

	// Point
	float PointDelay = 0.5f; // 점령 중 포인트를 얻는 속도
	FTimerHandle PointDelayTimer; // 점령 포인트를 얻는 타이머
};
