// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAssassinateComponent.h"
#include Creature_h
#include SoldierAnimInstance_h
#include Melee_h

void UWeaponAssassinateComponent::BeginPlay()
{
	UWeaponComponent::BeginPlay();

	OwnerMelee = Cast<AMelee>(OwnerWeapon);
	if (OwnerMelee == nullptr)
	{
		KR_LOG(Error, TEXT("Owner is not Melee weapon"));
		return;
	}

	OwnerMelee->OnServerBeforeAttack.AddDynamic(this, &UWeaponAssassinateComponent::OnServerBeforeAttackEvent);
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

void UWeaponAssassinateComponent::OnLocal_AddOnDelegateEvent(UObject* Object)
{
	UWeaponComponent::OnLocal_AddOnDelegateEvent(Object);

	ACreature* Creature = Cast<ACreature>(Object);
	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(Creature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMelee_AssassinateAttack.AddDynamic(this, &UWeaponAssassinateComponent::OnAssassinateAttackEvent);
		AnimInstance->OnMelee_AssassinateEnd.AddDynamic(this, &UWeaponAssassinateComponent::OnAssassinateEndEvent);
	}
}

void UWeaponAssassinateComponent::OnLocal_RemoveOnDelegateEvent(UObject* Object)
{
	UWeaponComponent::OnLocal_RemoveOnDelegateEvent(Object);

	ACreature* Creature = Cast<ACreature>(Object);
	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(Creature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMelee_AssassinateAttack.RemoveDynamic(this, &UWeaponAssassinateComponent::OnAssassinateAttackEvent);
		AnimInstance->OnMelee_AssassinateEnd.RemoveDynamic(this, &UWeaponAssassinateComponent::OnAssassinateEndEvent);
	}
}

void UWeaponAssassinateComponent::OnServer_AddOnDelegateEvent(UObject* Object)
{
	UWeaponComponent::OnServer_AddOnDelegateEvent(Object);

	ACreature* Creature = Cast<ACreature>(Object);
	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(Creature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMelee_AssassinateAttack.AddDynamic(this, &UWeaponAssassinateComponent::OnServer_AssassinateAttackEvent);
		AnimInstance->OnMelee_AssassinateEnd.AddDynamic(this, &UWeaponAssassinateComponent::OnServer_AssassinateEndEvent);
	}
}

void UWeaponAssassinateComponent::OnServer_RemoveOnDelegateEvent(UObject* Object)
{
	UWeaponComponent::OnServer_RemoveOnDelegateEvent(Object);

	ACreature* Creature = Cast<ACreature>(Object);
	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(Creature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnMelee_AssassinateAttack.RemoveDynamic(this, &UWeaponAssassinateComponent::OnServer_AssassinateAttackEvent);
		AnimInstance->OnMelee_AssassinateEnd.RemoveDynamic(this, &UWeaponAssassinateComponent::OnServer_AssassinateEndEvent);
	}
}

void UWeaponAssassinateComponent::OnServer_Assassinate(AActor* Actor)
{
	ERROR_IF_CALLED_ON_CLIENT();

	if (!Actor)
		return;

	ACreature* Creature = Cast<ACreature>(Actor);
	if (!Creature)
		return;

	IsAssassinating = true;
	CurAssassinatedCreature = Creature;

	FAssassinateInfo AssassinateInfo;
	AssassinateInfo.AssassinatedMontagesFpp = AssassinatedMontagesFpp;
	AssassinateInfo.AssassinatedMontagesTpp = AssassinatedMontagesTpp;

	CurAssassinatedCreature->OnServer_Assassinated(GetOwnerCreature(), AssassinateInfo);

	Multicast_Assassinate(Actor);
}

void UWeaponAssassinateComponent::AssassinateEvent()
{
	OwnerMelee->OnPlayTppMontage.Broadcast(AssassinateMontagesTpp, 1.f);
	OwnerMelee->OnPlayFppMontage.Broadcast(AssassinateMontagesFpp, 1.f);
}

std::pair<bool, FHitResult> UWeaponAssassinateComponent::CalculateCanAssassinate()
{
	pair<bool, FHitResult> Result;

	if (GetOwnerCreature() == nullptr)
	{
		KR_LOG(Error, TEXT("Owner is nullptr"));
		return Result;
	}

	// 대상 찾기
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

	// 대상이 뒤돌아있는지
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

void UWeaponAssassinateComponent::Multicast_Assassinate_Implementation(AActor* Actor)
{
	ACreature* Creature = Cast<ACreature>(Actor);
	if (!Creature)
		return;

	CurAssassinatedCreature = Creature;

	if (GetOwnerCreature()->IsLocallyControlled())
	{
		OwnerMelee->OnLocalAttackEndEvent();
		OwnerMelee->OnLocalSubAttackEndEvent();
		OnAssassinate.Broadcast(Actor);
	}
	AssassinateEvent();
}

void UWeaponAssassinateComponent::OnAssassinateAttackEvent()
{
}

void UWeaponAssassinateComponent::OnAssassinateEndEvent()
{
	IsAssassinating = false;
	OnAssassinateEnd.Broadcast();
}

void UWeaponAssassinateComponent::OnServer_AssassinateAttackEvent()
{
	FDamageEvent AssassinateDamageEvent(AssassinateDamageType);
	GetOwnerCreature()->CombatComponent->OnServer_GiveDamage(CurAssassinatedCreature, AssassinationDamage, AssassinateDamageEvent, GetOwnerCreature()->GetController(), OwnerMelee);
}

void UWeaponAssassinateComponent::OnServer_AssassinateEndEvent()
{
	CurAssassinatedCreature->OnServer_AssassinatedEnd();
}

void UWeaponAssassinateComponent::OnServerBeforeAttackEvent()
{
	ERROR_IF_CALLED_ON_CLIENT();

	if (bCanAssassination && OwnerMelee->IsComboAttacking() == false) // 콤보 공격중이 아닐때
	{
		pair<bool, FHitResult> CanAssassinate = CalculateCanAssassinate(); // 암살 대상 찾기
		if (CanAssassinate.first)
		{
			OnServer_Assassinate(CanAssassinate.second.GetActor());
			OwnerMelee->AttackCancel();
			return;
		}
	}
}
