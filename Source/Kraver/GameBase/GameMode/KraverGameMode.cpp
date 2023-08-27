// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverGameMode.h"
#include "Kraver/Character/Creature/Creature.h"

void AKraverGameMode::CreatureEliminated(class ACreature* ElimmedCreature, class AController* VictimController, AController* AttackerController)
{
}

void AKraverGameMode::RequestRespawn(ACreature* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}

	if (Cast<APlayerController>(ElimmedController))
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}
