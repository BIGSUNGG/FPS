// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include CreatureAnimInstance_h
#include "SoldierAnimInstance.generated.h"

/**
 * 
 */

class ASoldier;
class AWeapon;

UCLASS()
class KRAVER_API USoldierAnimInstance : public UCreatureAnimInstance
{
	GENERATED_BODY()
	
public:
	USoldierAnimInstance();
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	void PlayHolsteWeaponrMontage();
	void PlayUnholsteWeaponrMontage();

protected:
#pragma region AnimNotify
	// Animation Notify
		// Weapon
	UFUNCTION()
	void AnimNotify_Weapon_Holster();
	UFUNCTION()
	void AnimNotify_Weapon_Unholster();

		// Melee
	UFUNCTION()
	void AnimNotify_Melee_SwingAttack();
	UFUNCTION()
	void AnimNotify_Melee_CanInputNextCombo();
	UFUNCTION()
	void AnimNotify_Melee_AttackNextCombo();
	UFUNCTION()
	void AnimNotify_Melee_ComboEnd();
	UFUNCTION()
	void AnimNotify_Melee_AssassinateAttack();
	UFUNCTION()
	void AnimNotify_Melee_AssassinateEnd();

		// Reload
	UFUNCTION()
	void AnimNotify_Reload_Insert_Magazine();
	UFUNCTION()
	void AnimNotify_Reload_Grab_Magazine();
	UFUNCTION()
	void AnimNotify_Reload_OpenFinish();
	UFUNCTION()
	void AnimNotify_Reload_InsertFinish();
#pragma endregion AnimNotify

public:
	bool IsPlayingHolsterWeapon() { return Montage_IsPlaying(HolsterMontage); }
	bool IsPlayingUnholsterWeapon() { return Montage_IsPlaying(UnholsterMontage); }
	
public:
	FAnimNotifyDele OnWeapon_Holster;
	FAnimNotifyDele OnWeapon_Unholster;

	FAnimNotifyDele OnMelee_SwingAttack;
	FAnimNotifyDele OnMelee_CanInputNextCombo;
	FAnimNotifyDele OnMelee_AttackNextCombo;
	FAnimNotifyDele OnMelee_ComboEnd;
	FAnimNotifyDele OnMelee_AssassinateAttack;
	FAnimNotifyDele OnMelee_AssassinateEnd;

	FAnimNotifyDele OnReload_Insert_Magazine;
	FAnimNotifyDele OnReload_Grab_Magazine;
	FAnimNotifyDele OnReload_OpenFinish;
	FAnimNotifyDele OnReload_InsertFinish;

protected:
	ASoldier* Soldier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Weapon", Meta = (AllowPrivateAccess = true))
	AWeapon* CurWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Weapon", Meta = (AllowPrivateAccess = true))
	EWeaponType CurWeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Weapon", Meta = (AllowPrivateAccess = true))
	bool IsEquippingWeapon; // 캐릭터가 무기를 장착하고 있는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Weapon", Meta = (AllowPrivateAccess = true))
	bool IsAttacking; // 캐릭터 무기가 공격하고 있는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Weapon", Meta = (AllowPrivateAccess = true))
	bool IsSubAttacking; // 캐릭터 무기가 보조 공격하고 있는지 여부

	// Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* AnimWeaponIdleTpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* AnimWeaponSprintTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* AnimWeaponSubAttackTpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Animation", Meta = (AllowPrivateAccess = true))
	UBlendSpace* AnimWeaponMovementTpp;	

	// Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* HolsterMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* UnholsterMontage;
};
