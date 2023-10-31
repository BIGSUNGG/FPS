// Fill out your copyright notice in the Description page of Project Settings.

#include "Melee.h"
#include Creature_h
#include SoldierAnimInstance_h

AMelee::AMelee()
{
	WeaponType = EWeaponType::MELEE;

	bCanFirstInputAttack = false;
	bAutomaticAttack = false;
}

void AMelee::BeginPlay()
{
	Super::BeginPlay();

	if (bAutomaticAttack) // ���� ����� bAutomaticAttack�� ������� ����
	{
		KR_LOG(Error, TEXT("Melee Weapon's bAutomaticAttack is true"));
		bAutomaticAttack = false;
	}

	// �޺� ��Ÿ���� ���� �ִ� �޺��� �ٸ���
	if(MaxComboAttack + 1 != AttackMontagesFpp.Num())
		KR_LOG(Warning, TEXT("AttackMontagesFpp size don't match with MaxComboAttack "));
	if (MaxComboAttack + 1 != AttackMontagesTpp.Num())
		KR_LOG(Warning, TEXT("AttackMontagesTpp size don't match with MaxComboAttack "));

}

void AMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMelee::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, CurComboAttack, COND_SkipOwner);
}

bool AMelee::OnServer_Equipped(ACreature* Character)
{
	bool bSuccess = Super::OnServer_Equipped(Character);
	return bSuccess;
}

bool AMelee::OnServer_UnEquipped()
{
	bool bSuccess = Super::OnServer_UnEquipped();
	ComboEnd();
	return bSuccess;
}

bool AMelee::OnLocal_Unholster()
{
	bool bSuccess = Super::OnLocal_Unholster();
	return bSuccess;
}

bool AMelee::OnLocal_Holster()
{
	bool bSuccess = Super::OnLocal_Holster();
	ComboEnd();
	return bSuccess;
}

void AMelee::OnLocal_AddOnOwnerDelegate()
{
	if (OwnerCreature == nullptr)
		return;

	Super::OnLocal_AddOnOwnerDelegate();

	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(OwnerCreature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMelee_SwingAttack.AddDynamic(this, &AMelee::OnSwingAttackEvent);
		AnimInstance->OnMelee_CanInputNextCombo.AddDynamic(this, &AMelee::OnCanInputNextComboEvent);
		AnimInstance->OnMelee_AttackNextCombo.AddDynamic(this, &AMelee::OnAttackNextComboEvent);
		AnimInstance->OnMelee_ComboEnd.AddDynamic(this, &AMelee::OnComboEndEvent);
	}	
}

void AMelee::OnLocal_RemoveOnOwnerDelegate()
{
	if (OwnerCreature == nullptr)
		return;

	Super::OnLocal_RemoveOnOwnerDelegate();

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

	Super::OnAttackStartEvent();
}

void AMelee::ComboStart()
{	
	if (IsComboAttacking()) // �޺����ΰ�
		return;

	KR_LOG(Log, TEXT("ComboStart"));
	bCanAttack = false;
}

void AMelee::NextComboAttack()
{
	KR_LOG(Log, TEXT("NextComboAttack"));
	++CurComboAttack;
	if (CurComboAttack > MaxComboAttack)
		return;

	TryAttack(); // ���� �޺� ����
}

void AMelee::ComboEnd()
{
	KR_LOG(Log, TEXT("ComboEnd"));

	bCanInputNextCombo = false;
	bInputNextCombo = false;
	bCanAttack = true;
	CurComboAttack = 0;

	if(bAutomaticRepeatCombo && bIsAttacking) // ���� ���̰� �޺��� �ݺ��� �� �ִ���
		TryAttack();
}

void AMelee::StartSwingEvent()
{
	OnPlayTppMontage.Broadcast(AttackMontagesTpp[CurComboAttack], 1.f);
	OnPlayFppMontage.Broadcast(AttackMontagesFpp[CurComboAttack], 1.f);
}

void AMelee::Server_SwingResult_Implementation(const TArray<FHitResult>& Results)
{
	for (auto& Result : Results)
	{
		auto Creature = Cast<ACreature>(Result.GetActor());
		if (IsValid(Result.GetActor()))
		{
			FPointDamageEvent DamageEvent(AttackDamage, Result, OwnerCreature->GetCamera()->GetForwardVector(), AttackDamageType);
			OwnerCreature->CombatComponent->OnServer_GiveDamage(Result.GetActor(), AttackDamage, DamageEvent, OwnerCreature->GetController(), this);
		}
	}
}

void AMelee::Server_SwingStart_Implementation(int Combo)
{
	CurComboAttack = Combo;
	Multicast_SwingStart(Combo);
}

void AMelee::Multicast_SwingStart_Implementation(int Combo)
{
	if (!OwnerCreature || !OwnerCreature->IsLocallyControlled())
	{
		CurComboAttack = Combo;
		StartSwingEvent();
	}
}

void AMelee::OnCanInputNextComboEvent()
{
	bCanInputNextCombo = true;
	
	if (bAutomaticCombo && bIsAttacking)
		bInputNextCombo = true;
}

void AMelee::OnSwingAttackEvent()
{
	// ���� ������ �ִ� ������Ʈ Ʈ���̽�
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

	Server_SwingResult(HitResults);
}

void AMelee::OnAttackNextComboEvent()
{
	if (bInputNextCombo) // ���� ������ �Էµ��ִ���
	{
		bCanInputNextCombo = false;
		bInputNextCombo = false;

		NextComboAttack(); // ���� �޺� ����
	}
}

void AMelee::OnComboEndEvent()
{
	ComboEnd();
}

void AMelee::Attack()
{
	if (IsComboAttacking() == false) // �޺� ������ ������
		ComboStart(); // �޺� ����
	
	StartSwingEvent(); // ���� ��Ÿ�� ȣ��
	Server_SwingStart(CurComboAttack);

	Super::Attack();
}