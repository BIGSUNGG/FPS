// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Net/UnrealNetwork.h"

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

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
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

void AWeapon::Equipped(ACreature* Character)
{
	OwnerCharacter = Character;
	Character->OnAttackStartDelegate.AddDynamic(this, &AWeapon::AttackStartEvent);
	Character->OnAttackEndDelegate.AddDynamic(this, &AWeapon::AttackEndEvent);

	Server_Equipped(Character);
}

void AWeapon::Server_Equipped_Implementation(ACreature* Character)
{
	WeaponState = EWeaponState::EQUIPPED;
}

bool AWeapon::GetCanInteract()
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

void AWeapon::AttackStartEvent()
{
	UE_LOG(LogTemp, Log, TEXT("AttackStart"));
	IsAttacking = true;
	Attack();
	if(IsAutomaticAttack)
		GetWorldTimerManager().SetTimer(AutomaticAttackHandle, this, &AWeapon::Attack, AttackDelay, true);
}

void AWeapon::AttackEndEvent()
{
	UE_LOG(LogTemp, Log, TEXT("AttackEnd"));
	IsAttacking = false;
	GetWorldTimerManager().ClearTimer(AutomaticAttackHandle);
}

void AWeapon::Attack()
{
}

