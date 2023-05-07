// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAssassinateComponent.h"
#include "Kraver/Creature/Creature.h"
#include "Kraver/Anim/Creature/Soldier/SoldierAnimInstance.h"
#include "Kraver/Weapon/Melee/Melee.h"

void UWeaponAssassinateComponent::BeginPlay()
{
	UWeaponComponent::BeginPlay();

	OwnerMelee = Cast<AMelee>(OwnerWeapon);
	if (OwnerMelee == nullptr)
	{
		KR_LOG(Error, TEXT("Owner is not Melee weapon"));
		return;
	}
}

void UWeaponAssassinateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UWeaponComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UWeaponAssassinateComponent::OnAddOnDelegateEvent(UObject* Object)
{
	UWeaponComponent::OnAddOnDelegateEvent(Object);

	ACreature* Creature = Cast<ACreature>(Object);
	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(Creature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMelee_AssassinateAttack.AddDynamic(this, &UWeaponAssassinateComponent::OnAssassinateAttackEvent);
		AnimInstance->OnMelee_AssassinateEnd.AddDynamic(this, &UWeaponAssassinateComponent::OnAssassinateEndEvent);
		KR_LOG(Log, TEXT("H"));
	}
}

void UWeaponAssassinateComponent::OnRemoveOnDelegateEvent(UObject* Object)
{
	UWeaponComponent::OnRemoveOnDelegateEvent(Object);

	ACreature* Creature = Cast<ACreature>(Object);
	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(Creature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMelee_AssassinateAttack.RemoveDynamic(this, &UWeaponAssassinateComponent::OnAssassinateAttackEvent);
		AnimInstance->OnMelee_AssassinateEnd.RemoveDynamic(this, &UWeaponAssassinateComponent::OnAssassinateEndEvent);
	}
}

void UWeaponAssassinateComponent::Assassinate(AActor* Actor)
{
	if (!Actor)
		return;

	ACreature* Creature = Cast<ACreature>(Actor);
	if (!Creature)
		return;

	IsAssassinating = true;
	CurAssassinatedCreature = Creature;

	OwnerMelee->OnAttackEndEvent();
	OwnerMelee->OnSubAttackEndEvent();

	OwnerMelee->OnPlayTppMontage.Broadcast(AssassinateMontagesTpp, 1.f);
	OwnerMelee->OnPlayFppMontage.Broadcast(AssassinateMontagesFpp, 1.f);
	OnAssassinate.Broadcast(Actor);
	Server_Assassinate(Actor);
}

std::pair<bool, FHitResult> UWeaponAssassinateComponent::CalculateCanAssassinate()
{
	pair<bool, FHitResult> Result;

	FCollisionQueryParams Params(NAME_None, false, OwnerMelee);
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
		FTransform OwnerTransform = OwnerCreature->GetTransform();
		FTransform EnemyTransform = Result.second.GetActor()->GetTransform();
		FTransform RelativeTransform = OwnerTransform.GetRelativeTransform(EnemyTransform);
		FRotator RelativeRotation = RelativeTransform.Rotator();
		float YawDifference = RelativeRotation.Yaw;

		Result.first = (-25.f < YawDifference && 25.f > YawDifference);
	}
	else
		Result.first = false;


	return Result;
}

void UWeaponAssassinateComponent::Server_Assassinate_Implementation(AActor* Actor)
{
	ACreature* Creature = Cast<ACreature>(Actor);
	if (!Creature)
		return;

	FAssassinateInfo AssassinateInfo;
	AssassinateInfo.AssassinatedMontagesFpp = AssassinatedMontagesFpp;
	AssassinateInfo.AssassinatedMontagesTpp = AssassinatedMontagesTpp;

	Creature->Assassinated(OwnerCreature, AssassinateInfo);
}

void UWeaponAssassinateComponent::OnAssassinateAttackEvent()
{
	FDamageEvent TempDamageEvent;
	OwnerCreature->CombatComponent->GiveDamage(CurAssassinatedCreature, AssassinationDamage, TempDamageEvent, OwnerCreature->GetController(), OwnerMelee);

	OwnerCreature->RpcComponent->SetSimulatedPhysics(CurAssassinatedCreature->GetMesh(), false);
}

void UWeaponAssassinateComponent::OnAssassinateEndEvent()
{
	IsAssassinating = false;
	if (CurAssassinatedCreature->CombatComponent->GetCurHp() <= 0)
		OwnerCreature->RpcComponent->SetSimulatedPhysics(CurAssassinatedCreature->GetMesh(), true);

	OnAssassinateEnd.Broadcast();
}

void UWeaponAssassinateComponent::OnBeforeAttackEvent()
{
	UWeaponComponent::OnBeforeAttackEvent();

	if (bCanAssassination && OwnerMelee->IsComboAttacking() == false)
	{
		pair<bool, FHitResult> CanAssassinate = CalculateCanAssassinate();
		if (CanAssassinate.first)
		{
			Assassinate(CanAssassinate.second.GetActor());
			OwnerMelee->AttackCancel();
			return;
		}
	}
}
