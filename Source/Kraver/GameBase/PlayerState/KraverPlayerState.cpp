// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayerState.h"
#include KraverPlayer_h
#include KraverGameMode_h
#include KraverPlayerController_h
#include DamageIndicatorSubsystem_h
#include SaveSubsystem_h
#include KraverHud_h

AKraverPlayerState::AKraverPlayerState()
{
	OnPawnSet.AddDynamic(this, &ThisClass::OnPawnSetEvent);
}

void AKraverPlayerState::Server_RequestDefaultWeapon_Implementation(const TArray<TSubclassOf<class AWeapon>>& RequestWeapons)
{
	if (bSpawnDefaultWeapon)
		return;

	bSpawnDefaultWeapon = true;

	AKraverGameMode* GameMode = Cast<AKraverGameMode>(UGameplayStatics::GetGameMode(this));
	GameMode->RequestDefaultWeapon(this, RequestWeapons);
}

void AKraverPlayerState::OnPawnSetEvent(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if (this != Player)
		return;

	if(!NewPawn)
		return;

	OnNewPawn(NewPawn);
}

void AKraverPlayerState::OnNewPawn(APawn* NewPawn)
{
	OwnerPlayer = Cast<AKraverPlayer>(NewPawn);
	if (!OwnerPlayer)
		return;

	OnNewPlayer.Broadcast(OwnerPlayer);
	bSpawnDefaultWeapon = false;

	if (OwnerPlayer->IsLocallyControlled())
	{
		OwnerController = Cast<AKraverPlayerController>(OwnerPlayer->GetController());
		if (!OwnerController)
		{
			KR_LOG(Error, TEXT("Local Controller class is not AKraverPlayerController"));
			return;
		}

		AKraverHud* HUD = Cast<AKraverHud>(OwnerController->GetHUD());
		if (!HUD)
		{
			KR_LOG(Error, TEXT("HUD class is not AKraverHUD"));
			return;
		}
		KR_LOG(Log, TEXT("New Local Player : %s"), *OwnerPlayer->GetName());

		GetGameInstance()->GetSubsystem<UDamageIndicatorSubsystem>()->SetLocalPlayer(OwnerPlayer);
		RequestDefaultWeapon(GetGameInstance()->GetSubsystem<USaveSubsystem>()->GetWeaponArray());
	}


}

void AKraverPlayerState::RequestDefaultWeapon(const TArray<TSubclassOf<class AWeapon>>& RequestWeapons)
{
	Server_RequestDefaultWeapon(RequestWeapons);
}
