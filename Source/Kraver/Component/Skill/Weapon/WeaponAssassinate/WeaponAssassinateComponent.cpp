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

	AssassinateEvent();
	OnAssassinate.Broadcast(Actor);
	Server_Assassinate(Actor);
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

	CurAssassinatedCreature->OnServer_Assassinated(GetOwnerCreature(), AssassinateInfo);	
	Multicast_Assassinate(Actor);
}

void UWeaponAssassinateComponent::Multicast_Assassinate_Implementation(AActor* Actor)
{
	ACreature* Creature = Cast<ACreature>(Actor);
	if (!Creature)
		return;

	CurAssassinatedCreature = Creature;

	if (!GetOwnerCreature() || !GetOwnerCreature()->IsLocallyControlled())
		AssassinateEvent();
}

void UWeaponAssassinateComponent::Server_OnAssassinateAttackEvent_Implementation()
{
	FDamageEvent AssassinateDamageEvent(AssassinateDamageType);
	GetOwnerCreature()->CombatComponent->OnServer_GiveDamage(CurAssassinatedCreature, AssassinationDamage, AssassinateDamageEvent, GetOwnerCreature()->GetController(), OwnerMelee);
}

void UWeaponAssassinateComponent::Server_OnAssassinateEndEvent_Implementation()
{
	CurAssassinatedCreature->AssassinatedEnd();
}

void UWeaponAssassinateComponent::OnAssassinateAttackEvent()
{
	Server_OnAssassinateAttackEvent();
}

void UWeaponAssassinateComponent::OnAssassinateEndEvent()
{
	IsAssassinating = false;

	OnAssassinateEnd.Broadcast();
	Server_OnAssassinateEndEvent();
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
