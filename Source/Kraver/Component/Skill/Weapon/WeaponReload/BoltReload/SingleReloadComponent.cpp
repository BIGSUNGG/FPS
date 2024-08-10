// Fill out your copyright notice in the Description page of Project Settings.


#include "SingleReloadComponent.h"
#include KraverPlayer_h
#include Gun_h
#include SoldierAnimInstance_h

void USingleReloadComponent::OnLocal_AddOnDelegateEvent(UObject* Object)
{
	Super::OnLocal_AddOnDelegateEvent(Object);

	ACreature* Creature = Cast<ACreature>(Object);
	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(Creature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnReload_OpenFinish.AddDynamic(this, &ThisClass::OnReload_OpenFinishEvent);
		AnimInstance->OnReload_InsertFinish.AddDynamic(this, &ThisClass::OnReload_InsertFinishEvent);
	}
}

void USingleReloadComponent::OnLocal_RemoveOnDelegateEvent(UObject* Object)
{
	Super::OnLocal_RemoveOnDelegateEvent(Object);

	ACreature* Creature = Cast<ACreature>(Object);
	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(Creature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnReload_OpenFinish.RemoveDynamic(this, &ThisClass::OnReload_OpenFinishEvent);
		AnimInstance->OnReload_InsertFinish.RemoveDynamic(this, &ThisClass::OnReload_InsertFinishEvent);
	}
}

void USingleReloadComponent::OnReload_OpenFinishEvent()
{
	ACreature* Creature = Cast<ACreature>(GetOwnerCreature());
	UAnimInstance* TppAnimInstance = Creature->GetMesh()->GetAnimInstance();

	AKraverPlayer* Player = Cast<AKraverPlayer>(Creature);
	UAnimInstance* FppAnimInstance = Player->GetArmMesh()->GetAnimInstance();

	ReloadInstert();
}

void USingleReloadComponent::OnReload_InsertFinishEvent()
{
	ACreature* Creature = Cast<ACreature>(GetOwnerCreature());
	UAnimInstance* TppAnimInstance = Creature->GetMesh()->GetAnimInstance();

	AKraverPlayer* Player = Cast<AKraverPlayer>(Creature);
	UAnimInstance* FppAnimInstance = Player->GetArmMesh()->GetAnimInstance();

	// 재장전할 탄약이 있다면 장전 반복
	if (OwnerGun->GetCurAmmo() < OwnerGun->GetMaxAmmo() && OwnerGun->GetMaxAmmo() > 0)
		ReloadInstert();
	// 재장전할 탄약이 없다면 장전 종료
	else
		ReloadClose();
}

void USingleReloadComponent::Server_ReloadInstert_Implementation()
{
	Multicast_ReloadInstert();
}

void USingleReloadComponent::Multicast_ReloadInstert_Implementation()
{
	if (!GetOwnerCreature() || !GetOwnerCreature()->IsLocallyControlled())
		ReloadInstertEvent();
}

void USingleReloadComponent::Server_ReloadClose_Implementation()
{
	Multicast_ReloadClose();
}

void USingleReloadComponent::Multicast_ReloadClose_Implementation()
{
	if (!GetOwnerCreature() || !GetOwnerCreature()->IsLocallyControlled())
		ReloadCloseEvent();
}

void USingleReloadComponent::ReloadInstert()
{
	ReloadInstertEvent();
	Server_ReloadInstert();
}

void USingleReloadComponent::ReloadInstertEvent()
{
	if (ReloadInsertMontageFpp)
		OwnerGun->OnPlayFppMontage.Broadcast(ReloadInsertMontageFpp, 1.f);
	if (ReloadInsertMontageTpp)
		OwnerGun->OnPlayTppMontage.Broadcast(ReloadInsertMontageTpp, 1.f);

	if (ReloadInsertMontageWep)
	{
		if (OwnerGun->GetTppWeaponMesh()->GetAnimInstance())
			OwnerGun->GetTppWeaponMesh()->GetAnimInstance()->Montage_Play(ReloadInsertMontageWep);

		if (OwnerGun->GetFppWeaponMesh()->GetAnimInstance())
			OwnerGun->GetFppWeaponMesh()->GetAnimInstance()->Montage_Play(ReloadInsertMontageWep);

	}

	if (ReloadInsertSound)
	{
		UGameplayStatics::PlaySound2D
		(
			this,
			ReloadInsertSound
		);
	}
}

void USingleReloadComponent::ReloadClose()
{
	ReloadCloseEvent();
	Server_ReloadClose();
}

void USingleReloadComponent::ReloadCloseEvent()
{
	if (ReloadCloseMontageFpp)
		OwnerGun->OnPlayFppMontage.Broadcast(ReloadCloseMontageFpp, 1.f);
	if (ReloadCloseMontageTpp)
		OwnerGun->OnPlayTppMontage.Broadcast(ReloadCloseMontageTpp, 1.f);

	if (ReloadCloseMontageWep)
	{
		if (OwnerGun->GetTppWeaponMesh()->GetAnimInstance())
			OwnerGun->GetTppWeaponMesh()->GetAnimInstance()->Montage_Play(ReloadCloseMontageWep);
		else
			KR_LOG(Error, TEXT("AnimInstance is nullptr"));

		if (OwnerGun->GetFppWeaponMesh()->GetAnimInstance())
			OwnerGun->GetFppWeaponMesh()->GetAnimInstance()->Montage_Play(ReloadCloseMontageWep);
		else
			KR_LOG(Error, TEXT("AnimInstance is nullptr"));
	}

	if (ReloadCloseSound)
	{
		UGameplayStatics::PlaySound2D
		(
			this,
			ReloadCloseSound
		);
	}
}

bool USingleReloadComponent::RefillAmmo()
{
	if (OwnerGun->CurAmmo == OwnerGun->MaxAmmo)
		return false;

	++OwnerGun->CurAmmo;
	--OwnerGun->TotalAmmo;

	if (!IS_SERVER())
		Server_RefillAmmo();

	return true;
}

bool USingleReloadComponent::IsReloading()
{
	if(!GetOwnerCreature())
		return false;

	if(Super::IsReloading())
		return true;

	if (GetOwnerCreature()->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadInsertMontageTpp))
		return true;

	if (GetOwnerCreature()->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadCloseMontageTpp))
		return true;

	return false;
}
