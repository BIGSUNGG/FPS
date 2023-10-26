// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponParryComponent.h"
#include Weapon_h
#include Creature_h

void UWeaponParryComponent::BeginPlay()
{
	UWeaponComponent::BeginPlay();

	if(OwnerWeapon == nullptr)
		return;

	OwnerWeapon->OnSubAttackStart.AddDynamic(this, &ThisClass::OnSubAttackStartEvent);
}

void UWeaponParryComponent::ParryStart()
{
	bCanParry = false;
	IsParrying = true;
	OwnerWeapon->GetWorldTimerManager().SetTimer(ParryDurationTimer, this, &UWeaponParryComponent::ParryEnd, ParryDuration, false, ParryDuration);
	OwnerWeapon->GetWorldTimerManager().SetTimer(ParryDelayTimer, this, &UWeaponParryComponent::ParryDelayEnd, ParryDelay, false, ParryDelay);
	Server_ParryStart();
}
 
void UWeaponParryComponent::ParryEnd()
{
	IsParrying = false;
	Server_ParryEnd();
}

void UWeaponParryComponent::ParryDelayEnd()
{
	bCanParry = true;
}

void UWeaponParryComponent::OnServer_ParrySuccess()
{
	// 패링성공 시 데미지 받기 취소
	OwnerWeapon->GetOwnerCreature()->CombatComponent->OnServer_CancelTakeDamage();
	KR_LOG(Log, TEXT("Parry Success"));
	Server_ParryEnd();
}

void UWeaponParryComponent::OnSubAttackStartEvent()
{
	if(!bCanParry)
		return;

	ParryStart();
}

void UWeaponParryComponent::OnServerBeforeTakeDamageEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	if(OwnerWeapon == nullptr)
		return;

	UKraverDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UKraverDamageType>();

	if (IsParrying && DamageType->bCanParried) // 패링가능한 공격일 경우 
	{
		// 패링성공
		OnServer_ParrySuccess();
	}
}

void UWeaponParryComponent::Server_ParryStart_Implementation()
{
	OwnerWeapon->GetOwnerCreature()->CombatComponent->OnServerBeforeTakeDamage.AddDynamic(this, &UWeaponParryComponent::OnServerBeforeTakeDamageEvent);
	Client_ParryStart();
}

void UWeaponParryComponent::Client_ParryStart_Implementation()
{
	OwnerWeapon->OnPlayFppMontage.Broadcast(ParryMontageFpp, 1.5f);
	OwnerWeapon->OnPlayTppMontage.Broadcast(ParryMontageTpp, 1.5f);
}

void UWeaponParryComponent::Server_ParryEnd_Implementation()
{
	if(OwnerWeapon == nullptr)
		OwnerWeapon->GetOwnerCreature()->CombatComponent->OnServerBeforeTakeDamage.RemoveDynamic(this, &UWeaponParryComponent::OnServerBeforeTakeDamageEvent);
}
