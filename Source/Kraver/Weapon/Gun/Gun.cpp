// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Kraver/Creature/Creature.h"

AGun::AGun() : AWeapon()
{
	WeaponType = EWeaponType::GUN;

	FireEffect = CreateDefaultSubobject<UNiagaraComponent>("FireEffect");
	FireEffect->SetupAttachment(WeaponMesh, FireEffectSocketName);
	FireEffect->bAutoActivate = false;
	
	ImpactEffect = CreateDefaultSubobject<UNiagaraComponent>("ImpactEffect");
	ImpactEffect->bAutoActivate = false;
}

void AGun::Tick(float DeltaTime)
{
	AWeapon::Tick(DeltaTime);

	if (OwnerCreature)
	{
		if (CurAttackDelay > 0)
		{
			AddSpread(SpreadPerTime * DeltaTime);
		}
		else
		{
			CurBulletSpread -= SpreadForceBack * DeltaTime;
			if (CurBulletSpread < MinSpread)
				CurBulletSpread = MinSpread;
		}

		if (OwnerCreature->GetMovementComponent()->IsFalling())
			AdditiveSpreadInAir = FMath::FInterpTo(AdditiveSpreadInAir, SpreadInAir, DeltaTime, 10);
		else
			AdditiveSpreadInAir = FMath::FInterpTo(AdditiveSpreadInAir, 0, DeltaTime, 40);

		FVector Velocity = OwnerCreature->GetVelocity();
		Velocity.Z = 0.f;
		float Speed =  Velocity.Size();
		if(Speed > SpreadMaxSpeed)
			Speed = SpreadMaxSpeed;

		AdditiveSpreadPerSpeed = Speed * SpreadPerSpeed;

		float TempRecoilPitch = FMath::FInterpTo(TargetRecoilPitch, 0.f, DeltaTime, 15.f);
		float TempRecoilYaw = FMath::FInterpTo(TargetRecoilYaw, 0.f, DeltaTime, 15.f);

		float AddRecoilPitch = TargetRecoilPitch - TempRecoilPitch;
		float AddRecoilYaw = TargetRecoilYaw - TempRecoilYaw;

		OwnerCreature->AddControllerPitchInput(AddRecoilPitch);
		OwnerCreature->AddControllerYawInput(AddRecoilYaw);

		TargetRecoilPitch -= AddRecoilPitch;
		TargetRecoilYaw -= AddRecoilYaw;
	}
}

void AGun::BeginPlay()
{
	AWeapon::BeginPlay();
	CurBulletSpread = MinSpread;
}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	AWeapon::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGun, MaxAmmo);
	DOREPLIFETIME(AGun, CurAmmo);
	DOREPLIFETIME(AGun, TotalAmmo);
	DOREPLIFETIME(AGun, BulletDistance);
	DOREPLIFETIME(AGun, BulletRadius);
}

void AGun::PostInitializeComponents()
{
	AWeapon::PostInitializeComponents();

}

int32 AGun::MakeAdditiveWeaponMesh()
{
	int32 Index = AWeapon::MakeAdditiveWeaponMesh();
	UNiagaraComponent* MakeFireEffect = NewObject<UNiagaraComponent>(this, UNiagaraComponent::StaticClass(), TEXT("Additive Fire Effect"));
	MakeFireEffect->bAutoActivate = false;
	MakeFireEffect->RegisterComponent();
	MakeFireEffect->SetAsset(FireEffect->GetAsset());
	MakeFireEffect->AttachToComponent(AdditiveWeaponMesh[Index], FAttachmentTransformRules::SnapToTargetIncludingScale, FireEffectSocketName);
	MakeFireEffect->SetRelativeRotation(FireEffect->GetRelativeRotation());
	AdditiveFireEffect.Push(MakeFireEffect);
	return Index;
}

int32 AGun::RemoveAdditiveWeaponMesh(USkeletalMeshComponent* Mesh)
{
	int32 Index = AWeapon::RemoveAdditiveWeaponMesh(Mesh);
	if (Index == -1)
		return Index;

	AdditiveFireEffect[Index]->DestroyComponent();
	AdditiveFireEffect.RemoveAt(Index);
	return Index;
}

bool AGun::RefillAmmo()
{
	if(CurAmmo == MaxAmmo)
		return false;

	int32 NeedAmmo = MaxAmmo - CurAmmo;
	if (TotalAmmo >= NeedAmmo)
	{
		TotalAmmo -= NeedAmmo;
		CurAmmo = MaxAmmo;
	}
	else
	{
		CurAmmo += TotalAmmo;
		TotalAmmo = 0;
	}
	if (!HasAuthority())
	{
		Server_SetCurAmmo(CurAmmo);
		Server_SetTotalAmmo(TotalAmmo);
	}
	OnAttackEndEvent();
	return true;
}

TArray<FHitResult> AGun::CalculateFireHit(ECollisionChannel BulletChannel, FVector Spread /*= FVector(0,0,0)*/)
{
	FVector EndPoint = OwnerCreature->GetCamera()->GetForwardVector() + Spread;
	EndPoint.Normalize();
	EndPoint *= BulletDistance;
	EndPoint += OwnerCreature->GetCamera()->GetComponentLocation();

	TArray<FHitResult> BulletHitResults;
	FCollisionQueryParams BulletParams(NAME_None, false, OwnerCreature);
	bool bResult = GetWorld()->SweepMultiByChannel(
		BulletHitResults,
		OwnerCreature->GetCamera()->GetComponentLocation(),
		EndPoint,
		FQuat::Identity,
		BulletChannel,
		FCollisionShape::MakeSphere(BulletRadius),
		BulletParams
	);

	return BulletHitResults;
}

void AGun::ShowFireEffect()
{
	if(HasAuthority())
		Multicast_ShowFireEffect();
	else
		Server_ShowFireEffect();
}

void AGun::AddRecoil()
{
	TargetRecoilPitch += FMath::RandRange(MinRecoilPitch,MaxRecoilPitch);
	TargetRecoilYaw += FMath::RandRange(MinRecoilYaw, MaxRecoilYaw);
}

void AGun::FireBullet()
{
	float SpreadX;
	float SpreadY;
	float SpreadZ;
	if (GetbApplySpread())
	{
		float Spread = CalculateCurSpread();
		SpreadX = FMath::RandRange(-Spread, Spread);
		SpreadY = FMath::RandRange(-Spread, Spread);
		SpreadZ = FMath::RandRange(-Spread, Spread);
	}
	else
	{
		SpreadX = 0.f;
		SpreadY = 0.f;
		SpreadZ = 0.f;
	}

	TArray<FHitResult> BulletHitResults = CalculateFireHit(ECC_BULLET, FVector(SpreadX, SpreadY, SpreadZ));

	for (auto& Result : BulletHitResults)
	{
		auto Creature = Cast<ACreature>(Result.GetActor());
		if (IsValid(Result.GetActor()))
		{
			FKraverDamageEvent DamageEvent;
			DamageEvent.bCanHeadShot = true;
			DamageEvent.bCanParried = true;
			DamageEvent.DamageImpulse = AttackImpulse;
			DamageEvent.HitInfo = Result;
			OwnerCreature->CombatComponent->GiveDamage(Result.GetActor(), AttackDamage, DamageEvent, OwnerCreature->GetController(), this);
			if (Result.bBlockingHit)
			{
				FVector ImpaceEffectPos = Result.ImpactPoint - OwnerCreature->GetCamera()->GetForwardVector() * 15.f;
				Server_SpawnImpactEffect(ImpaceEffectPos);
			}
		}
	}
}

void AGun::OnAttackEvent()
{
	if (!IsAttacking)
		return;

	if (OwnerCreature == nullptr)
	{
		KR_LOG(Error, TEXT("OwnerCreature is nullptr"), *GetName());
		return;
	}

	if (CurAmmo > 0)
	{
		if (!bInfinityAmmo)
		{
			--CurAmmo;
			if (!HasAuthority())
				Server_SetCurAmmo(CurAmmo);
		}

		FireBullet();
		AddRecoil();
		ShowFireEffect();
		OnPlayTppMontage.Broadcast(AttackMontageTpp, 1.f);
		OnPlayFppMontage.Broadcast(AttackMontageFpp, 1.f);
	}
	else
	{
		OnAttackEndEvent();
	}

	AWeapon::OnAttackEvent();
}

void AGun::Server_SetTotalAmmo_Implementation(int32 Ammo)
{
	TotalAmmo = Ammo;
}

void AGun::Server_SetCurAmmo_Implementation(int32 Ammo)
{
	CurAmmo = Ammo;
}

void AGun::Server_SetMaxAmmo_Implementation(int32 Ammo)
{
	MaxAmmo = Ammo;
}

void AGun::Server_ShowFireEffect_Implementation()
{
	Multicast_ShowFireEffect();
}

void AGun::Multicast_ShowFireEffect_Implementation()
{
	FireEffect->Activate(true);
	for (auto& TempFireEffect : AdditiveFireEffect)
	{
		if (TempFireEffect)
		{
			TempFireEffect->Activate(true);
		}
	}
}

void AGun::Server_SpawnImpactEffect_Implementation(FVector ImpactPos)
{
	Multicast_SpawnImpactEffect(ImpactPos);
}

void AGun::Multicast_SpawnImpactEffect_Implementation(FVector ImpactPos)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactEffect->GetAsset(), ImpactPos);
}

void AGun::AddSpread(float Spread)
{
	CurBulletSpread += Spread;
	if(CurBulletSpread > MaxSpread)
		CurBulletSpread = MaxSpread;
}
