// Fill out your copyright notice in the Description page of Project Settings.


#include "Melee.h"
#include "Kraver/Creature/Creature.h"
#include "Kraver/Anim/Creature/Soldier/SoldierAnimInstance.h"

AMelee::AMelee()
{
	WeaponType = EWeaponType::MELEE;

	bCanFirstInputAttack = false;
	bAutomaticAttack = false;
}

void AMelee::BeginPlay()
{
	AWeapon::BeginPlay();

	if (bAutomaticAttack)
	{
		KR_LOG(Error, TEXT("Melee Weapon's bAutomaticAttack is true"));
		bAutomaticAttack = false;
	}
}

void AMelee::Tick(float DeltaTime)
{
	AWeapon::Tick(DeltaTime);

	if(bInputNextCombo)
	{
		GEngine->AddOnScreenDebugMessage(
			0,
			0.f,
			FColor::White,
			TEXT("InputNextCombo")
		);
	}
}

bool AMelee::Equipped(ACreature* Character)
{
	return AWeapon::Equipped(Character);
}

bool AMelee::UnEquipped()
{
	ComboEnd();
	return AWeapon::UnEquipped();
}

bool AMelee::Hold()
{
	return AWeapon::Hold();
}

bool AMelee::Holster()
{
	ComboEnd();
	return AWeapon::Holster();
}

void AMelee::AddOnOwnerDelegate()
{
	if (OwnerCreature == nullptr)
		return;

	AWeapon::AddOnOwnerDelegate();

	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(OwnerCreature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMelee_SwingAttack.AddDynamic(this, &AMelee::OnSwingAttackEvent);
		AnimInstance->OnMelee_CanInputNextCombo.AddDynamic(this, &AMelee::OnCanInputNextComboEvent);
		AnimInstance->OnMelee_AttackNextCombo.AddDynamic(this, &AMelee::OnAttackNextComboEvent);
		AnimInstance->OnMelee_ComboEnd.AddDynamic(this, &AMelee::OnComboEndEvent);
	}
	
}

void AMelee::RemoveOnOwnerDelegate()
{
	if (OwnerCreature == nullptr)
		return;

	AWeapon::RemoveOnOwnerDelegate();

	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(OwnerCreature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMelee_SwingAttack.RemoveDynamic(this, &AMelee::OnSwingAttackEvent);
		AnimInstance->OnMelee_CanInputNextCombo.RemoveDynamic(this, &AMelee::OnCanInputNextComboEvent);
		AnimInstance->OnMelee_AttackNextCombo.RemoveDynamic(this, &AMelee::OnAttackNextComboEvent);
		AnimInstance->OnMelee_ComboEnd.RemoveDynamic(this, &AMelee::OnComboEndEvent);
	}
}

void AMelee::Attack()
{
	AWeapon::Attack();

	if(CurComboAttack == 0)
		ComboStart();

}

void AMelee::AttackStartEvent()
{
	if(bCanInputNextCombo)
		bInputNextCombo = true;

	AWeapon::AttackStartEvent();
}

void AMelee::ComboStart()
{	
	KR_LOG(Log, TEXT("ComboStart"));
	bCanAttack = false;
}

void AMelee::NextComboAttack()
{
	KR_LOG(Log, TEXT("NextComboAttack"));
	++CurComboAttack;
	if (CurComboAttack > MaxComboAttack)
	{
		KR_LOG(Error, TEXT("CurComboAttack > MaxComboAttack"))
		return;
	}

	Attack();
}

void AMelee::ComboEnd()
{
	KR_LOG(Log, TEXT("ComboEnd"));

	bCanInputNextCombo = false;
	bInputNextCombo = false;
	bCanAttack = true;
	CurComboAttack = 0;
}

void AMelee::OnCanInputNextComboEvent()
{
	bCanInputNextCombo = true;
	
	if (bAutomaticCombo && IsAttacking)
		bInputNextCombo = true;
}

void AMelee::OnSwingAttackEvent()
{
}

void AMelee::OnAttackNextComboEvent()
{
	if (bInputNextCombo)
	{
		bCanInputNextCombo = false;
		bInputNextCombo = false;

		NextComboAttack();
	}
}

void AMelee::OnComboEndEvent()
{
	ComboEnd();
}
