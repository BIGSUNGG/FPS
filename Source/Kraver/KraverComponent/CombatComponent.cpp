// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kraver/Creature/Soldier/KraverPlayer/KraverPlayer.h"
#include "Kraver/PlayerController/KraverPlayerController.h"
#include "Kraver/HUD/KraverHUD.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	// ...
}


float UCombatComponent::CalculateDamage(float DamageAmount)
{
	return DamageAmount;
}

float UCombatComponent::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Log, TEXT("Damaged"));
	float Damage = CalculateDamage(DamageAmount);
	Server_TakeDamage(DamageAmount,DamageEvent,EventInstigator,DamageCauser);

	return Damage;
}

float UCombatComponent::GiveDamage(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{	
	const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
	Server_GivePointDamage(DamagedActor,DamageAmount,*PointDamageEvent,EventInstigator,DamageCauser);
	return 0.f;
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
	UE_LOG(LogTemp, Log, TEXT("Death"));
	if(CurWeapon != nullptr)
		UnEquipWeapon(CurWeapon);

	OnDeath.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if(OwnerPlayer)
		SetHUDCrosshairs(DeltaTime);
}

void UCombatComponent::Reload()
{
	if(!CurWeapon)
		return;

	CurWeapon->Reload();
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
	if(CurWeapon == nullptr)
		return;

	if (bAttack)
	{
		OnAttackStartDelegate.Broadcast();
	}
	else
	{
		OnAttackEndDelegate.Broadcast();
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
				HUDPackage.CrosshairCenter	= CurWeapon->CrosshairsCenter;
				HUDPackage.CrosshairLeft	= CurWeapon->CrosshairsLeft;
				HUDPackage.CrosshairRight	= CurWeapon->CrosshairsRight;
				HUDPackage.CrosshairTop		= CurWeapon->CrosshairsTop;
				HUDPackage.CrosshairBottom = CurWeapon->CrosshairsBottom;
			}
			else if (OwnerPlayer)
			{
				HUDPackage.CrosshairCenter	= OwnerPlayer->CrosshairsCenter;
				HUDPackage.CrosshairLeft	= OwnerPlayer->CrosshairsLeft;
				HUDPackage.CrosshairRight	= OwnerPlayer->CrosshairsRight;
				HUDPackage.CrosshairTop		= OwnerPlayer->CrosshairsTop;
				HUDPackage.CrosshairBottom	= OwnerPlayer->CrosshairsBottom;
			}
			else
			{	
				HUDPackage.CrosshairCenter	= nullptr;
				HUDPackage.CrosshairLeft	= nullptr;
				HUDPackage.CrosshairRight	= nullptr;
				HUDPackage.CrosshairTop		= nullptr;
				HUDPackage.CrosshairBottom	= nullptr;
			}
			HUD->SetCrosshairsPackage(HUDPackage);
		}
	}
}

void UCombatComponent::Server_CurHp_Implementation(int32 value)
{
	CurHp = value;
}

void UCombatComponent::Server_MaxHp_Implementation(int32 value)
{
	MaxHp = value;
}

void UCombatComponent::Server_UnEquipWeapon_Implementation(AWeapon* Weapon)
{
	if (CurWeapon == Weapon)
	{
		CurWeapon = nullptr;
	}
	OnServerUnEquipWeaponSuccess.Broadcast(Weapon);
}

void UCombatComponent::Server_TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Client_TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void UCombatComponent::Client_TakeDamage_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = CalculateDamage(DamageAmount);
	CurHp -= Damage;
	if (CurHp <= 0)
	{
		CurHp = 0;
		Death(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}

	OnTakeDamaged.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Server_CurHp(CurHp);
}

void UCombatComponent::Server_GivePointDamage_Implementation(AActor* DamagedActor, float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = DamagedActor->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
