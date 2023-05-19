// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kraver/Creature/Soldier/KraverPlayer/KraverPlayer.h"
#include "Kraver/PlayerController/KraverPlayerController.h"
#include "Kraver/HUD/KraverHUD.h"
#include "Kraver/Weapon/Gun/Gun.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}

FKraverDamageResult UCombatComponent::CalculateDamage(float DamageAmount, FKraverDamageEvent const& DamageEvent)
{
	FKraverDamageResult Result;
	Result.ActualDamage = DamageAmount;

	if(GetIsDead())
		Result.bAlreadyDead = true;

	if (DamageEvent.bCanHeadShot && DamageEvent.HitInfo.BoneName == "Head")
	{
		Result.ActualDamage *= 10.f;
		Result.bCritical = true;
	}

	if(CurHp - Result.ActualDamage <= 0.f)
		Result.bCreatureDead = true;

	return Result;
}

float UCombatComponent::TakeDamage(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = CalculateDamage(DamageAmount, DamageEvent).ActualDamage;
	Server_TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	return Damage;
}

float UCombatComponent::GiveDamage(AActor* DamagedActor, float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{	
	float Damage;
	ACreature* Creature = Cast<ACreature>(DamagedActor);
	if(Creature)
		Damage = Creature->CombatComponent->CalculateDamage(DamageAmount, DamageEvent).ActualDamage;
	else
		Damage = DamageAmount;

	KR_LOG(Log, TEXT("Give %f Damge to %s"), Damage, *DamagedActor->GetName());
	Server_GiveDamage(DamagedActor, DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	return Damage;
}

void UCombatComponent::Client_GiveDamageSuccess_Implementation(AActor* DamagedActor, float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	OnClientGiveDamageSuccess.Broadcast(DamagedActor, DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::CancelTakeDamage()
{
	bCanceledTakeDamage = true;
}

// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerCreature = Cast<ACreature>(GetOwner());
	OwnerPlayer = Cast<AKraverPlayer>(GetOwner());
	if(OwnerCreature == nullptr)
		UE_LOG(LogTemp, Fatal, TEXT("Owner Actor is not Creature class"));

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, CurWeapon); 
	DOREPLIFETIME(UCombatComponent, CurHp);
	DOREPLIFETIME(UCombatComponent, MaxHp);
}


void UCombatComponent::Death(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Server_Death(DamageAmount,DamageEvent,EventInstigator,DamageCauser);
}

bool UCombatComponent::GetCanEquipWeapon()
{
	if (WeaponSlot.Num() < MaxWeaponSlotSize)
		return true;

	return false;
}

bool UCombatComponent::GetIsDead()
{
	if(CurHp <= 0.f)
		return true;

	return false;
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if(OwnerPlayer)
		SetHUDCrosshairs(DeltaTime);
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (OwnerCreature == nullptr || OwnerCreature->Controller == nullptr)
		return;

	Controller = Controller == nullptr ? Cast<AKraverPlayerController>(OwnerCreature->Controller) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<AKraverHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			FCrosshairsPackage HUDPackage;
			if (CurWeapon)
			{
				HUDPackage.CrosshairsCenter = CurWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = CurWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = CurWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = CurWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = CurWeapon->CrosshairsBottom;
			}
			else if (OwnerPlayer)
			{
				HUDPackage.CrosshairsCenter = OwnerPlayer->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = OwnerPlayer->CrosshairsLeft;
				HUDPackage.CrosshairsRight = OwnerPlayer->CrosshairsRight;
				HUDPackage.CrosshairsTop = OwnerPlayer->CrosshairsTop;
				HUDPackage.CrosshairsBottom = OwnerPlayer->CrosshairsBottom;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
			}

			AGun* Gun = Cast<AGun>(CurWeapon);
			if (Gun)
			{
				HUDPackage.CrosshairSpread = Gun->CalculateCurSpread() * 50;
			}
			HUD->SetCrosshairsPackage(HUDPackage);
		}
	}
}

void UCombatComponent::EquipWeapon(AWeapon* Weapon)
{
	if (!Weapon)
		return;

	Server_EquipWeapon(Weapon);
}

void UCombatComponent::UnEquipWeapon(AWeapon* Weapon)
{
	if(!Weapon)
		return;

	HolsterWeapon(CurWeapon);
	Server_UnEquipWeapon(Weapon);
}

bool UCombatComponent::HoldWeapon(int32 WeaponIndex)
{
	if (WeaponIndex >= WeaponSlot.Num())
	{
		return false;
	}

	if (WeaponIndex >= MaxWeaponSlotSize)
	{
		KR_LOG(Error, TEXT("WeaponIndex is bigger than MaxWeaponSlotSize"));
		return false;
	}

	if (WeaponSlot[WeaponIndex] == nullptr)
	{
		KR_LOG(Warning, TEXT("WeaponSlot[%d] is null"), WeaponIndex);
		return false;
	}

	if (WeaponSlot[WeaponIndex] == CurWeapon)
	{
		KR_LOG(Warning, TEXT("WeaponSlot[%d] is CurWeapon"), WeaponIndex);
		return false;
	}

	HoldWeapon(WeaponSlot[WeaponIndex]);
	return true;
}

void UCombatComponent::HoldWeapon(AWeapon* Weapon)
{
	if (Weapon == nullptr)
	{
		KR_LOG(Warning, TEXT("Weapon is nullptr"));
		return;
	}
	
	KR_LOG(Log,TEXT("Hold Weapon %s"),*Weapon->GetName());
	HolsterWeapon(CurWeapon);
	SetCurWeapon(Weapon);
	Weapon->Hold();
	OnClientHoldWeapon.Broadcast(Weapon);

	Server_HoldWeapon(Weapon);
}

bool UCombatComponent::HolsterWeapon(AWeapon* Weapon)
{
	if(CurWeapon == nullptr)
		return false;

	KR_LOG(Log, TEXT("Holster Weapon %s"), *CurWeapon->GetName());
	bool Success = Weapon->Holster();
	if(Success)
	{
		SetCurWeapon(nullptr);
		OnClientHolsterWeapon.Broadcast(Weapon);

		Server_HolsterWeapon(Weapon);
		return true;
	}

	return false;
}

void UCombatComponent::SetIsAttacking(bool bAttack)
{
	if (!CurWeapon)
		return;

	if (bAttack && CurWeapon->GetIsAttacking() == false)
	{
		OnAttackStartDelegate.Broadcast();
	}
	else if(!bAttack && CurWeapon->GetIsAttacking())
	{
		OnAttackEndDelegate.Broadcast();
	}
}

void UCombatComponent::SetIsSubAttacking(bool bAttack)
{
	if(!CurWeapon)
		return;

	if (bAttack && CurWeapon->GetIsSubAttacking() == false)
	{
		OnSubAttackStartDelegate.Broadcast();
	}
	else if (!bAttack && CurWeapon->GetIsSubAttacking())
	{
		OnSubAttackEndDelegate.Broadcast();
	}
}

void UCombatComponent::SetMaxWeaponSlot(int32 size)
{
	MaxWeaponSlotSize = size;
}

void UCombatComponent::SetCurWeapon(AWeapon* Weapon)
{
	CurWeapon = Weapon;
	Server_SetCurWeapon(Weapon);
}

void UCombatComponent::Server_SetCurWeapon_Implementation(AWeapon* Weapon)
{
	CurWeapon = Weapon;
}

void UCombatComponent::Server_SetCurHp_Implementation(int32 value)
{
	CurHp = value;
	Multicast_SetCurHp(value);
}

void UCombatComponent::Multicast_SetCurHp_Implementation(int32 value)
{
	CurHp = value;
}

void UCombatComponent::Server_SetMaxHp_Implementation(int32 value)
{
	MaxHp = value;
}

void UCombatComponent::Server_EquipWeapon_Implementation(AWeapon* Weapon)
{
	if (!Weapon)
		return;

	bool Success = Weapon->Equipped(OwnerCreature);
	if (Success)
	{
		Weapon->SetOwner(OwnerCreature);
		KR_LOG(Log, TEXT("Success to equip weapon %s"), *Weapon->GetName());

		OnServerEquipWeaponSuccess.Broadcast(Weapon);
		Client_EquipWeaponSuccess(Weapon);
	}
	else
		KR_LOG(Warning, TEXT("Failed to equip weapon"));
}

void UCombatComponent::Server_UnEquipWeapon_Implementation(AWeapon* Weapon)
{
	if (!Weapon)
		return;

	bool Success = Weapon->UnEquipped();
	if (Success)
	{
		Weapon->SetOwner(nullptr);
		KR_LOG(Log, TEXT("UnEquipWeapon %s"), *Weapon->GetName());

		OnServerUnEquipWeaponSuccess.Broadcast(Weapon);
		Client_UnEquipWeaponSuccess(Weapon);
	}
	else
		KR_LOG(Warning, TEXT("Failed to unequip weapon"));
}

void UCombatComponent::Client_EquipWeaponSuccess_Implementation(AWeapon* Weapon)
{
	HolsterWeapon(CurWeapon);

	SetCurWeapon(Weapon);
	Weapon->SetOwnerCreature(OwnerCreature);
	WeaponSlot.Add(Weapon);
	if (WeaponSlot.Num() > MaxWeaponSlotSize)
		KR_LOG(Error, TEXT("WeaponSlot size is bigger than MaxWeaponSlot"));

	OnClientEquipWeaponSuccess.Broadcast(Weapon);

	HoldWeapon(Weapon);
}

void UCombatComponent::Client_UnEquipWeaponSuccess_Implementation(AWeapon* Weapon)
{
	HolsterWeapon(Weapon);
	WeaponSlot.Remove(Weapon);

	OnClientUnEquipWeaponSuccess.Broadcast(Weapon);
}

void UCombatComponent::Server_HoldWeapon_Implementation(AWeapon* Weapon)
{
	OnServerHoldWeapon.Broadcast(Weapon);
}

void UCombatComponent::Server_HolsterWeapon_Implementation(AWeapon* Weapon)
{
	OnServerHolsterWeapon.Broadcast(Weapon);
}

void UCombatComponent::Server_TakeDamage_Implementation(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	FKraverDamageResult DamageResult = CalculateDamage(DamageAmount, DamageEvent);

	OnServerBeforeTakeDamage.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	if (bCanceledTakeDamage)
	{
		bCanceledTakeDamage = false;
		return;
	}

	KR_LOG(Log, TEXT("Take %f Damage by %s"), DamageResult.ActualDamage, *DamageCauser->GetName());
	CurHp -= DamageResult.ActualDamage;
	if (CurHp <= 0)
	{
		CurHp = 0;
		Server_SetCurHp(CurHp);
		Death(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	Server_SetCurHp(CurHp);

	Client_TakeDamageSuccess(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);

	ACreature* Creature = Cast<ACreature>(DamageCauser->GetOwner());
	if(Creature)
		Creature->CombatComponent->Client_GiveDamageSuccess(GetOwner(), DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Client_TakeDamageSuccess_Implementation(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	OnClientAfterTakeDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Server_GiveDamage_Implementation(AActor* DamagedActor, float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = DamagedActor->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	FKraverDamageResult DamageResult;
	ACreature* Creature = Cast<ACreature>(DamagedActor);
	if(Creature)
		DamageResult = CalculateDamage(DamageAmount, DamageEvent);

}

void UCombatComponent::Server_Death_Implementation(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	OnServerDeath.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Client_Death(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void UCombatComponent::Client_Death_Implementation(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	KR_LOG(Log, TEXT("Dead By %s"), *DamageCauser->GetName());
	Server_SetCurHp(CurHp);
	OnClientDeath.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
