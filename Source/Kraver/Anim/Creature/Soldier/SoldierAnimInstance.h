// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Anim/Creature/CreatureAnimInstance.h"
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

	virtual void FabrikLeftHand(USkeletalMeshComponent* HandMesh, USkeletalMeshComponent* WeaponMesh, FTransform& Transform);

protected:
	// Anim Notify
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
		void AnimNotify_RefillAmmo();
public:
	FAnimNotifyDele OnMelee_SwingAttack;
	FAnimNotifyDele OnMelee_CanInputNextCombo;
	FAnimNotifyDele OnMelee_AttackNextCombo;
	FAnimNotifyDele OnMelee_ComboEnd;
	FAnimNotifyDele OnMelee_AssassinateAttack;
	FAnimNotifyDele OnMelee_AssassinateEnd;
	FAnimNotifyDele OnRefillAmmo;

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

	UPROPERTY(BlueprintReadOnly, Category = "Data|Fabrik", meta = (AllowPrivateAccess = "true"))
		FTransform LeftHandFabrikTransform; // 왼쪽손에 적용할 트랜스폼

	// Anim
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Anim", Meta = (AllowPrivateAccess = true))
		UAnimSequence* AnimWeaponIdleTpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Anim", Meta = (AllowPrivateAccess = true))
		UAnimSequence* AnimWeaponSubAttackTpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Weapon|Anim", Meta = (AllowPrivateAccess = true))
		UBlendSpace* AnimWeaponMovementTpp;	
};
