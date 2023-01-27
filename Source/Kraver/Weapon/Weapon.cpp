// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->SetCollisionProfileName(FName("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	//ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	//ProjectileMovement->bShouldBounce = true;
	//ProjectileMovement->bBounceAngleAffectsFriction = true;
	//ProjectileMovement->Bounciness = 0.3f;
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

void AWeapon::Equipped(ACreature* Character , bool AttachToMesh)
{
	OwnerCharacter = Character;
	Character->OnAttackStartDelegate.AddDynamic(this, &AWeapon::AttackStartEvent);
	Character->OnAttackEndDelegate.AddDynamic(this, &AWeapon::AttackEndEvent);

}

void AWeapon::AttackStartEvent()
{
	UE_LOG(LogTemp, Warning, TEXT("Start"));
	IsAttacking = true;
	Attack();
	if(IsAutomaticAttack)
		GetWorldTimerManager().SetTimer(AutomaticAttackHandle, this, &AWeapon::Attack, AttackDelay, true);
}

void AWeapon::AttackEndEvent()
{
	UE_LOG(LogTemp, Warning, TEXT("End"));
	IsAttacking = false;
	GetWorldTimerManager().ClearTimer(AutomaticAttackHandle);
}

void AWeapon::Attack()
{
}

