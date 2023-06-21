// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponParryComponent.h"
#include "Kraver/Weapon/Weapon.h"
#include "Kraver/Creature/Creature.h"

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

void UWeaponParryComponent::ParrySuccess()
{
	OwnerWeapon->GetOwnerCreature()->CombatComponent->CancelTakeDamage();
	KR_LOG(Log, TEXT("Parry Success"));
	Server_ParryEnd();
}

void UWeaponParryComponent::OnSubAttackStartEvent()
{
	if(!bCanParry)
		return;

	ParryStart();
}

void UWeaponParryComponent::OnServerBeforeTakeDamageEvent(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	if(OwnerWeapon == nullptr)
		return;

	if (IsParrying && DamageEvent.bCanParried)
	{
		ParrySuccess();
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
