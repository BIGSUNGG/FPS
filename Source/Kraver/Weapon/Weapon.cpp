// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Kraver/Creature/Creature.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"

// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->SetCollisionProfileName(FName("WeaponMesh"));
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->bReplicatePhysicsToAutonomousProxy = false;
	SetRootComponent(WeaponMesh);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME(AWeapon, AdditiveWeaponMesh);
	DOREPLIFETIME(AWeapon, OwnerCreature);
}

float AWeapon::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount,DamageEvent,EventInstigator,DamageCauser);
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		AWeapon* Weapon = Cast<AWeapon>(DamageCauser);
		if(Weapon)
		{ 
			Weapon->GetOwnerCreature()->ServerComponent->AddImpulseAtLocation(WeaponMesh, PointDamageEvent->ShotDirection * 150.f * WeaponMesh->GetMass(), PointDamageEvent->HitInfo.ImpactPoint);
		}
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
	}

	return DamageAmount;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int32 AWeapon::AddAdditiveWeaponMesh(USkeletalMeshComponent* Mesh)
{
	int32 Index = AdditiveWeaponMesh.Add(Mesh);
	return Index;
}

int32 AWeapon::RemoveAdditiveWeaponMesh(USkeletalMeshComponent* Mesh)
{
	int32 Index = 0;
	for (int i = 0; i < AdditiveWeaponMesh.Num(); i++)
	{
		if (AdditiveWeaponMesh[i] == Mesh)
		{
			Index = i;
			break;
		}
	}
	AdditiveWeaponMesh[Index]->SetHiddenInGame(true);
	AdditiveWeaponMesh.RemoveAt(Index);
	return Index;
}

void AWeapon::Server_AddAdditiveWeaponMesh_Implementation(USkeletalMeshComponent* Mesh)
{
	AdditiveWeaponMesh.Add(Mesh);
}

bool AWeapon::Reload()
{
	return false;
}

bool AWeapon::Equipped(ACreature* Character)
{
	if (GetCanInteracted() == false)
		return false;

	OwnerCreature = Character;
	OwnerCreature->ServerComponent->SetSimulatedPhysics(WeaponMesh, false);
	OwnerCreature->CombatComponent->OnAttackStartDelegate.AddDynamic(this, &AWeapon::AttackStartEvent);
	OwnerCreature->CombatComponent->OnAttackEndDelegate.AddDynamic(this, &AWeapon::AttackEndEvent);
	 
	Server_Equipped(OwnerCreature);
	return true;
}

void AWeapon::Server_Equipped_Implementation(ACreature* Character)
{
	WeaponState = EWeaponState::EQUIPPED;
}

bool AWeapon::GetCanInteracted()
{
	switch (WeaponState)
	{
	case EWeaponState::NONE:
		return true;
	case EWeaponState::EQUIPPED:
		return false;
	default:
		return false;
	}
}

bool AWeapon::UnEquipped()
{
	WeaponMesh->SetSimulatePhysics(true);
	OwnerCreature->CombatComponent->OnAttackStartDelegate.RemoveDynamic(this, &AWeapon::AttackStartEvent);
	OwnerCreature->CombatComponent->OnAttackEndDelegate.RemoveDynamic(this, &AWeapon::AttackEndEvent);
	OwnerCreature = nullptr;
	if(IsAttacking)
		AttackEndEvent();

	Server_UnEquipped();
	return true;
}

void AWeapon::Server_UnEquipped_Implementation()
{
	WeaponState = EWeaponState::NONE;

	WeaponMesh->SetSimulatePhysics(true);
	OwnerCreature = nullptr;
}

void AWeapon::AttackStartEvent()
{
	if(IsAttacking == true)
		return;

	UE_LOG(LogTemp, Log, TEXT("AttackStart"));
	IsAttacking = true;
	if(bFirstAttackDelay == false)
		Attack();
	if (bAutomaticAttack)
		GetWorldTimerManager().SetTimer(AutomaticAttackHandle, this, &AWeapon::Attack, AttackDelay, true);
}

void AWeapon::AttackEndEvent()
{
	if(IsAttacking == false)
		return;

	UE_LOG(LogTemp, Log, TEXT("AttackEnd"));
	IsAttacking = false;
	GetWorldTimerManager().ClearTimer(AutomaticAttackHandle);
}

void AWeapon::Attack()
{
	if(IsAttacking == false)
		return;
	UE_LOG(LogTemp, Log, TEXT("Fire"));
}
