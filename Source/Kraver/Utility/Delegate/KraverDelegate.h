// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DamageEvents.h"
#include "Kraver/Utility/Struct/KraverStruct.h"
#include "KraverDelegate.generated.h"

// Crouch Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCrouchDele, float, HalfHeightAdjust, float, ScaledHalfHeightAdjust);

// Landed Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLandedDele, const FHitResult&, Hit);

// Jump Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpDele);

// Assassinate Delegate
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

// Holster Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHolsterWeaponDele, AWeapon*, Weapon);

// Take Damage
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FTakeDamageDele, float, DamageAmount, FDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser, FKraverDamageResult const&, DamageResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FTakePointDamageDele, float, DamageAmount, FPointDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser, FKraverDamageResult const&, DamageResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FTakeRadialDamageDele, float, DamageAmount, FRadialDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser, FKraverDamageResult const&, DamageResult);

// GiveDamage Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FGiveDamageDele, AActor*, DamagedActor, float, DamageAmount, FDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser, FKraverDamageResult const&, DamageResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FGivePointDamageDele, AActor*, DamagedActor, float, DamageAmount, FPointDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser, FKraverDamageResult const&, DamageResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FGiveRadialDamageDele, AActor*, DamagedActor, float, DamageAmount, FRadialDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser, FKraverDamageResult const&, DamageResult);

// Death Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FDeathDele, float, DamageAmount, FDamageEvent const&, DamageEvent, AController*, EventInstigator, AActor*, DamageCauser, FKraverDamageResult const&, DamageResult);

// Anim Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAnimNotifyDele);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayMontageDele, UAnimMontage*, PlayedMontage, float, MontageSpeed);

// Add Remove Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAddOnDele, UObject*, Object);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRemoveOnDele, UObject*, Object);

// Make NewWeaponPrimitivaeInfo
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMakeNewPrimitiveInfoDele, int, Index);

// Destroy Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDestroyDele, AActor*, Actor);

// Impact Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FImpactDele, AActor*, Bullet, AActor*, OtherActor, UPrimitiveComponent*, OtherComponent, const FHitResult&, Hit);

UCLASS()
class KRAVER_API ACustomDelegate : public AActor
{
	GENERATED_BODY()

};
