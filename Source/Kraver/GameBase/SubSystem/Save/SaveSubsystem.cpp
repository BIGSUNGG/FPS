// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSubsystem.h"

USaveSubsystem::USaveSubsystem()
{
	WeaponSelectArray.SetNum(WeaponSelectArraySize);
	WeaponSelectNameArray.SetNum(WeaponSelectArraySize);
}

FORCEINLINE void USaveSubsystem::SetWeaponSelect(TSubclassOf<class AWeapon> Value, const FString& Name, int Index)
{
	WeaponSelectArray[Index] = Value;
	WeaponSelectNameArray[Index] = Name;
}
