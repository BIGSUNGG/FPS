// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API USaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USaveSubsystem();
	
public:
	// Getter Setter
	FORCEINLINE const TArray<TSubclassOf<class AWeapon>>& GetWeaponArray() { return WeaponSelectArray; }

	FORCEINLINE void SetWeaponSelect(TSubclassOf<class AWeapon> Value, const FString& Name, int Index);

public:
	inline static int WeaponSelectArraySize = 3;

private:
	// Weapon Select
	TArray<TSubclassOf<class AWeapon>> WeaponSelectArray;
	TArray<FString> WeaponSelectNameArray;

};
