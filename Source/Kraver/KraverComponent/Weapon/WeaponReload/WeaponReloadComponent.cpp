// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponReloadComponent.h"
#include "Kraver/Weapon/Gun/Gun.h"
#include "Kraver/Creature/Creature.h"
#include "Kraver/Anim/Creature/Soldier/SoldierAnimInstance.h"

void UWeaponReloadComponent::BeginPlay()
{
	UWeaponComponent::BeginPlay();

	OwnerGun = Cast<AGun>(OwnerWeapon);
	if (!OwnerGun)
	{
		KR_LOG(Error, TEXT("Owner is not AGun class"));
		return;
	}

	OwnerGun->OnSkillFirst.AddDynamic(this, &UWeaponReloadComponent::OnSkillFirstEvent);
}

void UWeaponReloadComponent::OnAddOnDelegateEvent(UObject* Object)
{
	UWeaponComponent::OnAddOnDelegateEvent(Object);

	ACreature* Creature = Cast<ACreature>(Object);
	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(Creature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnRefillAmmo.AddDynamic(this, &UWeaponReloadComponent::OnRefiilAmmoEvent);
	}
}

void UWeaponReloadComponent::OnRemoveOnDelegateEvent(UObject* Object)
{
	UWeaponComponent::OnAddOnDelegateEvent(Object);

	ACreature* Creature = Cast<ACreature>(Object);
	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(Creature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnRefillAmmo.RemoveDynamic(this, &UWeaponReloadComponent::OnRefiilAmmoEvent);
	}
}

void UWeaponReloadComponent::OnSkillFirstEvent()
{
	if(!GetCanReload())
		return;

	OwnerGun->OnPlayFppMontage.Broadcast(ReloadMontageFpp, 1.5f);
	OwnerGun->OnPlayTppMontage.Broadcast(ReloadMontageTpp, 1.5f);
}

void UWeaponReloadComponent::OnRefiilAmmoEvent()
{
	OwnerGun->RefillAmmo();
}

bool UWeaponReloadComponent::GetCanReload()
{
	if (OwnerGun->GetCurAmmo() == OwnerGun->GetMaxAmmo())
		return false;

	if (OwnerGun->GetTotalAmmo() == 0)
		return false;

	return true;
}
