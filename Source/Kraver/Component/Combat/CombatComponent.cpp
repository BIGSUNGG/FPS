// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Net/UnrealNetwork.h"
#include KraverPlayer_h
#include KraverPlayerController_h
#include KraverGameMode_h
#include KraverHud_h
#include Gun_h

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

	if(IsDead()) // 이미 죽어있다면
		Result.bAlreadyDead = true;

	if(DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent const& PointDamageEvent = static_cast<FPointDamageEvent const&>(DamageEvent);
		if (DamageType->bCanHeadShot && PointDamageEvent.HitInfo.BoneName == "Head") // 헤드샷이가능하고 머리에 맞았을경우
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

	if(CurHp - Result.ActualDamage <= 0.f) // 데미지를 받으면 죽는다면
		Result.bCreatureDead = true;

	return Result;
}

float UCombatComponent::OnServer_TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!IS_SERVER())
	{
		KR_LOG(Error, TEXT("Called on client"));
		return 0.f;
	}

	UCombatComponent* AttackerCobatComp = FindComponentByClassIncludeOwner<UCombatComponent>(DamageCauser);
	if (FTeamInfo::CheckIsTeam(TeamInfo.CurTeam, AttackerCobatComp->TeamInfo.CurTeam)) // 같은 팀일 경우 데미지를 받지 않음
		return 0.f;

	FKraverDamageResult DamageResult = CalculateDamage(DamageAmount, DamageEvent);

	OnServerBeforeTakeDamage.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	if (bCancelNextTakeDamage)
	{
		bCancelNextTakeDamage = false;
		return 0.f;
	}

	KR_LOG(Log, TEXT("Take %d Point Damage by %s"), DamageResult.ActualDamage, *DamageCauser->GetName());

	// 체력 감소
	DecreaseCurHp(DamageResult.ActualDamage);

	// 체력이 없을 경우
	if (CurHp == 0)
	{
		if (DamageResult.bAlreadyDead == false)
			OnServer_Death(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	}

	// 데미지 타입 확인
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent const& PointDamageEvent = static_cast<FPointDamageEvent const&>(DamageEvent);

		Client_TakePointDamage(DamageAmount, PointDamageEvent, EventInstigator, DamageCauser, DamageResult);
		Multicast_TakePointDamage(DamageAmount, PointDamageEvent, EventInstigator, DamageCauser, DamageResult);

		if (AttackerCobatComp)
			AttackerCobatComp->Client_GivePointDamageSuccess(GetOwner(), DamageAmount, PointDamageEvent, EventInstigator, DamageCauser, DamageResult);

		OnServerAfterTakeAnyDamageSuccess.Broadcast(DamageAmount, PointDamageEvent, EventInstigator, DamageCauser, DamageResult);
		OnServerAfterTakePointDamageSuccess.Broadcast(DamageAmount, PointDamageEvent, EventInstigator, DamageCauser, DamageResult);
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		FRadialDamageEvent const& RadialDamageEvent = static_cast<FRadialDamageEvent const&>(DamageEvent);

		Client_TakeRadialDamage(DamageAmount, RadialDamageEvent, EventInstigator, DamageCauser, DamageResult);
		Multicast_TakeRadialDamage(DamageAmount, RadialDamageEvent, EventInstigator, DamageCauser, DamageResult);

		if (AttackerCobatComp)
			AttackerCobatComp->Client_GiveRadialDamageSuccess(GetOwner(), DamageAmount, RadialDamageEvent, EventInstigator, DamageCauser, DamageResult);

		OnServerAfterTakeAnyDamageSuccess.Broadcast(DamageAmount, RadialDamageEvent, EventInstigator, DamageCauser, DamageResult);
		OnServerAfterTakeRadialDamageSuccess.Broadcast(DamageAmount, RadialDamageEvent, EventInstigator, DamageCauser, DamageResult);
	}
	else
	{
		Client_TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
		Multicast_TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);

		if (AttackerCobatComp)
			AttackerCobatComp->Client_GiveDamageSuccess(GetOwner(), DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);

		OnServerAfterTakeAnyDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
		OnServerAfterTakeDamageSuccess.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	}

	OnServer_AutoHealReset();
	return DamageResult.ActualDamage;
}

float UCombatComponent::OnServer_GiveDamage(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!IS_SERVER())
	{
		KR_LOG(Error, TEXT("Called on client"));
		return 0.f;
	}

	if (!DamageEvent.DamageTypeClass)
	{
		KR_LOG(Error, TEXT("DamageTypeClass is nullptr"));
		return 0.f;
	}

	UKraverDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UKraverDamageType>();
	if (!DamageType)
	{
		KR_LOG(Error, TEXT("Damage Type is not UKraverDamageType class"));
		return 0.f;
	}

	if (DamageType->AttackType == EKraverDamageType::UNKWOWN)
		KR_LOG(Error, TEXT("Damage Type is UNKWON"));

	float Damage = 0.f;
	UCombatComponent* DamagedCombatComp = FindComponentByClassIncludeOwner<UCombatComponent>(DamagedActor);
	if (DamagedCombatComp == nullptr || FTeamInfo::CheckIsEnemy(TeamInfo.CurTeam, DamagedCombatComp->GetTeamInfo().CurTeam))
		Damage = DamagedActor->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	KR_LOG(Log, TEXT("Give %f Damge to %s"), Damage, *DamagedActor->GetName());

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

void UCombatComponent::OnServer_CancelTakeDamage()
{
	ERROR_IF_CALLED_ON_CLIENT();
	bCancelNextTakeDamage = true;
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

	if(IS_SERVER())
		OnServer_AutoHealReset();
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, CurWeapon);
	DOREPLIFETIME(UCombatComponent, WeaponSlot);
	DOREPLIFETIME(UCombatComponent, TeamInfo);
	DOREPLIFETIME(UCombatComponent, CurHp);
	DOREPLIFETIME(UCombatComponent, MaxHp);
}

void UCombatComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	// 장착중인 무기를 모두 Destroy함
	if(IS_SERVER())
	{
		for (auto& Weapon : WeaponSlot)
		{
			if (Weapon)
			{
				KR_LOG(Log, TEXT("Destroy weapon %s"), *Weapon->GetName());
				Weapon->Destroy();
			}
		}
	}
}

int8 UCombatComponent::FindWeaponSlotIndex(AWeapon* FindWeapon)
{
	for (int i = 0; i < WeaponSlot.Num(); i++)
	{
		if(FindWeapon == WeaponSlot[i])
			return i;
	}

	return -1; // WeaponSlot에 현재 무기가 없을 경우 -1반환
}

bool UCombatComponent::GetCanEquipWeapon()
{
	for (auto& Value : WeaponSlot)
	{
		if (Value == nullptr) // WeaponSlot에 Nullptr이 있을경우 장착가능
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

	// 자동 치유가 정상적으로 할당되어 있는지
	if (IS_SERVER() && CanAutoHeal() && !GetWorld()->GetTimerManager().IsTimerActive(AutoHealingTimer))
	{
		KR_LOG(Warning, TEXT("AutoHealingTimer is not active"));
		OnServer_AutoHealReset(); 
	}

}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (OwnerPlayer == nullptr || OwnerPlayer->IsLocallyControlled() == false)
		return;

	Controller = Controller == nullptr ? Cast<AKraverPlayerController>(OwnerCreature->Controller) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<AKraverHud>(Controller->GetHUD()) : HUD;
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

	HolsterWeapon();
	Server_UnEquipWeapon(Weapon);
}

bool UCombatComponent::UnholsterWeapon(int32 WeaponIndex)
{
	ERROR_IF_NOT_CALLED_ON_LOCAL_PARAM(false);

	if (WeaponIndex >= WeaponSlot.Num()) // Index가 WeaponSlot크기보다 클경우
	{
		return false;
	}

	if (WeaponIndex >= MaxWeaponSlotSize) // Index가 MaxWeaponSlotSize크기보다 클경우
	{
		KR_LOG(Error, TEXT("WeaponIndex is bigger than MaxWeaponSlotSize"));
		return false;
	}

	if (WeaponSlot[WeaponIndex] == nullptr) // 들려는 무기가 Nullptr일 경우
	{
		KR_LOG(Warning, TEXT("WeaponSlot[%d] is null"), WeaponIndex);
		return false;
	}

	if (WeaponSlot[WeaponIndex] == CurWeapon) // 들려는 무기가 현재들고있는 무기일 경우
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
	
	HolsterWeapon();

	KR_LOG(Log,TEXT("Unholster Weapon %s"),*Weapon->GetName());

	Weapon->OnLocal_Unholster();

	Server_UnholsterWeapon(Weapon);
}

void UCombatComponent::HolsterWeapon()
{
	for (auto& Weapon : WeaponSlot)
	{
		if(!Weapon)
			continue;

		KR_LOG(Log, TEXT("Holster Weapon %s"), *Weapon->GetName());
		bool Success = Weapon->OnLocal_Holster();
		if (Success)
		{
			Server_HolsterWeapon(Weapon);
		}
	}
}

void UCombatComponent::OnLocal_SetIsAttacking(bool bAttack)
{
	ERROR_IF_NOT_CALLED_ON_LOCAL();

	if (bAttack)
		OnLocalAttackStartDelegate.Broadcast();
	else
		OnLocal_AttackEndDelegate.Broadcast();
}

void UCombatComponent::OnLocal_SetIsSubAttacking(bool bAttack)
{
	ERROR_IF_NOT_CALLED_ON_LOCAL();

	if (bAttack)
		OnLocalSubAttackStartDelegate.Broadcast();
	else
		OnLocalSubAttackEndDelegate.Broadcast();
}

bool UCombatComponent::IsDead()
{
	if (CurHp <= 0.f)
		return true;

	return false;
}

bool UCombatComponent::CanAutoHeal()
{
	if (IsDead())
		return false;

	if (CurHp == MaxHp) // 최대 체력이면
		return false;

	return true;
}

void UCombatComponent::Server_EquipWeapon_Implementation(AWeapon* Weapon)
{
	if (!Weapon)
		return;

	OwnerCreature = OwnerCreature ? OwnerCreature : Cast<ACreature>(GetOwner());

	bool Success = Weapon->OnServer_Equipped(OwnerCreature);
	if (Success)
	{
		Weapon->SetOwner(OwnerCreature);
		KR_LOG(Log, TEXT("Success to equip weapon %s"), *Weapon->GetName());

		if (!AddWeapon(Weapon))
			KR_LOG(Error, TEXT("Failed to AddWeapon"));


		OnServerEquipWeaponSuccess.Broadcast(Weapon);

		if(OwnerCreature->IsLocallyControlled())
			OnLocal_EquipWeaponSuccess(Weapon);
		OnEquipWeaponSuccess.Broadcast(Weapon);
	}
	else
		KR_LOG(Warning, TEXT("Failed to equip weapon"));
}

void UCombatComponent::Server_UnEquipWeapon_Implementation(AWeapon* Weapon)
{
	if (!Weapon)
		return;

	bool Success = Weapon->OnServer_UnEquipped();
	if (Success)
	{
		KR_LOG(Log, TEXT("UnEquipWeapon %s"), *Weapon->GetName());

		if (!RemoveWeapon(Weapon))
			KR_LOG(Error, TEXT("Failed to RemoveWeapon"));

		OnServerUnEquipWeaponSuccess.Broadcast(Weapon);

		if (OwnerCreature->IsLocallyControlled())
			OnLocal_UnEquipWeaponSuccess(Weapon);

		OnUnEquipWeaponSuccess.Broadcast(Weapon);
	}
	else
		KR_LOG(Warning, TEXT("Failed to unequip weapon"));
}

void UCombatComponent::OnLocal_EquipWeaponSuccess(AWeapon* Weapon)
{
	if (!OwnerCreature->IsLocallyControlled())
	{
		KR_LOG(Error, TEXT("Func isn't called on client"));
		return;
	}

	if (IsValid(Weapon) == false)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([=](){ OnLocal_EquipWeaponSuccess(Weapon); });
		return;
	}

	Weapon->SetOwner(OwnerCreature);

	if (bUnholsterNextEquip || CurWeaponIndex == FindWeaponSlotIndex(Weapon))
	{
		bUnholsterNextEquip = false;
		UnholsterWeapon(Weapon);
	}
	
}

void UCombatComponent::OnLocal_UnEquipWeaponSuccess(AWeapon* Weapon)
{
	if (!OwnerCreature->IsLocallyControlled())
	{
		KR_LOG(Error, TEXT("Func isn't called on client"));
		return;
	}

	HolsterWeapon();
}

void UCombatComponent::Server_UnholsterWeapon_Implementation(AWeapon* Weapon)
{
	CurWeapon = Weapon;
	OnServerUnholsterWeapon.Broadcast(Weapon);
	OnUnholsterWeapon.Broadcast(Weapon);
}

void UCombatComponent::Server_HolsterWeapon_Implementation(AWeapon* Weapon)
{
	CurWeapon = nullptr;
	OnServerHolsterWeapon.Broadcast(Weapon);
	OnHolsterWeapon.Broadcast(Weapon);
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

void UCombatComponent::OnServer_Death(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	AKraverGameMode* KraverGameMode = GetWorld()->GetAuthGameMode<AKraverGameMode>();
	if(KraverGameMode)
		KraverGameMode->CreatureDeath(OwnerCreature, OwnerCreature->GetController(), DamageCauser, EventInstigator, DamageResult);

	OnServerDeath.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);

	Client_Death(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
	Multicast_Death(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Client_Death_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	KR_LOG(Log, TEXT("Dead By %s"), *DamageCauser->GetName());
	OnClientDeath.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Multicast_Death_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	OnMulticastDeath.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UCombatComponent::Client_SetCurHp_Implementation(int32 Value)
{
	CurHp = Value;
}

void UCombatComponent::OnRep_CurWeaponEvent(AWeapon* PrevWeapon)
{
	if (PrevWeapon)
		OnHolsterWeapon.Broadcast(PrevWeapon);

	if (CurWeapon)
		OnUnholsterWeapon.Broadcast(CurWeapon);

}

void UCombatComponent::OnRep_WeaponSlotEvent(TArray<AWeapon*> PrevWeaponSlot)
{
	if (!OwnerCreature)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([=]() { OnRep_WeaponSlotEvent(PrevWeaponSlot); });
		return;
	}

	for (int i = 0; i < WeaponSlot.Num(); i++)
	{
		if (WeaponSlot[i] && !PrevWeaponSlot[i]) // 장착한 경우
		{
			if (OwnerCreature->IsLocallyControlled())
				OnLocal_EquipWeaponSuccess(WeaponSlot[i]);

			OnEquipWeaponSuccess.Broadcast(WeaponSlot[i]);
		}
		else if (!WeaponSlot[i] && PrevWeaponSlot[i]) // 장착해제한 경우
		{
			if (OwnerCreature->IsLocallyControlled())
				OnLocal_UnEquipWeaponSuccess(PrevWeaponSlot[i]);

			OnUnEquipWeaponSuccess.Broadcast(PrevWeaponSlot[i]);
		}
	}
}

void UCombatComponent::OnServer_AutoHealStart()
{
	ERROR_IF_CALLED_ON_CLIENT();

	GetWorld()->GetTimerManager().ClearTimer(AutoHealingTimer);
	GetWorld()->GetTimerManager().SetTimer(AutoHealingTimer, this, &UCombatComponent::OnServer_AutoHealEvent, AutoHealStartUpdateTime, true, AutoHealStartUpdateTime);

}

void UCombatComponent::OnServer_AutoHealEvent()
{
	ERROR_IF_CALLED_ON_CLIENT();

	if (!CanAutoHeal())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoHealingTimer);
		return;
	}

	IncreaseCurHp(AutoHealHp);
}

void UCombatComponent::OnServer_AutoHealReset()
{
	ERROR_IF_CALLED_ON_CLIENT();

	if (!CanAutoHeal())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoHealingTimer);
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(AutoHealingTimer);
	GetWorld()->GetTimerManager().SetTimer(AutoHealingTimer, this, &UCombatComponent::OnServer_AutoHealStart, AutoHealStartTime, false, AutoHealStartTime);

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

void UCombatComponent::IncreaseCurHp(int Value)
{
	CurHp += Value;

	if (CurHp > MaxHp)
		CurHp = MaxHp;

	Client_SetCurHp(CurHp);
}

void UCombatComponent::DecreaseCurHp(int Value)
{
	CurHp -= Value;

	if (CurHp < 0)
		CurHp = 0;

	Client_SetCurHp(CurHp);
}
