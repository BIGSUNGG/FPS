// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Net/UnrealNetwork.h"
#include Creature_h
#include WeaponAssassinateComponent_h
#include CreatureMovementComponent_h

// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	//bAsyncPhysicsTickEnabled = true;
	SetReplicateMovement(true);
	NetUpdateFrequency = 1000.f;
	MinNetUpdateFrequency = 30.f;
	NetPriority = 2.f;
	GetReplicatedMovement_Mutable().LocationQuantizationLevel = EVectorQuantization::RoundTwoDecimals;
	GetReplicatedMovement_Mutable().VelocityQuantizationLevel = EVectorQuantization::RoundTwoDecimals;
	GetReplicatedMovement_Mutable().RotationQuantizationLevel = ERotatorQuantization::ShortComponents;
	
	TppWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	TppWeaponMesh->SetCollisionProfileName("WeaponMesh");
	TppWeaponMesh->SetAnimInstanceClass(UAnimInstance::StaticClass());
	TppWeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TppWeaponMesh->SetSimulatePhysics(true);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MONTAGE_TppHolster(TEXT("Engine.AnimMontage'/Game/InfimaGames/AnimatedLowPolyWeapons/Art/Characters/Animations/ARs/AM_TP_CH_AR_01_Holster.AM_TP_CH_AR_01_Holster'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MONTAGE_FppHolster(TEXT("Engine.AnimMontage'/Game/InfimaGames/AnimatedLowPolyWeapons/Art/Characters/Animations/ARs/AM_FP_PCH_AR_01_Holster.AM_FP_PCH_AR_01_Holster'"));

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MONTAGE_TppUnholster(TEXT("Engine.AnimMontage'/Game/InfimaGames/AnimatedLowPolyWeapons/Art/Characters/Animations/ARs/AM_TP_CH_AR_01_Unholster.AM_TP_CH_AR_01_Unholster'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MONTAGE_FppUnholster(TEXT("Engine.AnimMontage'/Game/InfimaGames/AnimatedLowPolyWeapons/Art/Characters/Animations/ARs/AM_FP_PCH_AR_01_Unholster.AM_FP_PCH_AR_01_Unholster'"));

	if (MONTAGE_TppHolster.Succeeded())
		HolsterMontageTpp = MONTAGE_TppHolster.Object;
	else
		KR_LOG(Error, TEXT("Failed to find HolsterMontageTpp asset"));

	if (MONTAGE_FppHolster.Succeeded())
		HolsterMontageFpp = MONTAGE_FppHolster.Object;
	else
		KR_LOG(Error, TEXT("Failed to find HolsterMontageFpp asset"));

	if (MONTAGE_TppUnholster.Succeeded())
		UnholsterMontageTpp = MONTAGE_TppUnholster.Object;
	else
		KR_LOG(Error, TEXT("Failed to find UnholsterMontageTpp asset"));

	if (MONTAGE_FppUnholster.Succeeded())
		UnholsterMontageFpp = MONTAGE_FppUnholster.Object;
	else
		KR_LOG(Error, TEXT("Failed to find UnholsterMontageFpp asset"));

	SetRootComponent(TppWeaponMesh);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, OwnerCreature);
	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME_CONDITION(AWeapon, IsAttacking, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AWeapon, IsSubAttacking, COND_SkipOwner);
}

float AWeapon::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ERROR_IF_CALLED_ON_CLIENT_PARAM(0.f);

	Super::TakeDamage(DamageAmount,DamageEvent,EventInstigator,DamageCauser);

	UKraverDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UKraverDamageType>();

	if(TppWeaponMesh->IsSimulatingPhysics())
	{
		if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		{
			FPointDamageEvent const& PointDamageEvent = static_cast<FPointDamageEvent const&>(DamageEvent);

			Server_TakeImpulseAtLocation(
				PointDamageEvent.ShotDirection * DamageType->DamageImpulse * GetTppWeaponMesh()->GetMass(),
				PointDamageEvent.HitInfo.ImpactPoint
			);	
		}
		else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
		{
			FRadialDamageEvent const& RadialDamageEvent = static_cast<FRadialDamageEvent const&>(DamageEvent);

			FVector Direction = RadialDamageEvent.ComponentHits[0].ImpactPoint - RadialDamageEvent.Origin;
			Direction.Normalize();

			Server_TakeImpulseAtLocation(
				Direction * DamageType->DamageImpulse * GetTppWeaponMesh()->GetMass(),
				RadialDamageEvent.ComponentHits[0].ImpactPoint
			);
		}
	}

	return DamageAmount;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	for (auto& Value : GetComponents())
	{
		if (Value == TppWeaponMesh)
		{
			WeaponTppPrimitiveInfo.Add("Root", TppWeaponMesh);
			continue;
		}

		UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Value);
		if (PrimComp && !WeaponTppPrimitiveInfo.Contains(PrimComp->GetName()))
		{
			WeaponTppPrimitiveInfo.Add(PrimComp->GetName(), PrimComp);
		}
	}

	MakeFppPrimitiveInfo();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsAttacking && !CanAttack())
		OnAttackEndEvent();
	if (IsSubAttacking && !CanSubAttack())
		OnSubAttackEndEvent();

	if (CurAttackDelay > 0.f)
	{
		CurAttackDelay -= DeltaTime;
		if (CurAttackDelay <= 0.f)
		{
			CurAttackDelay = 0.f;
			if (bCanFirstInputAttack && bFirstInputAttack == true)
			{
				bFirstInputAttack = false;
				OnAttackStartEvent();
			}
		}
	}

	if (OwnerCreature)
	{
		if (OwnerCreature->CombatComponent->GetCurWeapon() != this)
		{
			if(IS_SERVER())
				WeaponState = EWeaponState::EQUIPPED_HOLSTER;

			SetWeaponVisibility(false);
		}
		else if (OwnerCreature->CombatComponent->GetCurWeapon() == this)
		{
			if (IS_SERVER())
				WeaponState = EWeaponState::EQUIPPED_UNHOLSTER;

			SetWeaponVisibility(true);
		}
	}
}

bool AWeapon::OnServer_Equipped(ACreature* Character)
{
	if (!IS_SERVER())
	{
		KR_LOG(Error, TEXT("Called on client"));
		return false;
	}

	if (CanInteracted() == false)
		return false;


	SetOwner(Character);
	OwnerCreature = Character;
	WeaponState = EWeaponState::EQUIPPED_HOLSTER;

	EquipEvent();

	return true;
}

bool AWeapon::CanAttack()
{
	if (OwnerCreature && !bAttackWhileSprint)
	{
		if (UCreatureMovementComponent * MovementComp = OwnerCreature->GetComponentByClass<UCreatureMovementComponent>())
		{
			if (MovementComp && OwnerCreature->GetMovementComponent()->IsFalling() == false && MovementComp->GetMovementState() == EMovementState::SPRINT)
				return false;
		}
	}

	return true;
}

bool AWeapon::CanSubAttack()
{
	if (OwnerCreature && !bSubAttackWhileSprint)
	{
		if (UCreatureMovementComponent* MovementComp = OwnerCreature->GetComponentByClass<UCreatureMovementComponent>())
		{
			if (MovementComp && OwnerCreature->GetMovementComponent()->IsFalling() == false && MovementComp->GetMovementState() == EMovementState::SPRINT)
				return false;
		}
	}

	return true;
}

bool AWeapon::CanInteracted()
{
	switch (WeaponState)
	{
	case EWeaponState::IDLE:
		return true;
	case EWeaponState::EQUIPPED_HOLSTER:
		return false;
	case EWeaponState::EQUIPPED_UNHOLSTER:
		return false;
	default:
		return false;
	}
}

void AWeapon::SetWeaponVisibility(bool Value, bool bDoTpp /*= true*/, bool bDoFpp /*= true*/)
{
	if(bDoTpp)
	{
		for (auto& Tuple : WeaponTppPrimitiveInfo)
			Tuple.Value->SetVisibility(Value);
	}

	if (bDoFpp)
	{
		for (auto& Tuple : WeaponFppPrimitiveInfo)
			Tuple.Value->SetVisibility(Value);
	}
}

bool AWeapon::OnServer_UnEquipped()
{
	if (!IS_SERVER())
	{
		KR_LOG(Error, TEXT("Called on client"));
		return false;
	}

	ACreature* CreaturePtr = OwnerCreature;
	OwnerCreature = nullptr;
	WeaponState = EWeaponState::IDLE;

	UnEquipEvent();
	return true;
}

bool AWeapon::OnLocal_Unholster()
{
	ERROR_IF_NOT_CALLED_ON_LOCAL_PARAM(false);

	OwnerCreature = Cast<ACreature>(GetOwner());

	OnAttackEndEvent();
	OnSubAttackEndEvent();

	OnLocal_AddOnOwnerDelegate();

	Server_Unholster();
	return true;
}

bool AWeapon::OnLocal_Holster()
{
	ERROR_IF_NOT_CALLED_ON_LOCAL_PARAM(false);

	OnAttackEndEvent();
	OnSubAttackEndEvent();

	OnLocal_RemoveOnOwnerDelegate();

	Server_Holster();
	return true;
}

void AWeapon::OnLocal_AddOnOwnerDelegate()
{
	ERROR_IF_NOT_CALLED_ON_LOCAL();

	if (OwnerCreature == nullptr)
		return;

	OwnerCreature->CombatComponent->OnAttackStartDelegate.AddDynamic(this, &AWeapon::OnAttackStartEvent);
	OwnerCreature->CombatComponent->OnAttackEndDelegate.AddDynamic(this, &AWeapon::OnAttackEndEvent);

	OwnerCreature->CombatComponent->OnSubAttackStartDelegate.AddDynamic(this, &AWeapon::OnSubAttackStartEvent);
	OwnerCreature->CombatComponent->OnSubAttackEndDelegate.AddDynamic(this, &AWeapon::OnSubAttackEndEvent);

	OnAddOnDelegate.Broadcast(OwnerCreature);
}

void AWeapon::OnLocal_RemoveOnOwnerDelegate()
{
	ERROR_IF_NOT_CALLED_ON_LOCAL();

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

	if(!CanAttack())
		return;

	if (CurAttackDelay > 0 && bCanFirstInputAttack)
	{
		bFirstInputAttack = true;
		return;
	}

	IsAttacking = true;

	if (bCanAttack)
	{ 
		if (!bFirstAttackDelay)
			Attack();

		if(bAutomaticAttack)
			GetWorldTimerManager().SetTimer(AutomaticAttackHandle, this, &AWeapon::Attack, AttackDelay, true, AttackDelay);		
	}

	OnAttackStart.Broadcast();
	Server_OnAttackStartEvent();
}

void AWeapon::OnAttackEndEvent()
{
	bFirstInputAttack = false;
	if (IsAttacking == false)
		return;

	IsAttacking = false;
	GetWorldTimerManager().ClearTimer(AutomaticAttackHandle);
	OnAttackEnd.Broadcast();

	Server_OnAttackEndEvent();
}

void AWeapon::OnSubAttackStartEvent()
{	
	if(!bCanSubAttack)
		return;

	if(!CanSubAttack())
		return;

	IsSubAttacking = true;
	OnSubAttackStart.Broadcast();

	Server_OnSubAttackStartEvent();
}

void AWeapon::OnSubAttackEndEvent()
{
	IsSubAttacking = false;
	OnSubAttackEnd.Broadcast();

	Server_OnSubAttackEndEvent();
}

void AWeapon::Server_Unholster_Implementation()
{
	WeaponState = EWeaponState::EQUIPPED_UNHOLSTER;
	UnholsterEvent();
}

void AWeapon::Server_Holster_Implementation()
{
	WeaponState = EWeaponState::EQUIPPED_HOLSTER;
	HolsterEvent();
}

void AWeapon::Server_OnAttackStartEvent_Implementation()
{
	IsAttacking = true;
}

void AWeapon::Server_OnAttackEndEvent_Implementation()
{
	IsAttacking = false;
}

void AWeapon::Server_OnSubAttackStartEvent_Implementation()
{
	IsSubAttacking = true;
}

void AWeapon::Server_OnSubAttackEndEvent_Implementation()
{
	IsSubAttacking = false;
}

void AWeapon::Server_OnAttackEvent_Implementation()
{
	Multicast_OnAttackEvent();
}

void AWeapon::Multicast_OnAttackEvent_Implementation()
{

}

void AWeapon::Server_TakeImpulseAtLocation_Implementation(FVector Impulse, FVector ImpactPoint)
{
	Multicast_TakeImpulseAtLocation(Impulse, ImpactPoint);
}

void AWeapon::Multicast_TakeImpulseAtLocation_Implementation(FVector Impulse, FVector ImpactPoint)
{
	GetTppWeaponMesh()->AddImpulseAtLocation(Impulse,ImpactPoint);
}

void AWeapon::OnAttackEvent()
{
	Server_OnAttackEvent();
}

void AWeapon::Attack()
{
	bAttackCanceled = false;
	CurAttackDelay = AttackDelay;
	
	OnBeforeAttack.Broadcast();

	if (bAttackCanceled)
	{
		bAttackCanceled = false;
		return;
	}
	OnAttackEvent();
	OnAttack.Broadcast();
}

void AWeapon::OnRep_WeaponState(EWeaponState PrevWeaponState)
{
	if (PrevWeaponState == EWeaponState::IDLE)
		UnholsterEvent();

	if (WeaponState == EWeaponState::IDLE)
		UnEquipEvent();
	else if (WeaponState == EWeaponState::EQUIPPED_UNHOLSTER)
		UnholsterEvent();
	else if (WeaponState == EWeaponState::EQUIPPED_HOLSTER)
		HolsterEvent();
}

void AWeapon::MakeFppPrimitiveInfo()
{
	for (auto& Tuple : WeaponTppPrimitiveInfo)
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
			NewSkeletaComp->SetAnimInstanceClass(OriginSkeletaComp->GetAnimClass());

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

		WeaponFppPrimitiveInfo.Add(Tuple.Key, NewPrimitiveComp);
	}

	for (auto& Tuple : WeaponTppPrimitiveInfo)
	{
		if (Tuple.Value->GetAttachParent())
		{
			for (auto& CompareTuple : WeaponTppPrimitiveInfo)
			{
				if (Tuple.Value->GetAttachParent() == CompareTuple.Value)
				{
					UPrimitiveComponent* NewPrimitiveComp = WeaponFppPrimitiveInfo[Tuple.Key];
					NewPrimitiveComp->AttachToComponent(WeaponFppPrimitiveInfo[CompareTuple.Key], FAttachmentTransformRules::SnapToTargetIncludingScale, Tuple.Value->GetAttachSocketName());
					NewPrimitiveComp->SetRelativeRotation(Tuple.Value->GetRelativeRotation());
					NewPrimitiveComp->SetRelativeLocation(Tuple.Value->GetRelativeLocation());
					break;
				}
			}
		}
	}

	for (auto& Tuple : WeaponFppPrimitiveInfo)
	{
		Tuple.Value->SetOnlyOwnerSee(true);
		Tuple.Value->SetOwnerNoSee(false);
		Tuple.Value->SetCastShadow(false);
		Tuple.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	FppWeaponMesh = Cast<USkeletalMeshComponent>(WeaponFppPrimitiveInfo["Root"]);
	return;
}

void AWeapon::EquipEvent()
{
	OnAttackEndEvent();
	OnSubAttackEndEvent();

	GetTppWeaponMesh()->SetSimulatePhysics(false);
	GetTppWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HolsterEvent();
}

void AWeapon::UnEquipEvent()
{
	HolsterEvent();

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled())
	{
		OnAttackEndEvent();
		OnSubAttackEndEvent();
		OnLocal_RemoveOnOwnerDelegate();
	}

	GetTppWeaponMesh()->SetSimulatePhysics(true);
	GetTppWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	SetWeaponVisibility(true, true, false);
	SetWeaponVisibility(false, false, true);
}

void AWeapon::HolsterEvent()
{
	SetWeaponVisibility(false);
}

void AWeapon::UnholsterEvent()
{
	SetWeaponVisibility(true);
}

void AWeapon::AddWeaponPrimitive(FString Key, UPrimitiveComponent* Value)
{
	WeaponTppPrimitiveInfo.Add(Key, Value);
}