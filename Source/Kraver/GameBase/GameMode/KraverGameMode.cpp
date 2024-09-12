// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverGameMode.h"
#include KraverPlayer_h
#include KraverSpectator_h
#include KraverGameState_h
#include KraverPlayerState_h
#include KraverPlayerController_h

AKraverGameMode::AKraverGameMode()
{
	GameStateClass = AKraverGameState::StaticClass();
	PlayerStateClass = AKraverPlayerState::StaticClass();
}

void AKraverGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	KraverGameState = Cast<AKraverGameState>(GameState);
}

void AKraverGameMode::BeginPlay()
{
	Super::BeginPlay();

	KraverGameState = Cast<AKraverGameState>(GameState);
}

void AKraverGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
}

AActor* AKraverGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
	return PlayerStarts[Selection];
}

void AKraverGameMode::CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, ACreature* AttackerCreature, AController* AttackerController, FKraverDamageResult const& DamageResult)
{
	KraverGameState = KraverGameState ? KraverGameState : Cast<AKraverGameState>(GameState);
	if (KraverGameState)
	{
		KraverGameState->CreatureDeath(DeadCreature, VictimController, AttackerCreature, AttackerController, DamageResult);
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("GameState Class is not KraerGameState Class"));
		return;
	}

	// 리스폰 허용 시 플레이어 캐릭터 리스폰 타이머에 등록
	if (DeadCreature->IsPlayerControlled() && bRespawn)
	{
		FTimerHandle RespawnTimer;
		AKraverPlayer* PlayerCharacter = Cast<AKraverPlayer>(DeadCreature);
		if(PlayerCharacter)
		{
			GetWorldTimerManager().SetTimer(
				RespawnTimer,
				[=]() { RequestRespawn(PlayerCharacter, VictimController); },
				RespawnTime,
				false,
				RespawnTime
			);
		}
	}
}

void AKraverGameMode::RequestRespawn(AKraverPlayer* RespawnPlayer, AController* PlayerController)
{
	if (!RespawnPlayer || !PlayerController)
		return;

	AKraverPlayerController* KraverController = Cast<AKraverPlayerController>(PlayerController);
	if (!KraverController)
		return;

	KR_LOG(Log, TEXT("Player Respawn"));

	// 플레이어 캐릭터 초기화
	RespawnPlayer->Reset();
	RespawnPlayer->Destroy();

	// 플레이어 캐릭터 생성
	RestartPlayer(PlayerController);
}

void AKraverGameMode::RequestDefaultWeapon(AKraverPlayerState* Player, const TArray<TSubclassOf<class AWeapon>>& RequestWeapons)
{
	AKraverPlayer* KraverPlayer = Player->GetOwnerPLayer();
	if (!KraverPlayer)
		return;

	// 플레이어 기본 무기 설정
	GetWorldTimerManager().SetTimerForNextTick([=]()
	{
		SpawnDefaultWeapon(Player, RequestWeapons);
	});
}

void AKraverGameMode::SpawnDefaultWeapon(class AKraverPlayerState* Player, const TArray<TSubclassOf<class AWeapon>>& RequestWeapons)
{
	if (!Player)
		return;

	// 기본 무기 장착
	for (auto& Class : RequestWeapons)
	{
		if (!Class)
			continue;

		AWeapon* NewWeapon = Player->GetOwnerPLayer()->GetWorld()->SpawnActor<AWeapon>(Class);
		Player->GetOwnerPLayer()->CombatComponent->EquipWeapon(NewWeapon);
	}
}

void AKraverGameMode::GameFinishEvent(ETeam WinTeam)
{
	if (IsGameFinish)
		return;

	KR_LOG(Log, TEXT("Game finish"));
	IsGameFinish = true;

	// 게임 속도 설정
	AWorldSettings* WorldSetting = GetWorld()->GetWorldSettings();
	if (WorldSetting)
		WorldSetting->SetTimeDilation(GameFinishTimeDilation);

	// 게임 종료 타이머 설정
	GetWorldTimerManager().SetTimer(GameFinishExitTimer, this, &ThisClass::OnGameFinishExitTimerEvent, GameFinishExitTime * GameFinishTimeDilation, false, GameFinishExitTime * GameFinishTimeDilation);
	KraverGameState->GameFinish(WinTeam);
}

void AKraverGameMode::OnGameFinishExitTimerEvent()
{
	// 게임 종료
	AKraverPlayerController* PC = Cast<AKraverPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PC) return;

	PC->ReturnToMainMenu();
}
