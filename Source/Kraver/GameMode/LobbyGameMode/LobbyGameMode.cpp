// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num(); // 현재 들어와있는 플레이어의 수를 구함
	if (NumberOfPlayers == 4)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			bUseSeamlessTravel = false;
			UE_LOG(LogTemp, Log, TEXT("Try to change Map"));
			bool Success = World->ServerTravel(FString("/Game/ProjectFile/Maps/GameStartMap?listen")); // 맵 변경
			if(Success)
			{ 
				UE_LOG(LogTemp, Log, TEXT("Success to change Map"));
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Failed to change Map"));				
			}
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Enter Player %s"),*NewPlayer->GetName());
}
