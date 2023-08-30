// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayerState.h"
#include "Kraver/Character/Creature/Soldier/KraverPlayer/KraverPlayer.h"
#include "Kraver/GameBase/PlayerController/KraverPlayerController.h"
#include "Kraver/GameBase/SubSystem/DamageIndicatorSubsystem.h"
#include "Kraver/GameBase/Ui/HUD/KraverHUD.h"

AKraverPlayerState::AKraverPlayerState()
{
	OnPawnSet.AddDynamic(this, &ThisClass::OnPawnSetEvent);
}

void AKraverPlayerState::CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult)
{
	Multicast_CreatureDeath(DeadCreature, VictimController, AttackerActor, AttackerController, DamageResult);
}

void AKraverPlayerState::Multicast_CreatureDeath_Implementation(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult)
{
	OnCreatureDeath.Broadcast(DeadCreature, VictimController, AttackerActor, AttackerController, DamageResult);
}

void AKraverPlayerState::OnPawnSetEvent(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if(!NewPawn)
		return;
	
	AKraverPlayer* NewPlayer = Cast<AKraverPlayer>(NewPawn);
	if (!NewPlayer)
	{
		KR_LOG(Error, TEXT("New Player class is not AKraverPlayer"));
		return;
	}
	OnNewPlayer.Broadcast(NewPlayer);

	if(NewPawn->IsLocallyControlled())
	{
		LocalPlayer = NewPlayer;
		LocalController = Cast<AKraverPlayerController>(LocalPlayer->GetController());
		if (!LocalController)
		{
			KR_LOG(Error, TEXT("Local Controller class is not AKraverPlayerController"));
			return;
		}

		HUD = Cast<AKraverHUD>(LocalController->GetHUD());
		if (!HUD)
		{
			KR_LOG(Error, TEXT("HUD class is not AKraverHUD"));
			return;
		}
		KR_LOG(Log, TEXT("New Local Player : %s"), *LocalPlayer->GetName());

		GetGameInstance()->GetSubsystem<UDamageIndicatorSubsystem>()->SetLocalPlayer(LocalPlayer);
		OnNewLocalPlayer.Broadcast(LocalPlayer);
	}
}
