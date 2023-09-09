// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponReloadComponent.h"
#include Gun_h
#include Creature_h
#include SoldierAnimInstance_h
#include CreatureMovementComponent_h

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
	OwnerGun->OnAttack.AddDynamic(this, &ThisClass::OnFireEvent);
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
		AnimInstance->OnReload_Insert_Magazine.AddDynamic(this, &UWeaponReloadComponent::OnReload_Insert_MagazineEvent);
	}
}

void UWeaponReloadComponent::OnRemoveOnDelegateEvent(UObject* Object)
{
	UWeaponComponent::OnAddOnDelegateEvent(Object);

	ACreature* Creature = Cast<ACreature>(Object);
	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(Creature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnReload_Insert_Magazine.RemoveDynamic(this, &UWeaponReloadComponent::OnReload_Insert_MagazineEvent);
	}
}

void UWeaponReloadComponent::Server_ReloadStart_Implementation()
{
	Multicast_ReloadStart();
}

void UWeaponReloadComponent::Multicast_ReloadStart_Implementation()
{
	if (!GetOwnerCreature() || !GetOwnerCreature()->IsLocallyControlled())
		ReloadStartEvent();
}

void UWeaponReloadComponent::OnSkillFirstEvent()
{
	ReloadStart();
}

void UWeaponReloadComponent::OnReload_Insert_MagazineEvent()
{
	OwnerGun->RefillAmmo();
}

void UWeaponReloadComponent::OnFireEvent()
{
	if (OwnerGun->GetCurAmmo() <= 0)
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::ReloadStart);
}

void UWeaponReloadComponent::ReloadStart()
{
	if(!CanReload())
		return;

	OwnerGun->OnAttackEndEvent();
	OwnerGun->OnSubAttackEndEvent();

	ReloadStartEvent();
	Server_ReloadStart();
}

void UWeaponReloadComponent::ReloadStartEvent()
{
	if(ReloadMontageFpp)
		OwnerGun->OnPlayFppMontage.Broadcast(ReloadMontageFpp, 1.f);
	if (ReloadMontageTpp)
		OwnerGun->OnPlayTppMontage.Broadcast(ReloadMontageTpp, 1.f);

	if (ReloadMontageWep)
	{
		if (OwnerGun->GetTppWeaponMesh()->GetAnimInstance())
			OwnerGun->GetTppWeaponMesh()->GetAnimInstance()->Montage_Play(ReloadMontageWep);

		if (OwnerGun->GetFppWeaponMesh()->GetAnimInstance())
			OwnerGun->GetFppWeaponMesh()->GetAnimInstance()->Montage_Play(ReloadMontageWep);
	}

	if (ReloadSound)
	{
		UGameplayStatics::PlaySound2D
		(
			this,
			ReloadSound
		);
	}
}

bool UWeaponReloadComponent::IsReloading()
{
	if (!GetOwnerCreature())
		return false;

	if (GetOwnerCreature()->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadMontageTpp))
		return true;

	return false;
}

bool UWeaponReloadComponent::CanReload()
{
	if(!GetOwnerCreature())
		return false;

	if (OwnerGun->GetCurAmmo() == OwnerGun->GetMaxAmmo())
		return false;

	if (OwnerGun->GetTotalAmmo() == 0)
		return false;

	if(IsReloading())
		return false;

	if (!bReloadWhileSprint)
	{
		UCreatureMovementComponent* MovementComp = GetOwnerCreature()->GetComponentByClass<UCreatureMovementComponent>();
		if(MovementComp && MovementComp->GetMovementState() == EMovementState::SPRINT)
			return false;
	}

	return true;
}
