// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Kraver/Creature/Creature.h"
#include "Kraver/KraverComponent/Weapon/WeaponAssassinate/WeaponAssassinateComponent.h"
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

	OnAttack.AddDynamic(this, &AWeapon::OnAttackEvent);
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
	if (DamageEvent.IsOfType(FKraverDamageEvent::ClassID))
	{
		const FKraverDamageEvent* KraverDamageEvent = static_cast<const FKraverDamageEvent*>(&DamageEvent);
		AWeapon* Weapon = Cast<AWeapon>(DamageCauser);
		if(Weapon)
		{
			FVector Direction = KraverDamageEvent->GetHitDirection();
			Server_TakeImpulseAtLocation(
				Direction * KraverDamageEvent->DamageImpulse * WeaponMesh->GetMass(),
				KraverDamageEvent->HitInfo.ImpactPoint
			);
		}
	}
	else
	{
		KR_LOG(Error, TEXT("Use only FKraverDamageEvent"));
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
			if (bFirstInputAttack && bCanFirstInputAttack)
			{
				bFirstInputAttack = false;
				OnAttackStartEvent();
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

	AdditiveWeaponMesh[Index]->DestroyComponent();
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

bool AWeapon::Equipped(ACreature* Character)
{
	if (IS_SERVER() == false)
	{
		KR_LOG(Error, TEXT("Called on client"));
		return false;
	}

	if (GetCanInteracted() == false)
		return false;

	SetOwner(Character);
	SetOwnerCreature(Character);
	WeaponState = EWeaponState::EQUIPPED;

	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Multicast_Equipped(Character);

	return true;
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
	if (IS_SERVER() == false)
	{
		KR_LOG(Error, TEXT("Called on client"));
		return false;
	}


	ACreature* CreaturePtr = OwnerCreature;
	SetOwnerCreature(nullptr);

	WeaponState = EWeaponState::NONE;
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	Client_UnEquipped();
	Multicast_UnEquipped();
	return true;
}

bool AWeapon::Hold()
{
	OnAttackEndEvent();
	OnSubAttackEndEvent();

	AddOnOwnerDelegate();
	return true;
}

bool AWeapon::Holster()
{
	OnAttackEndEvent();
	OnSubAttackEndEvent();

	RemoveOnOwnerDelegate();
	return true;
}

void AWeapon::AddOnOwnerDelegate()
{
	if (OwnerCreature == nullptr)
		return;

	OwnerCreature->CombatComponent->OnAttackStartDelegate.AddDynamic(this, &AWeapon::OnAttackStartEvent);
	OwnerCreature->CombatComponent->OnAttackEndDelegate.AddDynamic(this, &AWeapon::OnAttackEndEvent);

	OwnerCreature->CombatComponent->OnSubAttackStartDelegate.AddDynamic(this, &AWeapon::OnSubAttackStartEvent);
	OwnerCreature->CombatComponent->OnSubAttackEndDelegate.AddDynamic(this, &AWeapon::OnSubAttackEndEvent);

	OnAddOnDelegate.Broadcast(OwnerCreature);
}

void AWeapon::RemoveOnOwnerDelegate()
{
	if (OwnerCreature == nullptr)
		return;

	OwnerCreature->CombatComponent->OnAttackStartDelegate.RemoveDynamic(this, &AWeapon::OnAttackStartEvent);
	OwnerCreature->CombatComponent->OnAttackEndDelegate.RemoveDynamic(this, &AWeapon::OnAttackEndEvent);

	OwnerCreature->CombatComponent->OnSubAttackStartDelegate.RemoveDynamic(this, &AWeapon::OnSubAttackStartEvent);
	OwnerCreature->CombatComponent->OnSubAttackEndDelegate.RemoveDynamic(this, &AWeapon::OnSubAttackEndEvent);

	OnRemoveOnDelegate.Broadcast(OwnerCreature);
}

void AWeapon::AttackCancel()
{
	bAttackCanceled = true;
}

void AWeapon::OnAttackStartEvent()
{
	if (IsAttacking == true)
		return;

	if (CurAttackDelay > 0 && bCanFirstInputAttack)
	{
		bFirstInputAttack = true;
		return;
	}

	SetIsAttacking(true);

	if (bCanAttack)
	{
		if (!bFirstAttackDelay)
			Attack();

		if (bAutomaticAttack)
			GetWorldTimerManager().SetTimer(AutomaticAttackHandle, this, &AWeapon::Attack, AttackDelay, bAutomaticAttack, AttackDelay);
	}

	OnAttackStart.Broadcast();
}

void AWeapon::OnAttackEndEvent()
{
	bFirstInputAttack = false;
	if (IsAttacking == false)
		return;

	SetIsAttacking(false);
	GetWorldTimerManager().ClearTimer(AutomaticAttackHandle);
	OnAttackEnd.Broadcast();
}

void AWeapon::OnSubAttackStartEvent()
{
	SetIsSubAttacking(true);
	OnSubAttackStart.Broadcast();
}

void AWeapon::OnSubAttackEndEvent()
{
	SetIsSubAttacking(false);
	OnSubAttackEnd.Broadcast();
}

void AWeapon::Client_Equipped_Implementation()
{
	OnAttackEndEvent();
	OnSubAttackEndEvent();
}

void AWeapon::Client_UnEquipped_Implementation()
{
	OnAttackEndEvent();
	OnSubAttackEndEvent();

	RemoveOnOwnerDelegate();
}

void AWeapon::Multicast_Equipped_Implementation(ACreature* Character)
{
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::Multicast_UnEquipped_Implementation()
{
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AWeapon::Server_TakeImpulseAtLocation_Implementation(FVector Impulse, FVector ImpactPoint)
{
	Multicast_TakeImpulseAtLocation(Impulse, ImpactPoint);
}

void AWeapon::Multicast_TakeImpulseAtLocation_Implementation(FVector Impulse, FVector ImpactPoint)
{
	WeaponMesh->AddImpulseAtLocation(Impulse,ImpactPoint);
}

void AWeapon::OnAttackEvent()
{

}

void AWeapon::Attack()
{
	CurAttackDelay = AttackDelay;
	
	OnBeforeAttack.Broadcast();

	if (bAttackCanceled)
	{
		bAttackCanceled = false;
		return;
	}
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
