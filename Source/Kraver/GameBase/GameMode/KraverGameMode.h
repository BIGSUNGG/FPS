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

	// �÷��̾� ���� ��ġ ���ϱ�
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;
	
	// Creature��� �� ȣ��
	virtual void CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult);

	// �÷��̾� ������ ��û
	// RespawnPlayer : �������� �÷��̾� ĳ����
	// PlayerController : �������� �÷��̾� ��Ʈ�ѷ�
	virtual void RequestRespawn(AKraverPlayer* RespawnPlayer, AController* PlayerController);

	// �÷��̾� ���� �� �⺻������ ������� ���� ��û
	virtual void RequestDefaultWeapon(class AKraverPlayerState* Player, const TArray<TSubclassOf<class AWeapon>>& RequestWeapons);
	// �÷��̾� ���� �� �⺻������ ������� ���� ����
	virtual void SpawnDefaultWeapon(class AKraverPlayerState* Player, const TArray<TSubclassOf<class AWeapon>>& RequestWeapons);

protected:
	// ���� ���� �̺�Ʈ
	// WinTeam : ���ӿ��� �¸��� ��
	virtual void GameFinishEvent(ETeam WinTeam);

	UFUNCTION()
	virtual void OnGameFinishExitTimerEvent();

public:
	// Getter Setter
	static float GetRespawnTime() { return RespawnTime; }

protected:
	class AKraverGameState* KraverGameState = nullptr;
	
	// Respawn
	bool bRespawn = true; // �÷��̾� ��� �� �������� ������
	inline static const float RespawnTime = 5.f; // ���������� �ɸ��� �ð�

	// Game Finish
	bool IsGameFinish = false; // ������ ����Ǿ�����
	float GameFinishTimeDilation = 0.2f; // ���� ���� �� ������ ���� �ð� ����
	FTimerHandle GameFinishExitTimer; // ���� ���� Ÿ�̸�
	float GameFinishExitTime = 5.f; // ������ ������ �ð�
};
