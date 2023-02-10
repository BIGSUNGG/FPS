// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if (NumberOfPlayers == 4)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			bUseSeamlessTravel = false;
			UE_LOG(LogTemp, Log, TEXT("Try to change Map"));
			bool Success = World->ServerTravel(FString("/Game/ProjectFile/Maps/GameStartMap?listen"));
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
