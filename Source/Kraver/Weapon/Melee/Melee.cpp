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
		AnimInstance->OnMelee_AssassinateAttack.AddDynamic(this, &AMelee::OnAssassinateAttackEvent);
		AnimInstance->OnMelee_AssassinateEnd.AddDynamic(this, &AMelee::OnAssassinateEndEvent);
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
		AnimInstance->OnMelee_AssassinateAttack.RemoveDynamic(this, &AMelee::OnAssassinateAttackEvent);
		AnimInstance->OnMelee_AssassinateEnd.RemoveDynamic(this, &AMelee::OnAssassinateEndEvent);
	}
}

void AMelee::Attack()
{
	if (IsComboAttacking() == false)
	{
		if (bCanAssassination)
		{
			pair<bool, FHitResult> CanAssassinate = CalculateCanAssassinate();	
			if (CanAssassinate.first)
				Assassinate(CanAssassinate.second.GetActor());
		}
		
		if(!IsAssassinating)
			ComboStart();
	}
	AWeapon::Attack();
}

void AMelee::AttackStartEvent()
{
	if(bCanInputNextCombo)
		bInputNextCombo = true;

	AWeapon::AttackStartEvent();
}

void AMelee::SwingAttack()
{	
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params(NAME_None, false, OwnerCreature);

	bool bResult = SweepMultiByChannel_ExceptWorldObject(
		GetWorld(),
		HitResults,
		OwnerCreature->GetCamera()->GetComponentLocation(),
		OwnerCreature->GetCamera()->GetComponentLocation() + OwnerCreature->GetCamera()->GetForwardVector() * 200.f,
		FQuat::Identity,
		ECC_SWING,
		FCollisionShape::MakeSphere(20.f),
		Params
	);

	for (auto& Result : HitResults)
	{
		auto Creature = Cast<ACreature>(Result.GetActor());
		if (IsValid(Result.GetActor()))
		{
			FPointDamageEvent damageEvent;
			damageEvent.HitInfo = Result;
			damageEvent.ShotDirection = OwnerCreature->GetCamera()->GetForwardVector();
			OwnerCreature->CombatComponent->GiveDamage(Result.GetActor(), AttackDamage, damageEvent, OwnerCreature->GetController(), this);
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

	if(bAutomaticRepeatCombo && IsAttacking)
		Attack();
}

std::pair<bool, FHitResult> AMelee::CalculateCanAssassinate()
{
	pair<bool, FHitResult> Result;

	FCollisionQueryParams Params(NAME_None, false, this);
	Params.AddIgnoredActor(OwnerCreature);

	bool bSuccess = GetWorld()->SweepSingleByChannel(
		Result.second,
		OwnerCreature->GetCamera()->GetComponentLocation(),
		OwnerCreature->GetCamera()->GetComponentLocation() + OwnerCreature->GetCamera()->GetForwardVector() * 30.f,
		FQuat::Identity,
		ECC_ASSASSINATION,
		FCollisionShape::MakeSphere(34.f),
		Params
	);

	if (bSuccess)
	{
		float YawDifference = Result.second.GetActor()->GetActorRotation().Yaw - OwnerCreature->GetActorRotation().Yaw;
		
		Result.first = (-45.f < YawDifference && 45.f > YawDifference);
	}
	else
		Result.first = false;


	return Result;
}

void AMelee::Assassinate(AActor* Actor)
{
	if(!Actor)
		return;

	ACreature* Creature = Cast<ACreature>(Actor);
	if(!Creature)
		return;

	IsAssassinating = true;
	CurAssasinatedCreature = Creature;

	AttackEndEvent();
	SubAttackEndEvent();

	OnAssassinate.Broadcast(Actor);
	Server_Assassinate(Actor);
}

void AMelee::Server_Assassinate_Implementation(AActor* Actor)
{
	ACreature* Creature = Cast<ACreature>(Actor);
	if (!Creature)
		return;

	FAssassinateInfo AssassinateInfo;
	AssassinateInfo.AssassinatedMontagesFpp = AssassinatedMontagesFpp;
	AssassinateInfo.AssassinatedMontagesTpp = AssassinatedMontagesTpp;

	Creature->Assassinated(OwnerCreature, AssassinateInfo);
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

void AMelee::OnAssassinateAttackEvent()
{
	FDamageEvent TempDamageEvent;
	OwnerCreature->CombatComponent->GiveDamage(CurAssasinatedCreature, AttackDamage * 2, TempDamageEvent, OwnerCreature->GetController(), this);

	OwnerCreature->RpcComponent->SetSimulatedPhysics(CurAssasinatedCreature->GetMesh(), false);
}

void AMelee::OnAssassinateEndEvent()
{
	IsAssassinating = false;
	if(CurAssasinatedCreature->CombatComponent->GetCurHp() <= 0)
		OwnerCreature->RpcComponent->SetSimulatedPhysics(CurAssasinatedCreature->GetMesh(), true);

	OnAssassinateEnd.Broadcast();
}

UAnimMontage* AMelee::GetAttackMontageTpp()
{
	if(IsAssassinating)
		return AssassinateMontagesTpp;

	return AttackMontagesTpp[CurComboAttack];
}

UAnimMontage* AMelee::GetAttackMontageFpp()
{
	if (IsAssassinating)
		return AssassinateMontagesFpp;

	return AttackMontagesFpp[CurComboAttack];
}
