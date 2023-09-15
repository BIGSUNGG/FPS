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
	
	OwnerPlayer = Cast<AKraverPlayer>(NewPawn);
	if (!OwnerPlayer)
		return;

	OnNewPlayer.Broadcast(OwnerPlayer);

	if(OwnerPlayer->IsLocallyControlled())
	{
		OwnerController = Cast<AKraverPlayerController>(OwnerPlayer->GetController());
		if (!OwnerController)
		{
			KR_LOG(Error, TEXT("Local Controller class is not AKraverPlayerController"));
			return;
		}

		AKraverHUD* HUD = Cast<AKraverHUD>(OwnerController->GetHUD());
		if (!HUD)
		{
			KR_LOG(Error, TEXT("HUD class is not AKraverHUD"));
			return;
		}
		KR_LOG(Log, TEXT("New Local Player : %s"), *OwnerPlayer->GetName());

		GetGameInstance()->GetSubsystem<UDamageIndicatorSubsystem>()->SetLocalPlayer(OwnerPlayer);
		OnNewLocalPlayer.Broadcast(OwnerPlayer);
	}
}
