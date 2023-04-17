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
	WeaponMesh->bReplicatePhysicsToAutonomousProxy = true;
	SetRootComponent(WeaponMesh);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME(AWeapon, OwnerCreature);
	DOREPLIFETIME(AWeapon, IsAttacking);
	DOREPLIFETIME(AWeapon, IsSubAttacking);
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
			Weapon->GetOwnerCreature()->RpcComponent->AddImpulseAtLocation
			( 
				WeaponMesh,
				PointDamageEvent->ShotDirection * Weapon->GetAttackImpulse() * WeaponMesh->GetMass(),
				PointDamageEvent->HitInfo.ImpactPoint
			);
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

	if (CurAttackDelay > 0.f)
	{
		CurAttackDelay -= DeltaTime;
		if (CurAttackDelay < 0.f)
		{
			CurAttackDelay = 0.f;
			if (bFirstInputAttack && bAutomaticAttack)
			{
				bFirstInputAttack = false;
				AttackStartEvent();
			}
		}
	}
}

int32 AWeapon::MakeAdditiveWeaponMesh()
{
	USkeletalMeshComponent* MakeMesh = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), TEXT("Additive Mesh"));
	MakeMesh->RegisterComponent();
	MakeMesh->SetSkeletalMesh(WeaponMesh->GetSkeletalMeshAsset());
	TArray<UMaterialInterface*> MaterialArray = WeaponMesh->GetMaterials();
	for (int i = 0; i < MaterialArray.Num(); i++)
	{
		MakeMesh->SetMaterial(i, MaterialArray[i]);
	}

	int32 Index = AdditiveWeaponMesh.Add(MakeMesh);
	KR_LOG(Log,TEXT("Add Mesh to AdditiveWeaponMesh[%d]"),Index);

	return Index;
}

int32 AWeapon::RemoveAdditiveWeaponMesh(USkeletalMeshComponent* Mesh)
{
	int32 Index = -1;
	for (int i = 0; i < AdditiveWeaponMesh.Num(); i++)
	{
		if (AdditiveWeaponMesh[i] == Mesh)
		{
			Index = i;
			break;
		}
	}

	if (Index == -1)
	{
		KR_LOG(Warning, TEXT("Failed to find AdditiveWeaponMesh"));
		return Index;
	}

	AdditiveWeaponMesh[Index]->SetHiddenInGame(true);
	AdditiveWeaponMesh.RemoveAt(Index);
	return Index;
}

int32 AWeapon::FindAdditiveWeaponMesh(USkeletalMeshComponent* Mesh)
{
	int32 Index = -1;
	for (int i = 0; i < AdditiveWeaponMesh.Num(); i++)
	{
		if (AdditiveWeaponMesh[i] == Mesh)
		{
			Index = i;
			break;
		}
	}
	return Index;
}

bool AWeapon::RefillAmmo()
{
	return false;
}

bool AWeapon::Equipped(ACreature* Character)
{
	if (GetCanInteracted() == false)
		return false;

	SetOwnerCreature(Character);
	OwnerCreature->RpcComponent->SetSimulatedPhysics(WeaponMesh, false);
	
	Character->RpcComponent->SetCollisionEnabled(WeaponMesh, ECollisionEnabled::NoCollision);
	Server_Equipped(OwnerCreature);
	return true;
}

void AWeapon::Server_Equipped_Implementation(ACreature* Character)
{
	SetOwnerCreature(Character);
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
	RemoveOnAttackDelegate();

	ACreature* CreaturePtr = OwnerCreature;
	SetOwnerCreature(nullptr);

	if(IsAttacking)
		AttackEndEvent();

	WeaponMesh->SetSimulatePhysics(true);
	CreaturePtr->RpcComponent->SetCollisionEnabled(WeaponMesh, ECollisionEnabled::QueryAndPhysics);
	Server_UnEquipped();
	return true;
}

bool AWeapon::Hold()
{
	AddOnAttackDelegate();
	return true;
}

bool AWeapon::Holster()
{
	RemoveOnAttackDelegate();
	return true;
}

void AWeapon::AddOnAttackDelegate()
{
	if (OwnerCreature == nullptr)
	{
	}

	OwnerCreature->CombatComponent->OnAttackStartDelegate.AddDynamic(this, &AWeapon::AttackStartEvent);
	OwnerCreature->CombatComponent->OnAttackEndDelegate.AddDynamic(this, &AWeapon::AttackEndEvent);

	OwnerCreature->CombatComponent->OnSubAttackStartDelegate.AddDynamic(this, &AWeapon::SubAttackStartEvent);
	OwnerCreature->CombatComponent->OnSubAttackEndDelegate.AddDynamic(this, &AWeapon::SubAttackEndEvent);
}

void AWeapon::RemoveOnAttackDelegate()
{
	OwnerCreature->CombatComponent->OnAttackStartDelegate.RemoveDynamic(this, &AWeapon::AttackStartEvent);
	OwnerCreature->CombatComponent->OnAttackEndDelegate.RemoveDynamic(this, &AWeapon::AttackEndEvent);

	OwnerCreature->CombatComponent->OnSubAttackStartDelegate.RemoveDynamic(this, &AWeapon::SubAttackStartEvent);
	OwnerCreature->CombatComponent->OnSubAttackEndDelegate.RemoveDynamic(this, &AWeapon::SubAttackEndEvent);
}

void AWeapon::Server_UnEquipped_Implementation()
{
	WeaponState = EWeaponState::NONE;

	WeaponMesh->SetSimulatePhysics(true);
}

void AWeapon::AttackStartEvent()
{
	if(IsAttacking == true)
		return;

	if (CurAttackDelay > 0 && bAutomaticAttack)
	{	
		bFirstInputAttack = true;
		return;
	}

	SetIsAttacking(true);
	if(bFirstAttackDelay == false)
		Attack();
	if (bAutomaticAttack)
		GetWorldTimerManager().SetTimer(AutomaticAttackHandle, this, &AWeapon::Attack, AttackDelay, true);
}

void AWeapon::AttackEndEvent()
{
	bFirstInputAttack = false;
	if(IsAttacking == false)
		return;

	SetIsAttacking(false);
	GetWorldTimerManager().ClearTimer(AutomaticAttackHandle);
}

void AWeapon::SubAttackStartEvent()
{
	SetIsSubAttacking(true);
}

void AWeapon::SubAttackEndEvent()
{
	SetIsSubAttacking(false);
}

void AWeapon::Attack()
{
	if(IsAttacking == false)
		return;
	CurAttackDelay = AttackDelay;

	OnAttack.Broadcast();
}

void AWeapon::SetOwnerCreature(ACreature* pointer)
{
	OwnerCreature = pointer;
	Server_SetOwnerCreature(pointer);
}

void AWeapon::Server_SetOwnerCreature_Implementation(ACreature* pointer)
{
	OwnerCreature = pointer;
}

void AWeapon::SetIsAttacking(bool Value)
{
	IsAttacking = Value;
	Server_SetIsAttacking(Value);
}

void AWeapon::Server_SetIsAttacking_Implementation(bool Value)
{
	IsAttacking = Value;
}

void AWeapon::SetIsSubAttacking(bool Value)
{
	IsSubAttacking = Value;
	Server_SetIsSubAttacking(Value);
}

void AWeapon::Server_SetIsSubAttacking_Implementation(bool Value)
{
	IsSubAttacking = Value;
}
