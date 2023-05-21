// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DamageEvents.h"
#include "Kraver/Utility/KraverStructHeaders.h"
#include "KraverDelegate.generated.h"

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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FTakeDamageDele, float, DamageAmount, FKraverDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser, FKraverDamageResult const&, DamageResult);

// Death Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FDeathDele, float, DamageAmount, FKraverDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser, FKraverDamageResult const&, DamageResult);

// GiveDamage Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FGiveDamageDele, AActor*, DamagedActor, float, DamageAmount, FKraverDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser, FKraverDamageResult const&, DamageResult);

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
