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

void UWeaponAssassinateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UWeaponComponent::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponAssassinateComponent, CurAssassinatedCreature);
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
	Params.AddIgnoredActor(GetOwnerCreature());

	bool bSuccess = GetWorld()->SweepSingleByChannel(
		Result.second,
		GetOwnerCreature()->GetCamera()->GetComponentLocation(),
		GetOwnerCreature()->GetCamera()->GetComponentLocation() + GetOwnerCreature()->GetCamera()->GetForwardVector() * 30.f,
		FQuat::Identity,
		ECC_ASSASSINATION,
		FCollisionShape::MakeSphere(34.f),
		Params
	);

	if (bSuccess)
	{
		FTransform OwnerTransform = GetOwnerCreature()->GetTransform();
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

	CurAssassinatedCreature = Creature;

	FAssassinateInfo AssassinateInfo;
	AssassinateInfo.AssassinatedMontagesFpp = AssassinatedMontagesFpp;
	AssassinateInfo.AssassinatedMontagesTpp = AssassinatedMontagesTpp;

	Creature->Assassinated(GetOwnerCreature(), AssassinateInfo);
	Multicast_Assassinate(Actor);
}

void UWeaponAssassinateComponent::Multicast_Assassinate_Implementation(AActor* Actor)
{
	ACreature* Creature = Cast<ACreature>(Actor);
	if (!Creature)
		return;

	CurAssassinatedCreature = Creature;
}

void UWeaponAssassinateComponent::Server_OnAssassinateAttackEvent_Implementation()
{
	Multicast_OnAssassinateAttackEvent();
}

void UWeaponAssassinateComponent::Multicast_OnAssassinateAttackEvent_Implementation()
{
	if (CurAssassinatedCreature == nullptr)
	{
		KR_LOG(Error,TEXT("CurAssassinatedCreature is  nullptr"));
		return;
	}

	CurAssassinatedCreature->GetMesh()->SetSimulatePhysics(false);
}

void UWeaponAssassinateComponent::Server_OnAssassinateEndEvent_Implementation()
{
	Multicast_OnAssassinateEndEvent();
}

void UWeaponAssassinateComponent::Multicast_OnAssassinateEndEvent_Implementation()
{
	if (CurAssassinatedCreature->CombatComponent->GetCurHp() <= 0)
		CurAssassinatedCreature->GetMesh()->SetSimulatePhysics(true);
}

void UWeaponAssassinateComponent::OnAssassinateAttackEvent()
{
	FKraverDamageEvent TempDamageEvent;
	GetOwnerCreature()->CombatComponent->GiveDamage(CurAssassinatedCreature, AssassinationDamage, TempDamageEvent, GetOwnerCreature()->GetController(), OwnerMelee);
	CurAssassinatedCreature->GetMesh()->SetSimulatePhysics(false);

	Server_OnAssassinateAttackEvent();
}

void UWeaponAssassinateComponent::OnAssassinateEndEvent()
{
	IsAssassinating = false;
	if (CurAssassinatedCreature->CombatComponent->GetCurHp() <= 0)
		CurAssassinatedCreature->GetMesh()->SetSimulatePhysics(true);

	Server_OnAssassinateEndEvent();
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
