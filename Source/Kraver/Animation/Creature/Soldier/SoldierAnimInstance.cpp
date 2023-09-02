// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierAnimInstance.h"
#include "Kraver/Character/Creature/Soldier/Soldier.h"

USoldierAnimInstance::USoldierAnimInstance() : Super()
{
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
		IsAttacking = Soldier->CombatComponent->GetCurWeapon()->GetIsAttacking();
		IsSubAttacking = Soldier->CombatComponent->GetCurWeapon()->GetIsSubAttacking();

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
