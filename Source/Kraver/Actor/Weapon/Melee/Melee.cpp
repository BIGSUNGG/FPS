// Fill out your copyright notice in the Description page of Project Settings.

#include "Melee.h"
#include Creature_h
#include SoldierAnimInstance_h

AMelee::AMelee()
{
	WeaponType = EWeaponType::MELEE;
}

void AMelee::BeginPlay()
{
	Super::BeginPlay();

	// 콤보 몽타주의 수가 최대 콤보와 다른지
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

bool AMelee::OnServer_UnEquipped()
{
	bool bSuccess = Super::OnServer_UnEquipped();
	ComboEnd();
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
		AnimInstance->OnMelee_CanInputNextCombo.RemoveDynamic(this, &AMelee::OnCanInputNextComboEvent);
		AnimInstance->OnMelee_AttackNextCombo.RemoveDynamic(this, &AMelee::OnAttackNextComboEvent);
		AnimInstance->OnMelee_ComboEnd.RemoveDynamic(this, &AMelee::OnComboEndEvent);
	}
}

void AMelee::OnServer_AddOnOwnerDelegate()
{
	Super::OnServer_AddOnOwnerDelegate();

	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(OwnerCreature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMelee_SwingAttack.AddDynamic(this, &AMelee::OnServerSwingAttackEvent);
	}
}

void AMelee::OnServer_RemoveOnOwnerDelegate()
{
	Super::OnServer_RemoveOnOwnerDelegate();

	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(OwnerCreature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMelee_SwingAttack.RemoveDynamic(this, &AMelee::OnServerSwingAttackEvent);
	}
}

void AMelee::OnLocalAttackStartEvent()
{
	if(bCanInputNextCombo)
		bInputNextCombo = true;

	if (bIsAttacking == true) // 이미 공격중이면 
		return;

	if (!CanAttack()) // 공격이 불가능하면
		return;

	Super::OnLocalAttackStartEvent();

	TryAttack();
}

void AMelee::ComboStart()
{	
	if (IsComboAttacking()) // 콤보중인가
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

	TryAttack(); // 다음 콤보 실행
}

void AMelee::ComboEnd()
{
	KR_LOG(Log, TEXT("ComboEnd"));

	bCanInputNextCombo = false;
	bInputNextCombo = false;
	bCanAttack = true;
	CurComboAttack = 0;

	Server_ComboEnd();

	if(bAutomaticRepeatCombo && bIsAttacking) // 공격 중이고 콤보를 반복할 수 있는지
		TryAttack();
}

void AMelee::Server_ComboEnd_Implementation()
{
	CurComboAttack = 0;
}

void AMelee::SwingStartEvent()
{
	if (AttackMontagesTpp.Num() <= CurComboAttack)
	{
		KR_LOG(Error, TEXT("CurCombo is too big %d"), CurComboAttack);
		return;
	}

	OnPlayTppMontage.Broadcast(AttackMontagesTpp[CurComboAttack], 1.f);
	OnPlayFppMontage.Broadcast(AttackMontagesFpp[CurComboAttack], 1.f);
}

void AMelee::OnServer_SwingResult(const TArray<FHitResult>& Results)
{
	ERROR_IF_CALLED_ON_CLIENT();

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

void AMelee::OnServer_SwingStart()
{
	Multicast_SwingStart(CurComboAttack);

	if (!OwnerCreature->IsLocallyControlled())
		CurComboAttack++;
}

void AMelee::Multicast_SwingStart_Implementation(int Combo)
{
	CurComboAttack = Combo;
	SwingStartEvent();
}

void AMelee::OnCanInputNextComboEvent()
{
	bCanInputNextCombo = true;
	
	if (bAutomaticCombo && bIsAttacking)
		bInputNextCombo = true;
}

void AMelee::OnServerSwingAttackEvent()
{
	ERROR_IF_CALLED_ON_CLIENT();

	// 공격 범위에 있는 오브젝트 트레이스
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

	OnServer_SwingResult(HitResults);
}

void AMelee::OnAttackNextComboEvent()
{
	if (bInputNextCombo) // 다음 공격이 입력되있는지
	{
		bInputNextCombo = false;
		bCanInputNextCombo = false;

		NextComboAttack(); // 다음 콤보 실행
	}
}

void AMelee::OnComboEndEvent()
{
	ComboEnd();
}

void AMelee::Attack()
{
	if (IsComboAttacking() == false) // 콤보 중이지 않으면
		ComboStart(); // 콤보 시작
	
	Super::Attack();
}

void AMelee::Server_Attack_Implementation()
{
	ERROR_IF_CALLED_ON_CLIENT();
	Super::Server_Attack_Implementation();

	OnServer_SwingStart();
}
