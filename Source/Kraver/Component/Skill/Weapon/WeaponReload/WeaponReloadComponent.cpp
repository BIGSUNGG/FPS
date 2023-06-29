// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponReloadComponent.h"
#include "Kraver/Actor/Weapon/Gun/Gun.h"
#include "Kraver/Character/Creature/Creature.h"
#include "Kraver/Animation/Creature/Soldier/SoldierAnimInstance.h"

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
	if(!GetCanReload())
		return;

	OwnerGun->OnPlayFppMontage.Broadcast(ReloadMontageFpp, 1.5f);
	OwnerGun->OnPlayTppMontage.Broadcast(ReloadMontageTpp, 1.5f);
}

void UWeaponReloadComponent::OnRefiilAmmoEvent()
{
	OwnerGun->RefillAmmo();
}

void UWeaponReloadComponent::OnBeforeAttackEvent()
{
	UWeaponComponent::OnBeforeAttackEvent();

	if(GetOwnerCreature() && GetOwnerCreature()->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontageTpp))
		OwnerGun->AttackCancel();
}

bool UWeaponReloadComponent::GetCanReload()
{
	if (OwnerGun->GetCurAmmo() == OwnerGun->GetMaxAmmo())
		return false;

	if (OwnerGun->GetTotalAmmo() == 0)
		return false;

	if (GetOwnerCreature() && GetOwnerCreature()->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontageTpp))
		return false;

	return true;
}
