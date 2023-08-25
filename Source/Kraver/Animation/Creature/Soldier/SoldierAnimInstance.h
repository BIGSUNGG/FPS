// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Animation/Creature/CreatureAnimInstance.h"
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

protected:
	// Animation Notify
	UFUNCTION()
		void AnimNotify_Weapon_Holster();
	UFUNCTION()
		void AnimNotify_Weapon_Unholster();

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

	UFUNCTION()
		void AnimNotify_Reload_Insert_Magazine();
	UFUNCTION()
		void AnimNotify_Reload_Grab_Magazine();

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
};
