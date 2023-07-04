// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Kraver/Character/Creature/Creature.h"
#include "Kraver/Component/Skill/Weapon/WeaponAssassinate/WeaponAssassinateComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"

// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	SetRootComponent(RootComponent);

	OnAttack.AddDynamic(this, &AWeapon::OnAttackEvent);
	OnMakeNewPrimitiveInfo.AddDynamic(this, &AWeapon::OnMakeNewPrimitiveInfoEvent);

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
				Direction * KraverDamageEvent->DamageImpulse * GetWeaponMesh()->GetMass(),
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

int32 AWeapon::MakeAdditivePrimitiveInfo()
{
	TMap<FString, UPrimitiveComponent*> NewWeaponPrimitiveInfo;
	int32 Index = AdditiveWeaponPrimitiveInfo.Add(NewWeaponPrimitiveInfo);

	for (auto& Tuple : WeaponPrimitiveInfo)
	{
		FString CompStr = "Additive " + Tuple.Key;
		FName CompName = FName(*CompStr);
		UPrimitiveComponent* NewPrimitiveComp = NewObject<UPrimitiveComponent>(this, Tuple.Value->GetClass(), CompName);
		NewPrimitiveComp->RegisterComponent();

		if (Cast<USkeletalMeshComponent>(NewPrimitiveComp))
		{
			USkeletalMeshComponent* NewSkeletaComp = dynamic_cast<USkeletalMeshComponent*>(NewPrimitiveComp);
			USkeletalMeshComponent* OriginSkeletaComp = dynamic_cast<USkeletalMeshComponent*>(Tuple.Value);
			
			NewSkeletaComp->SetSkeletalMesh(OriginSkeletaComp->GetSkeletalMeshAsset());

			TArray<UMaterialInterface*> MaterialArray = OriginSkeletaComp->GetMaterials();
			for (int i = 0; i < MaterialArray.Num(); i++)
			{
				NewSkeletaComp->SetMaterial(i, MaterialArray[i]);
			}
		}
		else if (Cast<UNiagaraComponent>(NewPrimitiveComp))
		{
			UNiagaraComponent* NewNiagaraComp = dynamic_cast<UNiagaraComponent*>(NewPrimitiveComp);
			UNiagaraComponent* OriginNiagaraComp = dynamic_cast<UNiagaraComponent*>(Tuple.Value);

			NewNiagaraComp->bAutoActivate = false;
			NewNiagaraComp->SetAsset(OriginNiagaraComp->GetAsset());
		}
		else if (Cast<UStaticMeshComponent>(NewPrimitiveComp))
		{
			UStaticMeshComponent* NewStaticComp = dynamic_cast<UStaticMeshComponent*>(NewPrimitiveComp);
			UStaticMeshComponent* OriginStaticComp = dynamic_cast<UStaticMeshComponent*>(Tuple.Value);

			NewStaticComp->SetStaticMesh(OriginStaticComp->GetStaticMesh());
			TArray<UMaterialInterface*> MaterialArray = OriginStaticComp->GetMaterials();
			for (int i = 0; i < MaterialArray.Num(); i++)
			{
				NewStaticComp->SetMaterial(i, MaterialArray[i]);
			}
		}

		AdditiveWeaponPrimitiveInfo[Index].Add(Tuple.Key, NewPrimitiveComp);
	}

	for (auto& Tuple : WeaponPrimitiveInfo)
	{
		if (Tuple.Value->GetAttachParent())
		{
			for (auto& CompareTuple : WeaponPrimitiveInfo)
			{
				if (Tuple.Value->GetAttachParent() == CompareTuple.Value)
				{
					UPrimitiveComponent* NewPrimitiveComp = AdditiveWeaponPrimitiveInfo[Index][Tuple.Key];
					NewPrimitiveComp->AttachToComponent(AdditiveWeaponPrimitiveInfo[Index][CompareTuple.Key], FAttachmentTransformRules::SnapToTargetIncludingScale, Tuple.Value->GetAttachSocketName());
					NewPrimitiveComp->SetRelativeRotation(Tuple.Value->GetRelativeRotation());
					NewPrimitiveComp->SetRelativeLocation(Tuple.Value->GetRelativeLocation());
					break;
				}
			}
		}
	}

	KR_LOG(Log,TEXT("Add Mesh to AdditiveWeaponMesh[%d]"), Index);

	OnMakeNewPrimitiveInfo.Broadcast(Index);
	return Index;
}

int32 AWeapon::RemoveAdditivePrimitiveInfo(const TMap<FString, UPrimitiveComponent*>& Info)
{
	int32 Index = FindAdditivePrimitiveInfo(Info);

	if (Index == -1)
	{
		KR_LOG(Warning, TEXT("Failed to find AdditiveWeaponMesh"));
		return Index;
	}

	for(auto& Comp : AdditiveWeaponPrimitiveInfo[Index])
		Comp.Value->DestroyComponent();

	AdditiveWeaponPrimitiveInfo.RemoveAt(Index);
	return Index;
}

int32 AWeapon::FindAdditivePrimitiveInfo(const TMap<FString, UPrimitiveComponent*>& Info)
{
	int32 Index = -1;
	for (int i = 0; i < AdditiveWeaponPrimitiveInfo.Num(); i++)
	{
		if (AdditiveWeaponPrimitiveInfo[i]["Root"] == Info["Root"])
		{
			Index = i;
			break;
		}
	}

	return Index;
}

bool AWeapon::Equipped(ACreature* Character)
{
	if (GetCanInteracted() == false)
		return false;

	SetOwner(Character);
	SetOwnerCreature(Character);
	WeaponState = EWeaponState::EQUIPPED;

	GetWeaponMesh()->SetSimulatePhysics(false);
	GetWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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
	ACreature* CreaturePtr = OwnerCreature;
	SetOwnerCreature(nullptr);

	WeaponState = EWeaponState::NONE;
	GetWeaponMesh()->SetSimulatePhysics(true);
	GetWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

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

void AWeapon::OnMakeNewPrimitiveInfoEvent(int Index)
{

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
	GetWeaponMesh()->SetSimulatePhysics(false);
	GetWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::Multicast_UnEquipped_Implementation()
{
	GetWeaponMesh()->SetSimulatePhysics(true);
	GetWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AWeapon::Server_TakeImpulseAtLocation_Implementation(FVector Impulse, FVector ImpactPoint)
{
	Multicast_TakeImpulseAtLocation(Impulse, ImpactPoint);
}

void AWeapon::Multicast_TakeImpulseAtLocation_Implementation(FVector Impulse, FVector ImpactPoint)
{
	GetWeaponMesh()->AddImpulseAtLocation(Impulse,ImpactPoint);
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

void AWeapon::AddWeaponPrimitive(FString Key, UPrimitiveComponent* Value)
{
	WeaponPrimitiveInfo.Add(Key, Value);
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
