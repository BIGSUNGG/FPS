// Fill out your copyright notice in the Description page of Project Settings.

#include "Melee.h"
#include "Kraver/Character/Creature/Creature.h"
#include "Kraver/Animation/Creature/Soldier/SoldierAnimInstance.h"

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

	if(MaxComboAttack + 1 != AttackMontagesFpp.Num())
		KR_LOG(Warning, TEXT("AttackMontagesFpp size don't match with MaxComboAttack "));
	if (MaxComboAttack + 1 != AttackMontagesTpp.Num())
		KR_LOG(Warning, TEXT("AttackMontagesTpp size don't match with MaxComboAttack "));

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
	bool bSuccess = AWeapon::Equipped(Character);
	return bSuccess;
}

bool AMelee::UnEquipped()
{
	bool bSuccess = AWeapon::UnEquipped();
	ComboEnd();
	return bSuccess;
}

bool AMelee::Hold()
{
	bool bSuccess = AWeapon::Hold();
	return bSuccess;
}

bool AMelee::Holster()
{
	bool bSuccess = AWeapon::Holster();
	ComboEnd();
	return bSuccess;
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

void AMelee::OnAttackStartEvent()
{
	if(bCanInputNextCombo)
		bInputNextCombo = true;

	AWeapon::OnAttackStartEvent();
}

void AMelee::SwingAttack()
{	
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params(NAME_None, false, OwnerCreature);

	bool bResult = SweepMultiByProfile_ExceptWorldObject(
		GetWorld(),
		HitResults,
		OwnerCreature->GetCamera()->GetComponentLocation(),
		OwnerCreature->GetCamera()->GetComponentLocation() + OwnerCreature->GetCamera()->GetForwardVector() * 200.f,
		FQuat::Identity,
		PROFILE_Swing,
		FCollisionShape::MakeSphere(20.f),
		Params
	);

	for (auto& Result : HitResults)
	{
		auto Creature = Cast<ACreature>(Result.GetActor());
		if (IsValid(Result.GetActor()))
		{
			FKraverDamageEvent DamageEvent;
			DamageEvent.DamageType = EKraverDamageType::SWING;
			DamageEvent.bCanParried = true;
			DamageEvent.DamageImpulse = AttackImpulse;
			DamageEvent.HitInfo = Result;
			OwnerCreature->CombatComponent->GiveDamage(Result.GetActor(), AttackDamage, DamageEvent, OwnerCreature->GetController(), this);
		}
	}
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
		return;

	Attack();
}

void AMelee::ComboEnd()
{
	KR_LOG(Log, TEXT("ComboEnd"));

	bCanInputNextCombo = false;
	bInputNextCombo = false;
	bCanAttack = true;
	CurComboAttack = 0;

	if(bAutomaticRepeatCombo && IsAttacking)
		Attack();
}

void AMelee::OnCanInputNextComboEvent()
{
	bCanInputNextCombo = true;
	
	if (bAutomaticCombo && IsAttacking)
		bInputNextCombo = true;
}

void AMelee::OnSwingAttackEvent()
{
	SwingAttack();
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

void AMelee::OnAttackEvent()
{
	if (IsComboAttacking() == false)
	{
		ComboStart();
	}
	OnPlayTppMontage.Broadcast(AttackMontagesTpp[CurComboAttack], 1.f);
	OnPlayFppMontage.Broadcast(AttackMontagesFpp[CurComboAttack], 1.f);

	AWeapon::OnAttackEvent();
}