// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include Creature_h
#include AttachmentSilencerComponent_h
#include WeaponReloadComponent_h
#include WeaponAdsComponent_h
#include CreatureMovementComponent_h

AGun::AGun() : Super()
{
	AdsComponent = CreateDefaultSubobject<UWeaponAdsComponent>("AdsComponent");

	WeaponType = EWeaponType::GUN;
	
	bAttackWhileSprint = false;
	bSubAttackWhileSprint = false;
}

void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpreadUpdate(DeltaTime);
	RecoilUpdate(DeltaTime);

	// ���Է� ó��
	if (CurAttackDelay > 0.f)
	{
		CurAttackDelay -= DeltaTime;
		if (CurAttackDelay <= 0.f)
		{
			CurAttackDelay = 0.f;
			AttackDelayFinish();
		}
	}
}

void AGun::SpreadUpdate(float DeltaTime)
{
	if (OwnerCreature)
	{
		if (CurAttackDelay > 0 || (bIsAttacking && bAutomaticAttack)) // �߻����� �� 
			IncreaseSpread(SpreadPerTime * DeltaTime);
		else // �߻����� �ƴ� ��
			DecreaseSpread(SpreadForceBack * DeltaTime);

		if (OwnerCreature->CreatureMovementComponent->IsFalling()) // ���߿� ���ִ���
			AdditiveSpreadInAir = FMath::FInterpTo(AdditiveSpreadInAir, SpreadInAir, DeltaTime, 10);
		else
			AdditiveSpreadInAir = FMath::FInterpTo(AdditiveSpreadInAir, 0, DeltaTime, 10);

		// �ӵ��� ���� ��������
		FVector Velocity = OwnerCreature->GetVelocity();
		Velocity.Z = 0.f;
		float Speed = Velocity.Size();
		if (Speed > SpreadMaxSpeed)
			Speed = SpreadMaxSpeed;

		AdditiveSpreadPerSpeed = Speed * SpreadPerSpeed;
	}
}

void AGun::RecoilUpdate(float DeltaTime)
{
	if (OwnerCreature && OwnerCreature->IsLocallyControlled())
	{
		float TempRecoilPitch = FMath::FInterpTo(TargetRecoilPitch, 0.f, DeltaTime, 15.f);
		float TempRecoilYaw = FMath::FInterpTo(TargetRecoilYaw, 0.f, DeltaTime, 15.f);

		float AddRecoilPitch = TargetRecoilPitch - TempRecoilPitch;
		float AddRecoilYaw = TargetRecoilYaw - TempRecoilYaw;

		OwnerCreature->AddControllerPitchInput(AddRecoilPitch);
		OwnerCreature->AddControllerYawInput(AddRecoilYaw);

		FRotator TargetRot = FRotator(TargetRecoilYaw, TargetRecoilYaw, TargetRecoilPitch);
		RecoilSwayRot = FMath::RInterpTo(RecoilSwayRot, TargetRot, DeltaTime, 3.f);

		TargetRecoilPitch -= AddRecoilPitch;
		TargetRecoilYaw -= AddRecoilYaw;
	}
}

void AGun::BeginPlay()
{
	Super::BeginPlay();
	CurBulletSpread = MinSpread;

}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AGun, MaxAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGun, CurAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGun, TotalAmmo, COND_OwnerOnly);
}

void AGun::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AGun::OnLocalAttackStartEvent()
{
	if (CurAttackDelay > 0 && bCanFirstInputAttack) // ���� ������ ���̰� ���Է��� ������ �����ϸ�
	{
		bFirstInputAttack = true;
		return;
	}

	if (bIsAttacking == true) // �̹� �������̸� 
		return;

	if (!CanAttack()) // ������ �Ұ����ϸ�
		return;

	Super::OnLocalAttackStartEvent();
	if (bCanAttack)
	{
		if (!bFirstAttackDelay) // ù���� �����̰� ������
			TryAttack();

		if (bAutomaticAttack) // �ڵ������� �����ϸ�
			GetWorldTimerManager().SetTimer(AutomaticAttackHandle, this, &ThisClass::TryAttack, AttackDelay, true, AttackDelay);

		if (bFirstAttackDelay && !bAutomaticAttack) // ù���� �����̵� �ְ� �ڵ������� �Ұ����ϸ�
			GetWorldTimerManager().SetTimer(AutomaticAttackHandle, this, &ThisClass::TryAttack, AttackDelay, false, AttackDelay);

	}
}

void AGun::OnLocalAttackEndEvent()
{
	bFirstInputAttack = false; // ���Է� ���

	if (bIsAttacking == false)
		return;

	Super::OnLocalAttackEndEvent();

	GetWorldTimerManager().ClearTimer(AutomaticAttackHandle); // �ڵ����� ���

}

void AGun::Multicast_Attack_Implementation()
{	
	Super::Multicast_Attack_Implementation();

	if(!OwnerCreature || !OwnerCreature->IsLocallyControlled())
		FireEvent();
}

void AGun::OnServer_ImpactBullet(FVector ImpactPos)
{
	if (!IS_SERVER())
	{
		KR_LOG(Error, TEXT("Called on client"));
		return;
	}

	Multicast_ImpactBullet(ImpactPos);
}

void AGun::OnServer_Fire()
{
	if (OwnerCreature->IsLocallyControlled() == false && bInfinityAmmo == false)
		--CurAmmo;

	OnServer_FireBullet(); // �Ѿ� �߻�
}

void AGun::Multicast_ImpactBullet_Implementation(FVector ImpactPos)
{
	ImpactBulletEvent(ImpactPos);
}

void AGun::IncreaseRecoil()
{
	TargetRecoilPitch += FMath::RandRange(MinRecoilPitch, MaxRecoilPitch);
	TargetRecoilYaw += FMath::RandRange(MinRecoilYaw, MaxRecoilYaw);
}

void AGun::IncreaseSpread(float InValue)
{
	CurBulletSpread += InValue;
	if (CurBulletSpread > MaxSpread)
		CurBulletSpread = MaxSpread;
}

void AGun::DecreaseSpread(float InValue)
{
	CurBulletSpread -= InValue;
	if (CurBulletSpread < MinSpread)
		CurBulletSpread = MinSpread;
}

void AGun::Fire()
{
	if (!bInfinityAmmo)
		--CurAmmo;

	FireEvent(); // �߻� �̺�Ʈ
	IncreaseRecoil(); // �ݵ��߰�

	OnAfterFire.Broadcast();
}

void AGun::OnServer_FireBullet()
{
	OnFireBullet.Broadcast();
}

void AGun::TryAttack()
{
	CurAttackDelay = AttackDelay;

	Super::TryAttack();
}

void AGun::Attack()
{
	if (!bIsAttacking) // �������� �ƴ���
		return;

	if (OwnerCreature == nullptr)
	{
		KR_LOG(Error, TEXT("OwnerCreature is nullptr"), *GetName());
		return;
	}

	if (CurAmmo > 0) // �����ִ� �Ѿ��� �ִ���
	{
		Fire();
		Super::Attack();
	}
	else // �����ִ� �Ѿ��� ������
		OnLocalAttackEndEvent(); // ���� ����

}

void AGun::OnServer_Attack()
{
	ERROR_IF_CALLED_ON_CLIENT();
	Super::OnServer_Attack();

	OnServer_Fire();
}

void AGun::AttackDelayFinish()
{
	if (bCanFirstInputAttack && bFirstInputAttack == true) // ���Է��� �ԷµǾ��ְ� ���Է��� ������ ������ �� 
	{
		bFirstInputAttack = false;
		OnLocalAttackStartEvent(); // ���� ����
	}
}

void AGun::FireEvent()
{
	// ���� ��Ÿ��
	OnPlayTppMontage.Broadcast(AttackMontageTpp, 1.f);
	OnPlayFppMontage.Broadcast(AttackMontageFpp, 1.f);

	if (AttackMontageWep)
	{
		if (GetTppWeaponMesh()->GetAnimInstance())
			GetTppWeaponMesh()->GetAnimInstance()->Montage_Play(AttackMontageWep);
	}

	// �߻� ����Ʈ
	UAttachmentSilencerComponent* Silencer = FindComponentByClass<UAttachmentSilencerComponent>();
	if ((!Silencer || Silencer->GetbFireEffect()) && WeaponTppPrimitiveInfo.Contains("FireEffect"))
	{
		WeaponTppPrimitiveInfo["FireEffect"]->Activate(true);
		WeaponFppPrimitiveInfo["FireEffect"]->Activate(true);
	}

	// �߻� �Ҹ�
	if (AttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation
		(
			this,
			Silencer ? Silencer->GetSilencerFireSound() : AttackSound,
			OwnerCreature->GetActorLocation()
		);
	}

}

void AGun::ImpactBulletEvent(FVector ImpactPos)
{
	// ���� �Ҹ�
	if (ImpactEffect)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactEffect, ImpactPos);

	// ���� ����Ʈ
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ImpactSound,
			ImpactPos
		);
	}
}

bool AGun::CanAttack()
{
	if(!Super::CanAttack())
		return false;

	if(CurAmmo <= 0) // �����ִ� �Ѿ��� �ִ���
		return false;

	// ������ ������
	UWeaponReloadComponent* ReloadComp = this->GetComponentByClass<UWeaponReloadComponent>();
	if(ReloadComp && ReloadComp->IsReloading())
		return false;

	return true;
}

bool AGun::CanSubAttack()
{
	if (!Super::CanSubAttack())
		return false;

	// ������ ������
	UWeaponReloadComponent* ReloadComp = this->GetComponentByClass<UWeaponReloadComponent>();
	if (ReloadComp && ReloadComp->IsReloading())
		return false;

	return true;
}
