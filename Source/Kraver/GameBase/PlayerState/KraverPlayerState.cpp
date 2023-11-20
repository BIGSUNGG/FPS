// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayerState.h"
#include KraverPlayer_h
#include KraverGameMode_h
#include KraverPlayerController_h
#include KraverHud_h

AKraverPlayerState::AKraverPlayerState()
{
	DefaultWeapons.SetNum(3);

	static ConstructorHelpers::FClassFinder<AWeapon> MAIN_WEAPON(TEXT("/Game/ProjectFile/Actor/Weapon/Hitscan/BP_Hitscan_Assult.BP_Hitscan_Assult_C"));
	if (MAIN_WEAPON.Succeeded())
	{
		DefaultWeapons[0] = MAIN_WEAPON.Class;
	}

	static ConstructorHelpers::FClassFinder<AWeapon> SUB_WEAPON(TEXT("/Game/ProjectFile/Actor/Weapon/Hitscan/BP_Hitscan_Pistol.BP_Hitscan_Pistol_C"));
	if (SUB_WEAPON.Succeeded())
	{
		DefaultWeapons[1] = SUB_WEAPON.Class;
	}

	static ConstructorHelpers::FClassFinder<AWeapon> SPECIAL_WEAPON(TEXT("/Game/ProjectFile/Actor/Weapon/Projectile/BP_Projectile_GL.BP_Projectile_GL_C"));
	if (SPECIAL_WEAPON.Succeeded())
	{
		DefaultWeapons[2] = SPECIAL_WEAPON.Class;
	}

	OnPawnSet.AddDynamic(this, &ThisClass::OnPawnSetEvent);
}

void AKraverPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, DefaultWeapons);
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
	}

	if (IS_SERVER())
	{
		AKraverGameMode* GameMode = Cast<AKraverGameMode>(UGameplayStatics::GetGameMode(this));
		GameMode->RequestDefaultWeapon(this, DefaultWeapons);
	}
}

class AKraverPlayer* AKraverPlayerState::GetOwnerPLayer()
{
	return OwnerPlayer ? OwnerPlayer : Cast<AKraverPlayer>(GetOwner());
}

void AKraverPlayerState::SetDefaultWeapons(const TSubclassOf<class AWeapon>& InValue, int Index)
{
	if (!IS_SERVER())
		KR_LOG(Error, TEXT("Called on client"));

	DefaultWeapons[Index] = InValue;
}
