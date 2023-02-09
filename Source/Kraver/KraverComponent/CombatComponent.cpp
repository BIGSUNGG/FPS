// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kraver/Creature/Creature.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerCreature = Cast<ACreature>(GetOwner());
	if(OwnerCreature == nullptr)
		UE_LOG(LogTemp, Fatal, TEXT("Owner Actor is not Creature class"));
	
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, CurWeapon);
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCombatComponent::Reload()
{
	if(!CurWeapon)
		return;

	CurWeapon->Reload();
	SetIsAttacking(false);
}

void UCombatComponent::EquipWeapon(AWeapon* Weapon)
{
	if (!Weapon)
		return;

	CurWeapon = Weapon;
	OwnerCreature->ServerComponent->OwningOtherActor(CurWeapon);
	CurWeapon->Equipped(OwnerCreature);
	
	OnEquipWeaponSuccess.Broadcast(Weapon);
	Server_EquipWeapon(Weapon);
}

void UCombatComponent::Server_EquipWeapon_Implementation(AWeapon* Weapon)
{
	CurWeapon = Weapon;
	OnServerEquipWeaponSuccess.Broadcast(Weapon);
}

void UCombatComponent::UnEquipWeapon(AWeapon* Weapon)
{
	if (CurWeapon == Weapon)
	{
		CurWeapon = nullptr;
	}

	SetIsAttacking(false);
	Weapon->UnEquipped();
	OnUnEquipWeaponSuccess.Broadcast(Weapon);
	Server_UnEquipWeapon(Weapon);
}

void UCombatComponent::SetIsAttacking(bool bAttack)
{
	if(IsAttacking == bAttack)
		return;

	if (bAttack)
	{
		OnAttackStartDelegate.Broadcast();
	}
	else
	{
		OnAttackEndDelegate.Broadcast();
	}

	IsAttacking = bAttack;
}

void UCombatComponent::Server_UnEquipWeapon_Implementation(AWeapon* Weapon)
{
	if (CurWeapon == Weapon)
	{
		CurWeapon = nullptr;
	}
	OnServerUnEquipWeaponSuccess.Broadcast(Weapon);
}