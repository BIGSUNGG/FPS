// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/GameMode.h"
#include "KraverGameMode.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AKraverGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AKraverGameMode();
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 플레이어 스폰 위치 구하기
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;
	
	// Creature사망 시 호출
	virtual void CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult);

	// 플레이어 리스폰 요청
	// RespawnPlayer : 리스폰할 플레이어 캐릭터
	// PlayerController : 리스폰할 플레이어 컨트롤러
	virtual void RequestRespawn(AKraverPlayer* RespawnPlayer, AController* PlayerController);

	// 플레이어 스폰 시 기본적으로 들고있을 무기 요청
	virtual void RequestDefaultWeapon(class AKraverPlayerState* Player, const TArray<TSubclassOf<class AWeapon>>& RequestWeapons);
	// 플레이어 스폰 시 기본적으로 들고있을 무기 설정
	virtual void SpawnDefaultWeapon(class AKraverPlayerState* Player, const TArray<TSubclassOf<class AWeapon>>& RequestWeapons);

protected:
	// 게임 종료 이벤트
	// WinTeam : 게임에서 승리한 팀
	virtual void GameFinishEvent(ETeam WinTeam);

	UFUNCTION()
	virtual void OnGameFinishExitTimerEvent();

public:
	// Getter Setter
	static float GetRespawnTime() { return RespawnTime; }

protected:
	class AKraverGameState* KraverGameState = nullptr;
	
	// Respawn
	bool bRespawn = true; // 플레이어 사망 시 리스폰할 것인지
	inline static const float RespawnTime = 5.f; // 리스폰까지 걸리는 시간

	// Game Finish
	bool IsGameFinish = false; // 게임이 종료되었는지
	float GameFinishTimeDilation = 0.2f; // 게임 종료 시 설정할 게임 시간 배율
	FTimerHandle GameFinishExitTimer; // 세션 종료 타이머
	float GameFinishExitTime = 5.f; // 세션을 종료할 시간
};
