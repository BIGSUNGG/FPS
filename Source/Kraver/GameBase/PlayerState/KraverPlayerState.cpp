// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayerState.h"
#include KraverPlayer_h
#include KraverPlayerController_h
#include DamageIndicatorSubsystem_h
#include KraverHud_h

AKraverPlayerState::AKraverPlayerState()
{
	OnPawnSet.AddDynamic(this, &ThisClass::OnPawnSetEvent);
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
