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
	SetReplicateMovement(false);
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

	SetRootComponent(TppWeaponMesh);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, OwnerCreature);
	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME_CONDITION(AWeapon, bIsAttacking, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AWeapon, bIsSubAttacking, COND_SkipOwner);
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

	// WeaponTppPrimitiveInfo에 컴포넌트를 모두 추가
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

	if (OwnerCreature && OwnerCreature->CombatComponent->IsDead() == false)
	{
		if (OwnerCreature->CombatComponent->GetCurWeapon() != this) // OwnerCreature의 현재 무기가 이 무기가 아닐 경우
		{
			if(IS_SERVER())
				WeaponState = EWeaponState::EQUIPPED_HOLSTER;

			SetWeaponVisibility(false); // 메쉬 숨기기
		}
		else // OwnerCreature의 현재 무기가 이 무기일 경우
		{
			if (IS_SERVER())
				WeaponState = EWeaponState::EQUIPPED_UNHOLSTER;

			SetWeaponVisibility(true); // 메쉬 보이기
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
	if (bCanAttack == false)
		return false;

	if (OwnerCreature && !bAttackWhileSprint)
	{
		if (UCreatureMovementComponent * MovementComp = OwnerCreature->GetComponentByClass<UCreatureMovementComponent>())
		{
			if (MovementComp && OwnerCreature->GetMovementComponent()->IsFalling() == false && MovementComp->GetMovementState() == EMovementState::SPRINT)
				return false;
		}

		if (OwnerCreature->CombatComponent->IsDead())
			return false;
	}

	return true;
}

bool AWeapon::CanSubAttack()
{
	if (bCanSubAttack == false)
		return false;

	if (OwnerCreature && !bSubAttackWhileSprint)
	{
		if (UCreatureMovementComponent* MovementComp = OwnerCreature->GetComponentByClass<UCreatureMovementComponent>())
		{
			if (MovementComp && OwnerCreature->GetMovementComponent()->IsFalling() == false && MovementComp->GetMovementState() == EMovementState::SPRINT)
				return false;
		}

		if (OwnerCreature->CombatComponent->IsDead())
			return false;
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

	// 공격 종료
	OnLocalAttackEndEvent();
	OnLocalSubAttackEndEvent();

	OnLocal_AddOnOwnerDelegate();

	Server_Unholster();
	return true;
}

bool AWeapon::OnLocal_Holster()
{
	ERROR_IF_NOT_CALLED_ON_LOCAL_PARAM(false);

	// 공격 종료
	OnLocalAttackEndEvent();
	OnLocalSubAttackEndEvent();

	OnLocal_RemoveOnOwnerDelegate();

	Server_Holster();
	return true;
}

void AWeapon::OnLocal_AddOnOwnerDelegate()
{
	ERROR_IF_NOT_CALLED_ON_LOCAL();

	if (OwnerCreature == nullptr)
		return;

	OwnerCreature->CombatComponent->OnLocalAttackStartDelegate.AddDynamic(this, &AWeapon::OnLocalAttackStartEvent);
	OwnerCreature->CombatComponent->OnLocal_AttackEndDelegate.AddDynamic(this, &AWeapon::OnLocalAttackEndEvent);

	OwnerCreature->CombatComponent->OnLocalSubAttackStartDelegate.AddDynamic(this, &AWeapon::OnLocalSubAttackStartEvent);
	OwnerCreature->CombatComponent->OnLocalSubAttackEndDelegate.AddDynamic(this, &AWeapon::OnLocalSubAttackEndEvent);

	OnLocalAddOnDelegate.Broadcast(OwnerCreature);
}

void AWeapon::OnLocal_RemoveOnOwnerDelegate()
{
	ERROR_IF_NOT_CALLED_ON_LOCAL();

	if (OwnerCreature == nullptr)
		return;

	OwnerCreature->CombatComponent->OnLocalAttackStartDelegate.RemoveDynamic(this, &AWeapon::OnLocalAttackStartEvent);
	OwnerCreature->CombatComponent->OnLocal_AttackEndDelegate.RemoveDynamic(this, &AWeapon::OnLocalAttackEndEvent);

	OwnerCreature->CombatComponent->OnLocalSubAttackStartDelegate.RemoveDynamic(this, &AWeapon::OnLocalSubAttackStartEvent);
	OwnerCreature->CombatComponent->OnLocalSubAttackEndDelegate.RemoveDynamic(this, &AWeapon::OnLocalSubAttackEndEvent);

	OnLocalRemoveOnDelegate.Broadcast(OwnerCreature);
}

void AWeapon::OnServer_AddOnOwnerDelegate()
{
	ERROR_IF_CALLED_ON_CLIENT();

	if (OwnerCreature == nullptr)
		return;

	OnServerAddOnDelegate.Broadcast(OwnerCreature);
}

void AWeapon::OnServer_RemoveOnOwnerDelegate()
{
	ERROR_IF_CALLED_ON_CLIENT();

	if (OwnerCreature == nullptr)
		return;

	OnServerRemoveOnDelegate.Broadcast(OwnerCreature);
}

void AWeapon::AttackCancel()
{
	bAttackCancel = true;
}

void AWeapon::OnLocalAttackStartEvent()
{		
	if (bIsAttacking == true) // 이미 공격중이면 
		return;

	if(!CanAttack()) // 공격이 불가능하면
		return;

	bIsAttacking = true;
	OnAttackStart.Broadcast();
	Server_OnAttackStartEvent();
}

void AWeapon::OnLocalAttackEndEvent()
{
	if (bIsAttacking == false)
		return;

	bIsAttacking = false;
	OnAttackEnd.Broadcast();

	Server_OnAttackEndEvent();
}

void AWeapon::OnLocalSubAttackStartEvent()
{	
	if(!bCanSubAttack)
		return;

	if(!CanSubAttack())
		return;

	bIsSubAttacking = true;
	OnSubAttackStart.Broadcast();

	Server_OnSubAttackStartEvent();
}

void AWeapon::OnLocalSubAttackEndEvent()
{
	bIsSubAttacking = false;
	OnSubAttackEnd.Broadcast();

	Server_OnSubAttackEndEvent();
}

void AWeapon::Server_Unholster_Implementation()
{
	WeaponState = EWeaponState::EQUIPPED_UNHOLSTER;
	UnholsterEvent();

	OnServer_AddOnOwnerDelegate();
}

void AWeapon::Server_Holster_Implementation()
{
	WeaponState = EWeaponState::EQUIPPED_HOLSTER;
	HolsterEvent();

	OnServer_RemoveOnOwnerDelegate();
}

void AWeapon::Server_OnAttackStartEvent_Implementation()
{
	bIsAttacking = true;
}

void AWeapon::Server_OnAttackEndEvent_Implementation()
{
	bIsAttacking = false;
}

void AWeapon::Server_OnSubAttackStartEvent_Implementation()
{
	bIsSubAttacking = true;
}

void AWeapon::Server_OnSubAttackEndEvent_Implementation()
{
	bIsSubAttacking = false;
}

void AWeapon::Server_Attack_Implementation()
{
	//bAttackCancel = false;
	//OnServerBeforeAttack.Broadcast();
	//
	//if (bAttackCancel) // 공격이 취소되었을 경우
	//{
	//	bAttackCancel = false;
	//	KR_LOG(Log, TEXT("Attack Canceled"));
	//	return;
	//}

	Multicast_Attack();
}

void AWeapon::Multicast_Attack_Implementation()
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

void AWeapon::Attack()
{
	OnAttack.Broadcast();
	Server_Attack();
}

void AWeapon::TryAttack()
{
	KR_LOG(Log, TEXT("Try Attack"));

	bAttackCancel = false;
	OnBeforeAttack.Broadcast();

	if (bAttackCancel) // 공격이 취소되었을 경우
	{
		bAttackCancel = false;
		KR_LOG(Log, TEXT("Attack Canceled"));
		return;
	}
	Attack();
}

void AWeapon::OnRep_WeaponState(EWeaponState PrevWeaponState)
{
	if (PrevWeaponState == EWeaponState::IDLE)
		EquipEvent();

	if (WeaponState == EWeaponState::IDLE)
		UnEquipEvent();
	else if (WeaponState == EWeaponState::EQUIPPED_UNHOLSTER)
		UnholsterEvent();
	else if (WeaponState == EWeaponState::EQUIPPED_HOLSTER)
		HolsterEvent();
}

void AWeapon::MakeFppPrimitiveInfo()
{
	// WeaponTppPrimitiveInfo에 있는 컴포넌트를 WeaponFppPrimitiveInfo에 복사
	for (auto& Tuple : WeaponTppPrimitiveInfo)
	{
		FString CompStr = "Additive " + Tuple.Key;
		FName CompName = FName(*CompStr);
		UPrimitiveComponent* NewPrimitiveComp = NewObject<UPrimitiveComponent>(this, Tuple.Value->GetClass(), CompName);
		NewPrimitiveComp->RegisterComponent();

		// 컴포넌트 클래스별로 설정
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
	
	// TppComp가 부착된 컴포넌트와 같은 컴포넌트를 FppComp에 붙임
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

	// 1인칭에서만 보이게 설정
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
	// 공격과 보조 공격 중지
	OnLocalAttackEndEvent();
	OnLocalSubAttackEndEvent();

	// Physics 중지
	GetTppWeaponMesh()->SetSimulatePhysics(false);
	GetTppWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 세팅 초기화를 위해 HolsterEvent호출
	HolsterEvent();
}

void AWeapon::UnEquipEvent()
{
	// 세팅 초기화를 위해 HolsterEvent호출
	HolsterEvent();

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled())
	{
		// 공격과 보조 공격 중지
		OnLocalAttackEndEvent();
		OnLocalSubAttackEndEvent();

		// OwnerCreature의 델리게이트에 추가된 함수 제거
		OnLocal_RemoveOnOwnerDelegate();
	}

	// Physics 실행
	GetTppWeaponMesh()->SetSimulatePhysics(true);
	GetTppWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 3인칭 메쉬는 보이게 1인칭 메쉬는 숨기기
	SetWeaponVisibility(true, true, false);
	SetWeaponVisibility(false, false, true);
}

void AWeapon::HolsterEvent()
{
	// 무기 숨기기
	SetWeaponVisibility(false);
}

void AWeapon::UnholsterEvent()
{
	// 무기 드러내기
	SetWeaponVisibility(true);
}