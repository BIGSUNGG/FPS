// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include CombatComponent_h

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;	
	SetReplicateMovement(false);

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("BulletMesh");
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");

	SetRootComponent(BulletMesh);

	BulletMesh->SetCollisionProfileName("BulletShape");
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BulletMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	ProjectileMovementComponent->bRotationFollowsVelocity = true;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
	BulletMesh->IgnoreActorWhenMoving(GetInstigator(), true);
	
	if(IS_SERVER())
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&](){ Destroy(false, true); }, BulletLifeTime, false, BulletLifeTime);
	}
	else
	{
		BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ABullet::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BulletMesh->OnComponentHit.AddDynamic(this, &ThisClass::OnCollisionEvent);
}

// Called every frame 
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullet::OnServer_FireBullet(FRotator& Direction, AActor* Target /*= nullptr*/)
{
}

void ABullet::HitEvent(AActor* OtherActor, UPrimitiveComponent* OtherComponent, const FHitResult& Hit)
{
	if (HitCount >= MaxHitCount)
		return;

	AActor* CurActor = GetOwner();
	// Check Collision Actor is Owner
	while (CurActor)
	{
		CurActor = CurActor->GetOwner();
		if(CurActor == OtherActor)
			return;
	}

	HitCount++;

	GiveDamage(OtherActor, OtherComponent, Hit);

	if (HitCount >= MaxHitCount)
		Destroy(false, true);
}

void ABullet::OnCollisionEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{	
	if (IS_SERVER())
	{
		HitEvent(OtherActor, OtherComponent, Hit);
	}
	else
	{
		KR_LOG(Error, TEXT("Function called on client"));
		return;
	}
}

void ABullet::GiveDamage(AActor* OtherActor, UPrimitiveComponent* OtherComponent, const FHitResult& Hit)
{
	if (!IS_SERVER())
	{
		KR_LOG(Error, TEXT("Function called on client"));
		return;
	}

	// Find CombatComponent 
	AActor* CurActor = GetOwner();
	UCombatComponent* CombatComp = nullptr;
	while (CurActor)
	{
		CombatComp = CurActor->FindComponentByClass<UCombatComponent>();
		if (CombatComp)
			break;

		CurActor = CurActor->GetOwner();
	}

	FVector ShotDirection = GetVelocity();
	ShotDirection.Normalize();

	FPointDamageEvent DamageEvent(BulletDamage, Hit, ShotDirection, HitDamageType);
	CombatComp->OnServer_GiveDamage(OtherActor, BulletDamage, DamageEvent, CurActor->GetInstigatorController(), GetOwner());

	OnImpact.Broadcast(this, OtherActor, OtherComponent, Hit);
}
