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

float UCombatComponent::CalculateDamage(float DamageAmount, FDamageEvent const& DamageEvent)
{
	float Damage = DamageAmount;
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		if(PointDamageEvent->HitInfo.BoneName == "Head")
			Damage *= 10.f;
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
	}
	else
	{
	}
	return Damage;
}

float UCombatComponent::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = CalculateDamage(DamageAmount,DamageEvent);
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		Server_TakePointDamage(DamageAmount, *PointDamageEvent, EventInstigator, DamageCauser);
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
		Server_TakeRadialDamage(DamageAmount, *RadialDamageEvent, EventInstigator, DamageCauser);
	}
	else
	{
		Server_TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	return Damage;
}

float UCombatComponent::GiveDamage(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{	
	float Damage;
	ACreature* Creature = Cast<ACreature>(DamagedActor);
	if(Creature)
		Damage = Creature->CombatComponent->CalculateDamage(DamageAmount, DamageEvent);
	else
		Damage = DamageAmount;

	KR_LOG(Log, TEXT("Give %f Damge to %s"), Damage, *DamagedActor->GetName());
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		Server_GivePointDamage(DamagedActor, DamageAmount, *PointDamageEvent, EventInstigator, DamageCauser);
		OnGivePointDamage.Broadcast(DamagedActor, DamageAmount, *PointDamageEvent, EventInstigator, DamageCauser);
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
		Server_GiveRadialDamage(DamagedActor, DamageAmount, *RadialDamageEvent, EventInstigator, DamageCauser);
		OnGiveRadialDamage.Broadcast(DamagedActor, DamageAmount, *RadialDamageEvent, EventInstigator, DamageCauser);
	}
	else
	{
		Server_GiveDamage(DamagedActor, DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		OnGiveDamage.Broadcast(DamagedActor, DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	return Damage;
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
	DOREPLIFETIME(UCombatComponent,	MaxHp);
}


void UCombatComponent::Death(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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

void UCombatComponent::SetMaxWeaponSlot(int32 size)
{
	MaxWeaponSlotSize = size;
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if(OwnerPlayer)
		SetHUDCrosshairs(DeltaTime);
}

void UCombatComponent::RefillAmmo()
{
	if(!CurWeapon)
		return;

	CurWeapon->RefillAmmo();
}

void UCombatComponent::EquipWeapon(AWeapon* Weapon)
{
	if (!Weapon)
		return;

	HolsterCurWeapon();
	SetCurWeapon(Weapon);
	OwnerCreature->RpcComponent->OwningOtherActor(CurWeapon);
	bool Success = CurWeapon->Equipped(OwnerCreature);
	
	if(Success)
	{ 
		KR_LOG(Log,TEXT("Success to equip weapon %s"), *Weapon->GetName());
		WeaponSlot.Add(Weapon);
		if(WeaponSlot.Num() > MaxWeaponSlotSize)
			KR_LOG(Error,TEXT("WeaponSlot size is bigger than MaxWeaponSlot"));

		OnEquipWeaponSuccess.Broadcast(Weapon);
		HoldWeapon(Weapon);
	}
	else
		KR_LOG(Warning, TEXT("Failed to equip weapon"));
}

void UCombatComponent::UnEquipWeapon(AWeapon* Weapon)
{
	SetIsAttacking(false);
	if (CurWeapon == Weapon)
	{
		HolsterCurWeapon();
		SetCurWeapon(nullptr);
	}

	bool Success = Weapon->UnEquipped();

	if(Success)
	{ 
		KR_LOG(Log, TEXT("UnEquipWeapon %s"), *Weapon->GetName());
		WeaponSlot.Remove(Weapon);
		OnUnEquipWeaponSuccess.Broadcast(Weapon);
	}
	else
		KR_LOG(Warning, TEXT("Failed to unequip weapon"));
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

	HolsterCurWeapon();
	HoldWeapon(WeaponSlot[WeaponIndex]);
	return true;
}

void UCombatComponent::HoldWeapon(AWeapon* Weapon)
{
	KR_LOG(Log,TEXT("Hold Weapon %s"),*Weapon->GetName());
	SetCurWeapon(Weapon);
	SetIsAttacking(false);
	SetIsSubAttacking(false);
	Weapon->Hold();
	OnHoldWeapon.Broadcast(Weapon);
}

bool UCombatComponent::HolsterCurWeapon()
{
	if(CurWeapon == nullptr)
		return false;

	KR_LOG(Log, TEXT("Holster Weapon %s"), *CurWeapon->GetName());
	AWeapon* WeaponPtr = CurWeapon;
	SetIsAttacking(false);
	SetIsSubAttacking(false);
	SetCurWeapon(nullptr);
	WeaponPtr->Holster();
	OnHolsterWeapon.Broadcast(WeaponPtr);
	return true;
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

			FVector2D WalkSpeedRange(0.f, OwnerCreature->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = OwnerCreature->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			if (OwnerCreature->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			float TargetSpread = CrosshairVelocityFactor + CrosshairInAirFactor;
			AGun* Gun = Cast<AGun>(CurWeapon);
			if (Gun)
			{
				TargetSpread += Gun->GetCurSpread() * 50;
			}
			HUDPackage.CrosshairSpread = FMath::FInterpTo(TargetSpread, HUD->GetHUDPackage().CrosshairSpread,DeltaTime, 20.f);
			HUD->SetCrosshairsPackage(HUDPackage);
		}
	}
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

void UCombatComponent::Server_TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = CalculateDamage(DamageAmount, DamageEvent);
	KR_LOG(Log, TEXT("Take %f Damage by %s"), Damage, *DamageCauser->GetName());
	CurHp -= Damage;
	if (CurHp <= 0)
	{
		CurHp = 0;
		Server_SetCurHp(CurHp);
		Death(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	Server_SetCurHp(CurHp);
	Client_TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}


void UCombatComponent::Server_TakePointDamage_Implementation(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = CalculateDamage(DamageAmount, DamageEvent);
	KR_LOG(Log, TEXT("Take %f Damage by %s"), Damage, *DamageCauser->GetName());
	CurHp -= Damage;
	if (CurHp <= 0)
	{
		CurHp = 0;
		Server_SetCurHp(CurHp);
		Death(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	Server_SetCurHp(CurHp);
	Client_TakePointDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void UCombatComponent::Server_TakeRadialDamage_Implementation(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = CalculateDamage(DamageAmount, DamageEvent);
	KR_LOG(Log, TEXT("Take %f Damage by %s"), Damage, *DamageCauser->GetName());
	CurHp -= Damage;
	if (CurHp <= 0)
	{
		CurHp = 0;
		Server_SetCurHp(CurHp);
		Death(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	Server_SetCurHp(CurHp);
	Client_TakeRadialDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void UCombatComponent::Client_TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	OnAfterTakeDamage.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void UCombatComponent::Client_TakePointDamage_Implementation(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	OnAfterTakePointDamage.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void UCombatComponent::Client_TakeRadialDamage_Implementation(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	OnAfterTakeRadialDamge.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void UCombatComponent::Server_GiveDamage_Implementation(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = DamagedActor->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void UCombatComponent::Server_GivePointDamage_Implementation(AActor* DamagedActor, float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = DamagedActor->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void UCombatComponent::Server_GiveRadialDamage_Implementation(AActor* DamagedActor, float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = DamagedActor->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void UCombatComponent::Server_Death_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Client_Death(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void UCombatComponent::Client_Death_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	KR_LOG(Log, TEXT("Dead By %s"), *DamageCauser->GetName());
	Server_SetCurHp(CurHp);
	OnDeath.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
