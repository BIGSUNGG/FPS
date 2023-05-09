// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DamageEvents.h"
#include "CustomDelegate.generated.h"

// AssassinateDele
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAssassinateDele, AActor*, AssassinatedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAssassinateEndDele);

// Attack Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackStartDele);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackEndDele);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackDele);

// Skill Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSkillDele);

// Equip Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquipWeaponSuccessDele, AWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnEquipWeaponSuccessDele, AWeapon*, Weapon);

// Hold Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHoldWeaponDele, AWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHolsterWeaponDele, AWeapon*, Weapon);

// Damaged Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FTakeDamageDele, float, DamageAmount, FDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FTakePointDamageDele, float, DamageAmount, FPointDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FTakeRadialDamageDele, float, DamageAmount, FRadialDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FDeathDele, float, DamageAmount, FDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser);

// GiveDamage Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FGiveDamageDele, AActor*, DamagedActor, float, DamageAmount, FDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FGivePointDamageDele, AActor*, DamagedActor, float, DamageAmount, FPointDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FGiveRadialDamageDele, AActor*, DamagedActor, float, DamageAmount, FRadialDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser);

// Anim Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAnimNotifyDele);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayMontageDele, UAnimMontage*, PlayedMontage, float, MontageSpeed);

// Add Remove Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAddOnDele, UObject*, Object);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRemoveOnDele, UObject*, Object);

UCLASS()
class KRAVER_API ACustomDelegate : public AActor
{
	GENERATED_BODY()

};
