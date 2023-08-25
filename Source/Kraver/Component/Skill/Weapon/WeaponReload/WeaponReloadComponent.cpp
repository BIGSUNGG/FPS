// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponReloadComponent.h"
#include "Kraver/Actor/Weapon/Gun/Gun.h"
#include "Kraver/Character/Creature/Creature.h"
#include "Kraver/Animation/Creature/Soldier/SoldierAnimInstance.h"
#include "Kraver/Component/Movement/CreatureMovementComponent.h"

void UWeaponReloadComponent::BeginPlay()
{
	UWeaponComponent::BeginPlay();

	OwnerGun = Cast<AGun>(OwnerWeapon);
	if (!OwnerGun)
	{
		KR_LOG(Error, TEXT("Owner is not AGun class"));
		return;
	}

	OwnerGun->OnSkillFirst.AddDynamic(this, &ThisClass::OnSkillFirstEvent);
	OwnerGun->OnSubAttackStart.AddDynamic(this, &ThisClass::OnSubAttackStartEvent);
	OwnerGun->OnFire.AddDynamic(this, &ThisClass::OnFireEvent);
}

void UWeaponReloadComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UWeaponComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UWeaponReloadComponent::OnAddOnDelegateEvent(UObject* Object)
{
	UWeaponComponent::OnAddOnDelegateEvent(Object);

	ACreature* Creature = Cast<ACreature>(Object);
	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(Creature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnReload_Insert_Magazine.AddDynamic(this, &UWeaponReloadComponent::OnRefiilAmmoEvent);
	}
}

void UWeaponReloadComponent::OnRemoveOnDelegateEvent(UObject* Object)
{
	UWeaponComponent::OnAddOnDelegateEvent(Object);

	ACreature* Creature = Cast<ACreature>(Object);
	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(Creature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnReload_Insert_Magazine.RemoveDynamic(this, &UWeaponReloadComponent::OnRefiilAmmoEvent);
	}
}

void UWeaponReloadComponent::OnSkillFirstEvent()
{
	ReloadStart();
}

void UWeaponReloadComponent::OnRefiilAmmoEvent()
{
	OwnerGun->RefillAmmo();
}

void UWeaponReloadComponent::OnSubAttackStartEvent()
{
	if (GetOwnerCreature()->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontageTpp))
	{
		OwnerGun->OnSubAttackEndEvent();
	}
}

void UWeaponReloadComponent::OnFireEvent()
{
	if (OwnerGun->GetCurAmmo() <= 0)
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::ReloadStart);
}

void UWeaponReloadComponent::OnBeforeAttackEvent()
{
	UWeaponComponent::OnBeforeAttackEvent();

	if(GetOwnerCreature() && GetOwnerCreature()->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontageTpp))
		OwnerGun->AttackCancel();
}

void UWeaponReloadComponent::ReloadStart()
{
	if(!GetCanReload())
		return;

	OwnerGun->OnSubAttackEndEvent();
	OwnerGun->OnPlayFppMontage.Broadcast(ReloadMontageFpp, 1.f);
	OwnerGun->OnPlayTppMontage.Broadcast(ReloadMontageTpp, 1.f);

	if (ReloadSound)
	{
		UGameplayStatics::PlaySound2D
		(
			this,
			ReloadSound
		);
	}
}

bool UWeaponReloadComponent::GetCanReload()
{
	if(!GetOwnerCreature())
		return false;

	if (OwnerGun->GetCurAmmo() == OwnerGun->GetMaxAmmo())
		return false;

	if (OwnerGun->GetTotalAmmo() == 0)
		return false;

	if (GetOwnerCreature() && GetOwnerCreature()->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontageTpp))
		return false;

	if (!bReloadWhileSprint)
	{
		UCreatureMovementComponent* MovementComp = GetOwnerCreature()->GetComponentByClass<UCreatureMovementComponent>();
		if(MovementComp && MovementComp->GetMovementState() == EMovementState::SPRINT)
			return false;
	}

	return true;
}
