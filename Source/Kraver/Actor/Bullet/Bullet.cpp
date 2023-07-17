// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Kraver/Component/Combat/CombatComponent.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("BulletMesh");
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");

	SetRootComponent(BulletMesh);

	BulletMesh->SetupAttachment(BulletMesh);
	BulletMesh->SetCollisionProfileName("BulletShape");
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BulletMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
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

void ABullet::FireBullet(FVector& Direction, AActor* Target /*= nullptr*/)
{
	ProjectileMovementComponent->ConstrainDirectionToPlane(Direction);
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
		Destroy();
}

void ABullet::OnCollisionEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	HitEvent(OtherActor, OtherComponent, Hit);

}

void ABullet::GiveDamage(AActor* OtherActor, UPrimitiveComponent* OtherComponent, const FHitResult& Hit)
{
	// Find CombatComponent 
	AActor* CurActor = GetOwner();
	UCombatComponent* CombatComp = nullptr;
	while (CurActor)
	{
		CombatComp = CurActor->FindComponentByClass<UCombatComponent>();
		if(CombatComp)
			break;

		CurActor = CurActor->GetOwner();
	}

	FKraverDamageEvent DamageEvent;
	DamageEvent.DamageType = EKraverDamageType::BULLET;
	DamageEvent.HitInfo = Hit;
	DamageEvent.bCanHeadShot = true;
	CombatComp->GiveDamage(OtherActor, BulletDamage, DamageEvent, CurActor->GetInstigatorController(), GetOwner());

	OnImpact.Broadcast(this, OtherActor, OtherComponent, Hit);
}
