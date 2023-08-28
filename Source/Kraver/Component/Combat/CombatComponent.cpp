// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kraver/Character/Creature/Soldier/KraverPlayer/KraverPlayer.h"
#include "Kraver/GameBase/PlayerController/KraverPlayerController.h"
#include "Kraver/GameBase/GameMode/KraverGameMode.h"
#include "Kraver/GameBase/Ui/HUD/KraverHUD.h"
#include "Kraver/Actor/Weapon/Gun/Gun.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
	WeaponSlot.SetNum(3);
}

FKraverDamageResult UCombatComponent::CalculateDamage(float DamageAmount, FDamageEvent const& DamageEvent)
{
	FKraverDamageResult Result;
	Result.ActualDamage = DamageAmount;

	UKraverDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UKraverDamageType>();

	if(IsDead())
		Result.bAlreadyDead = true;

	if(DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent const& PointDamageEvent = static_cast<FPointDamageEvent const&>(DamageEvent);
		if (DamageType->bCanHeadShot && PointDamageEvent.HitInfo.BoneName == "Head")
		{
			Result.ActualDamage *= 3.f;
			Result.bCritical = true;
		}
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		FRadialDamageEvent const& RadialDamageEvent = static_cast<FRadialDamageEvent const&>(DamageEvent);
		FHitResult HitResult = RadialDamageEvent.ComponentHits[0];
		float Distance = (RadialDamageEvent.Origin - HitResult.ImpactPoint).Size();
		Result.ActualDamage *= RadialDamageEvent.Params.GetDamageScale(Distance);

	}

	if(CurHp - Result.ActualDamage <= 0.f)
		Result.bCreatureDead = true;

	return Result;
}

float UCombatComponent::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = CalculateDamage(DamageAmount, DamageEvent).ActualDamage;
	
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent const& PointDamageEvent = static_cast<FPointDamageEvent const&>(DamageEvent);
		Server_TakePointDamage(DamageAmount, PointDamageEvent, EventInstigator, DamageCauser);
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		FRadialDamageEvent const& RadialDamageEvent = static_cast<FRadialDamageEvent const&>(DamageEvent);
		Server_TakeRadialDamage(DamageAmount, RadialDamageEvent, EventInstigator, DamageCauser);
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
		Damage = Creature->CombatComponent->CalculateDamage(DamageAmount, DamageEvent).ActualDamage;
	else
		Damage = DamageAmount;


	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent const& PointDamageEvent = static_cast<FPointDamageEvent const&>(DamageEvent);
		Server_GivePointDamage(DamagedActor, DamageAmount, PointDamageEvent, EventInstigator, DamageCauser);
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		FRadialDamageEvent const& RadialDamageEvent = static_cast<FRadialDamageEvent const&>(DamageEvent);
		Server_GiveRadialDamage(DamagedActor, DamageAmount, RadialDamageEvent, EventInstigator, DamageCauser);
	}
	else
	{
		Server_GiveDamage(DamagedActor, DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	return Damage;
}

void UCombatComponent::Client_GiveDamageSuccess_Implementation(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	OnClientGiveAnyDamageSuccess.Broadcast(DamagedActor, DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	OnClientGiveDamageSuccess.Broadcast(DamagedActor, DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Client_GivePointDamageSuccess_Implementation(AActor* DamagedActor, float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	OnClientGiveAnyDamageSuccess.Broadcast(DamagedActor, DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	OnClientGivePointDamageSuccess.Broadcast(DamagedActor, DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Client_GiveRadialDamageSuccess_Implementation(AActor* DamagedActor, float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	OnClientGiveAnyDamageSuccess.Broadcast(DamagedActor, DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	OnClientGiveRadialDamageSuccess.Broadcast(DamagedActor, DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
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
	OwnerPlayer = Cast<AKraverPlayer>(OwnerCreature);
	if(OwnerCreature == nullptr)
		UE_LOG(LogTemp, Fatal, TEXT("Owner Actor is not Creature class"));

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, CurWeapon);
	DOREPLIFETIME(UCombatComponent, WeaponSlot);
	DOREPLIFETIME(UCombatComponent, CurHp); 
	DOREPLIFETIME(UCombatComponent, MaxHp);
}

int8 UCombatComponent::GetCurWeaponSlotIndex()
{
	for (int i = 0; i < WeaponSlot.Num(); i++)
	{
		if(CurWeapon == WeaponSlot[i])
			return i;
	}

	return -1;
}

bool UCombatComponent::GetCanEquipWeapon()
{
	for (auto& Value : WeaponSlot)
	{
		if (Value == nullptr)
		{
			return true;
		}
	}
	
	return false;
}

int UCombatComponent::CountWeapon()
{
	int WeaponCount = 0;
	for (auto& Value : WeaponSlot)
	{
		if(Value)
			++WeaponCount;
	}

	return WeaponCount;
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
	if (OwnerPlayer == nullptr || OwnerPlayer->IsLocallyControlled() == false)
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

bool UCombatComponent::UnholsterWeapon(int32 WeaponIndex)
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

	UnholsterWeapon(WeaponSlot[WeaponIndex]);
	return true;
}

void UCombatComponent::UnholsterWeapon(AWeapon* Weapon)
{
	if (Weapon == nullptr)
	{
		KR_LOG(Warning, TEXT("Weapon is nullptr"));
		return;
	}
	
	KR_LOG(Log,TEXT("Unholster Weapon %s"),*Weapon->GetName());
	if (CurWeapon != Weapon)
		HolsterWeapon(CurWeapon);

	SetCurWeapon(Weapon);
	Weapon->Unholster();
	OnClientUnholsterWeapon.Broadcast(Weapon);

	Server_UnholsterWeapon(Weapon);
}

bool UCombatComponent::HolsterWeapon(AWeapon* Weapon)
{
	if(CurWeapon == nullptr)
		return false;

	KR_LOG(Log, TEXT("Holster Weapon %s"), *Weapon->GetName());
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
	if (bAttack)
		OnAttackStartDelegate.Broadcast();
	else
		OnAttackEndDelegate.Broadcast();
}

void UCombatComponent::SetIsSubAttacking(bool bAttack)
{
	if (bAttack)
		OnSubAttackStartDelegate.Broadcast();
	else
		OnSubAttackEndDelegate.Broadcast();
}

bool UCombatComponent::IsDead()
{
	if (CurHp <= 0.f)
		return true;

	return false;
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

		if (!AddWeapon(Weapon))
			KR_LOG(Error, TEXT("Failed to AddWeapon"));

		OnServerEquipWeaponSuccess.Broadcast(Weapon);

		if(OwnerCreature->IsLocallyControlled())
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
		KR_LOG(Log, TEXT("UnEquipWeapon %s"), *Weapon->GetName());

		if (!RemoveWeapon(Weapon))
			KR_LOG(Error, TEXT("Failed to RemoveWeapon"));

		OnServerUnEquipWeaponSuccess.Broadcast(Weapon);

		if (OwnerCreature->IsLocallyControlled())
			Client_UnEquipWeaponSuccess(Weapon);
	}
	else
		KR_LOG(Warning, TEXT("Failed to unequip weapon"));
}

void UCombatComponent::Client_EquipWeaponSuccess(AWeapon* Weapon)
{
	if (!OwnerCreature->IsLocallyControlled())
	{
		KR_LOG(Error, TEXT("Func isn't called on client"));
		return;
	}

	if (IsValid(Weapon) == false)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([=](){ Client_EquipWeaponSuccess(Weapon); });
		return;
	}

	Weapon->SetOwner(OwnerCreature);

	HolsterWeapon(CurWeapon);

	SetCurWeapon(Weapon);
	Weapon->SetOwnerCreature(OwnerCreature);

	OnClientEquipWeaponSuccess.Broadcast(Weapon);

	UnholsterWeapon(Weapon);
}

void UCombatComponent::Client_UnEquipWeaponSuccess(AWeapon* Weapon)
{
	if (!OwnerCreature->IsLocallyControlled())
	{
		KR_LOG(Error, TEXT("Func isn't called on client"));
		return;
	}

	HolsterWeapon(Weapon);

	OnClientUnEquipWeaponSuccess.Broadcast(Weapon);
}

void UCombatComponent::Server_UnholsterWeapon_Implementation(AWeapon* Weapon)
{
	OnServerUnholsterWeapon.Broadcast(Weapon);
}

void UCombatComponent::Server_HolsterWeapon_Implementation(AWeapon* Weapon)
{
	OnServerHolsterWeapon.Broadcast(Weapon);
}

void UCombatComponent::Server_TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	FKraverDamageResult DamageResult = CalculateDamage(DamageAmount, DamageEvent);

	OnServerBeforeTakeDamage.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	if (bCanceledTakeDamage)
	{
		bCanceledTakeDamage = false;
		return;
	}

	KR_LOG(Log, TEXT("Take %d Damage by %s"), DamageResult.ActualDamage, *DamageCauser->GetName());
	CurHp -= DamageResult.ActualDamage;
	if (CurHp <= 0)
	{
		CurHp = 0;
		Server_SetCurHp(CurHp);
		if (DamageResult.bAlreadyDead == false)
			Server_Death(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	}
	Server_SetCurHp(CurHp);

	Client_TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	Multicast_TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);

	UCombatComponent* CauserCobatComp = FindComponentByClassIncludeOwner<UCombatComponent>(DamageCauser);
	if (CauserCobatComp)
		CauserCobatComp->Client_GiveDamageSuccess(GetOwner(), DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);

	OnServerAfterTakeAnyDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	OnServerAfterTakeDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Server_TakePointDamage_Implementation(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	FKraverDamageResult DamageResult = CalculateDamage(DamageAmount, DamageEvent);

	OnServerBeforeTakeDamage.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	if (bCanceledTakeDamage)
	{
		bCanceledTakeDamage = false;
		return;
	}

	KR_LOG(Log, TEXT("Take %d Point Damage by %s"), DamageResult.ActualDamage, *DamageCauser->GetName());
	CurHp -= DamageResult.ActualDamage;
	if (CurHp <= 0)
	{
		CurHp = 0;
		Server_SetCurHp(CurHp);
		if (DamageResult.bAlreadyDead == false)
			Server_Death(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	}
	Server_SetCurHp(CurHp);

	Client_TakePointDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	Multicast_TakePointDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);

	UCombatComponent* CauserCobatComp = FindComponentByClassIncludeOwner<UCombatComponent>(DamageCauser);
	if (CauserCobatComp)
		CauserCobatComp->Client_GivePointDamageSuccess(GetOwner(), DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);

	OnClientAfterTakeAnyDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	OnClientAfterTakePointDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Server_TakeRadialDamage_Implementation(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	FKraverDamageResult DamageResult = CalculateDamage(DamageAmount, DamageEvent);

	OnServerBeforeTakeDamage.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	if (bCanceledTakeDamage)
	{
		bCanceledTakeDamage = false;
		return;
	}

	KR_LOG(Log, TEXT("Take %d Radial Damage by %s"), DamageResult.ActualDamage, *DamageCauser->GetName());
	CurHp -= DamageResult.ActualDamage;
	if (CurHp <= 0)
	{
		CurHp = 0;
		Server_SetCurHp(CurHp);

		if(DamageResult.bAlreadyDead == false)
			Server_Death(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	}
	Server_SetCurHp(CurHp);

	Client_TakeRadialDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	Multicast_TakeRadialDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);

	UCombatComponent* CauserCobatComp = FindComponentByClassIncludeOwner<UCombatComponent>(DamageCauser);
	if (CauserCobatComp)
		CauserCobatComp->Client_GiveRadialDamageSuccess(GetOwner(), DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);

	OnClientAfterTakeAnyDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	OnClientAfterTakeRadialDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Client_TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	OnClientAfterTakeAnyDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	OnClientAfterTakeDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Client_TakePointDamage_Implementation(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	OnClientAfterTakeAnyDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	OnClientAfterTakePointDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Client_TakeRadialDamage_Implementation(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	OnClientAfterTakeAnyDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	OnClientAfterTakeRadialDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Multicast_TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	OnMulticastAfterTakeAnyDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	OnMulticastAfterTakeDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Multicast_TakePointDamage_Implementation(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	OnMulticastAfterTakeAnyDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	OnMulticastAfterTakePointDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Multicast_TakeRadialDamage_Implementation(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	OnMulticastAfterTakeAnyDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	OnMulticastAfterTakeRadialDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Server_GiveDamage_Implementation(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UKraverDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UKraverDamageType>();
	if (!DamageType)
	{
		KR_LOG(Error, TEXT("Damage Type is not UKraverDamageType class"));
		return;
	}

	if(DamageType->AttackType == EKraverDamageType::UNKWOWN)
		KR_LOG(Error, TEXT("Damage Type is UNKWON"));

	float Damage = DamagedActor->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	KR_LOG(Log, TEXT("Give %f Damge to %s"), Damage, *DamagedActor->GetName());

	FKraverDamageResult DamageResult;
	ACreature* Creature = Cast<ACreature>(DamagedActor);
	if(Creature)
		DamageResult = CalculateDamage(DamageAmount, DamageEvent);

}

void UCombatComponent::Server_GivePointDamage_Implementation(AActor* DamagedActor, float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UKraverDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UKraverDamageType>();
	if (!DamageType)
	{
		KR_LOG(Error, TEXT("Damage Type is not UKraverDamageType class"));
		return;
	}

	if (DamageType->AttackType == EKraverDamageType::UNKWOWN)
		KR_LOG(Error, TEXT("Damage Type is UNKWON"));

	float Damage = DamagedActor->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	KR_LOG(Log, TEXT("Give %f Point Damge to %s"), Damage, *DamagedActor->GetName());

	FKraverDamageResult DamageResult;
	ACreature* Creature = Cast<ACreature>(DamagedActor);
	if (Creature)
		DamageResult = CalculateDamage(DamageAmount, DamageEvent);
}

void UCombatComponent::Server_GiveRadialDamage_Implementation(AActor* DamagedActor, float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UKraverDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UKraverDamageType>();
	if (!DamageType)
	{
		KR_LOG(Error, TEXT("Damage Type is not UKraverDamageType class"));
		return;
	}

	if (DamageType->AttackType == EKraverDamageType::UNKWOWN)
		KR_LOG(Error, TEXT("Damage Type is UNKWON"));

	float Damage = DamagedActor->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	KR_LOG(Log, TEXT("Give %f Radial Damge to %s"), Damage, *DamagedActor->GetName());

	FKraverDamageResult DamageResult;
	ACreature* Creature = Cast<ACreature>(DamagedActor);
	if (Creature)
		DamageResult = CalculateDamage(DamageAmount, DamageEvent);
}

void UCombatComponent::Server_Death_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	AKraverGameMode* KraverGameMode = GetWorld()->GetAuthGameMode<AKraverGameMode>();
	if(KraverGameMode)
		KraverGameMode->CreatureEliminated(OwnerCreature, OwnerCreature->GetController(), EventInstigator);

	OnServerDeath.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);

	Client_Death(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	Multicast_Death(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Client_Death_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	KR_LOG(Log, TEXT("Dead By %s"), *DamageCauser->GetName());
	Server_SetCurHp(CurHp);
	OnClientDeath.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Multicast_Death_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	OnMulticastDeath.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::OnRep_CurWeaponEvent(AWeapon* PrevWeapon)
{
	OnRepCurWeapon.Broadcast(PrevWeapon, CurWeapon);
}

void UCombatComponent::OnRep_WeaponSlotEvent(TArray<AWeapon*> PrevWeaponSlot)
{
	if (!OwnerCreature)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([=](){ OnRep_WeaponSlotEvent(PrevWeaponSlot); });
		return;
	}

	if(OwnerCreature->IsLocallyControlled())
	{
		for (int i = 0; i < WeaponSlot.Num(); i++)
		{
			if (WeaponSlot[i] && !PrevWeaponSlot[i]) // 장착한 경우
			{
				Client_EquipWeaponSuccess(WeaponSlot[i]);
			}
			else if (!WeaponSlot[i] && PrevWeaponSlot[i]) // 장착해제한 경우
			{
				Client_UnEquipWeaponSuccess(PrevWeaponSlot[i]);
			}
		}
	}

	OnRepWeaponSlot.Broadcast(PrevWeaponSlot, WeaponSlot);
}

bool UCombatComponent::AddWeapon(AWeapon* Weapon)
{
	for (auto& Value : WeaponSlot)
	{
		if (Value == nullptr)
		{
			Value = Weapon;
			return true;
		}
	}

	return false;
}

bool UCombatComponent::RemoveWeapon(AWeapon* Weapon)
{
	for (auto& Value : WeaponSlot)
	{
		if (Value == Weapon)
		{
			Value = nullptr;
			return true;
		}
	}

	return false;
}
