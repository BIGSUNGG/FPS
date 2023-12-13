// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierAnimInstance.h"
#include "Kraver/Character/Creature/Soldier/Soldier.h"

USoldierAnimInstance::USoldierAnimInstance() : Super()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MONTAGE_TppHolster(TEXT("Engine.AnimMontage'/Game/InfimaGames/AnimatedLowPolyWeapons/Art/Characters/Animations/ARs/AM_TP_CH_AR_01_Holster.AM_TP_CH_AR_01_Holster'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MONTAGE_TppUnholster(TEXT("Engine.AnimMontage'/Game/InfimaGames/AnimatedLowPolyWeapons/Art/Characters/Animations/ARs/AM_TP_CH_AR_01_Unholster.AM_TP_CH_AR_01_Unholster'"));

	if (MONTAGE_TppHolster.Succeeded())
		HolsterMontage = MONTAGE_TppHolster.Object;
	else
		KR_LOG(Error, TEXT("Failed to find HolsterMontageTpp asset"));

	if (MONTAGE_TppUnholster.Succeeded())
		UnholsterMontage = MONTAGE_TppUnholster.Object;
	else
		KR_LOG(Error, TEXT("Failed to find UnholsterMontageTpp asset"));
}

void USoldierAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Soldier = Cast<ASoldier>(Creature);
}

void USoldierAnimInstance::NativeUpdateAnimation(float DeltaSeconds) 
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Soldier == nullptr)
		return;

	CurWeapon = Soldier->CombatComponent->GetCurWeapon();
	IsEquippingWeapon = (CurWeapon != nullptr);
	if (IsEquippingWeapon)
	{
		IsAttacking = Soldier->CombatComponent->GetCurWeapon()->IsAttacking();
		IsSubAttacking = Soldier->CombatComponent->GetCurWeapon()->IsSubAttacking();

		AnimWeaponIdleTpp = CurWeapon->GetAnimIdleTpp();
		AnimWeaponSprintTpp = CurWeapon->GetAnimSprintTpp();
		AnimWeaponSubAttackTpp = CurWeapon->GetSubAttackTpp();
		AnimWeaponMovementTpp = CurWeapon->GetAnimMovementTpp();
		CurWeaponType = CurWeapon->GetWeaponType();
	}
	else
	{
		IsAttacking = false;
		IsSubAttacking = false;
	}
}

void USoldierAnimInstance::PlayHolsteWeaponrMontage()
{
	Montage_Play(HolsterMontage);
}

void USoldierAnimInstance::PlayUnholsteWeaponrMontage()
{
	Montage_Play(UnholsterMontage);
}

void USoldierAnimInstance::AnimNotify_Weapon_Holster()
{
	OnWeapon_Holster.Broadcast();
}

void USoldierAnimInstance::AnimNotify_Weapon_Unholster()
{
	OnWeapon_Unholster.Broadcast();
}

void USoldierAnimInstance::AnimNotify_Melee_SwingAttack()
{
	OnMelee_SwingAttack.Broadcast();
}

void USoldierAnimInstance::AnimNotify_Melee_CanInputNextCombo()
{
	OnMelee_CanInputNextCombo.Broadcast();
}

void USoldierAnimInstance::AnimNotify_Melee_AttackNextCombo()
{
	OnMelee_AttackNextCombo.Broadcast();
}

void USoldierAnimInstance::AnimNotify_Melee_ComboEnd()
{
	OnMelee_ComboEnd.Broadcast();
}

void USoldierAnimInstance::AnimNotify_Melee_AssassinateAttack()
{
	OnMelee_AssassinateAttack.Broadcast();
}

void USoldierAnimInstance::AnimNotify_Melee_AssassinateEnd()
{
	OnMelee_AssassinateEnd.Broadcast();
}

void USoldierAnimInstance::AnimNotify_Reload_Insert_Magazine()
{
	OnReload_Insert_Magazine.Broadcast();
}

void USoldierAnimInstance::AnimNotify_Reload_Grab_Magazine()
{
	OnReload_Grab_Magazine.Broadcast();
}

void USoldierAnimInstance::AnimNotify_Reload_OpenFinish()
{
	OnReload_OpenFinish.Broadcast();
}

void USoldierAnimInstance::AnimNotify_Reload_InsertFinish()
{
	OnReload_InsertFinish.Broadcast();
}
