// Fill out your copyright notice in the Description page of Project Settings.

#include "DataSubsystem.h"
#include Weapon_h

UDataSubsystem::UDataSubsystem()
{
	WeaponSelectArray.SetNum(WeaponSelectArraySize);
	WeaponSelectNameArray.SetNum(WeaponSelectArraySize);

	static ConstructorHelpers::FClassFinder<AWeapon> MAIN_WEAPON(TEXT("/Game/ProjectFile/Actor/Weapon/Hitscan/BP_Hitscan_Assult.BP_Hitscan_Assult_C"));
	if (MAIN_WEAPON.Succeeded())
	{
		WeaponSelectArray[0] = MAIN_WEAPON.Class;
		WeaponSelectNameArray[0] = "AR";
	}

	static ConstructorHelpers::FClassFinder<AWeapon> SUB_WEAPON(TEXT("/Game/ProjectFile/Actor/Weapon/Hitscan/BP_Hitscan_Pistol.BP_Hitscan_Pistol_C"));
	if (SUB_WEAPON.Succeeded())
	{
		WeaponSelectArray[1] = SUB_WEAPON.Class;
		WeaponSelectNameArray[1] = "Pistol";
	}

	static ConstructorHelpers::FClassFinder<AWeapon> SPECIAL_WEAPON(TEXT("/Game/ProjectFile/Actor/Weapon/Projectile/BP_Projectile_GL.BP_Projectile_GL_C"));
	if (SPECIAL_WEAPON.Succeeded())
	{
		WeaponSelectArray[2] = SPECIAL_WEAPON.Class;
		WeaponSelectNameArray[2] = "Grenade Launcher";
	}

}

FORCEINLINE void UDataSubsystem::SetWeaponSelect(TSubclassOf<class AWeapon> Value, const FString& Name, int Index)
{
	WeaponSelectArray[Index] = Value;
	WeaponSelectNameArray[Index] = Name;
}
