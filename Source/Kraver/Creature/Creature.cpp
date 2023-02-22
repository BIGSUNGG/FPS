// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Kraver/Anim/CreatureAnimInstance.h"

// Sets default values
ACreature::ACreature()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	ServerComponent = CreateDefaultSubobject<UServerComponent>("ServerComponent");

	CombatComponent = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	CombatComponent->SetIsReplicated(true);
	CombatComponent->OnDeath.AddDynamic(this, &ACreature::OnDeathEvent);
	CombatComponent->OnEquipWeaponSuccess.AddDynamic(this, &ACreature::OnEquipWeaponSuccessEvent);
	CombatComponent->OnServerEquipWeaponSuccess.AddDynamic(this, &ACreature::Server_OnEquipWeaponSuccessEvent);
	CombatComponent->OnUnEquipWeaponSuccess.AddDynamic(this, &ACreature::OnUnEquipWeaponSuccessEvent);
	CombatComponent->OnServerUnEquipWeaponSuccess.AddDynamic(this, &ACreature::Server_OnUnEquipWeaponSuccessEvent);

	SpringArm->SetupAttachment(GetCapsuleComponent());
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritRoll = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bDoCollisionTest = true;

	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = true;

	GetCharacterMovement()->AirControl = 0.25f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;

}	


void ACreature::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACreature, MovementState);
}

float ACreature::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	float Damage = CombatComponent->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		AWeapon* Weapon = Cast<AWeapon>(DamageCauser);
		if (Weapon && CombatComponent->IsDead())
		{
			ServerComponent->AddImpulseAtLocation
			(
				GetMesh(),
				PointDamageEvent->ShotDirection * Weapon->GetAttackImpulse() * GetMesh()->GetMass(),
				PointDamageEvent->HitInfo.ImpactPoint
			);
		}
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
	}
	return Damage;
}

// Called when the game starts or when spawned
void ACreature::BeginPlay()
{
	Super::BeginPlay();
}

void ACreature::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

// Called every frame
void ACreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsJumping && GetCharacterMovement()->IsFalling() == false)
		IsJumping = false;
	
	AimOffset(DeltaTime);
}

// Called to bind functionality to input
void ACreature::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis(TEXT("MoveForward")	, this, &ACreature::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACreature::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp")	, this, &ACreature::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ACreature::Turn);

	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &ACreature::ReloadButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &ACreature::AttackButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Released, this, &ACreature::AttackButtonReleased);
	PlayerInputComponent->BindAction(TEXT("Run"), EInputEvent::IE_Pressed, this, &ACreature::RunButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Run"), EInputEvent::IE_Released, this, &ACreature::RunButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &ACreature::CrouchButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACreature::JumpingButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ACreature::JumpingButtonReleased);

}

bool ACreature::GetCanAttack()
{
	return !(MovementState == EMovementState::SPRINT);
}

void ACreature::MoveForward(float NewAxisValue)
{
	if(!GetMovementComponent()->IsFalling())
	{ 
		SetMovementState(EMovementState::WALK);

		if(NewAxisValue == 0 || Controller == nullptr)
			return;

		if(IsRunning)
		{ 
			if (NewAxisValue >= 0.5f)
				SetMovementState(EMovementState::SPRINT);
			else
				SetMovementState(EMovementState::RUN);
		}
	}

	FRotator Rotation = GetController()->GetControlRotation();
	FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), NewAxisValue);
}

void ACreature::MoveRight(float NewAxisValue)
{
	if(NewAxisValue == 0 || Controller == nullptr || NewAxisValue == 0)
		return;

	FRotator Rotation = GetController()->GetControlRotation();
	FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), NewAxisValue);
}

void ACreature::LookUp(float NewAxisValue)
{
	AddControllerPitchInput(NewAxisValue);
}

void ACreature::Turn(float NewAxisValue)
{
	AddControllerYawInput(NewAxisValue);
}

void ACreature::ReloadButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("Reload"));
	CombatComponent->RefillAmmo();
}

void ACreature::AttackButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("AttackStartPress"));
	if (GetCanAttack())
	{
		UE_LOG(LogTemp, Log, TEXT("Cant Attack"));
		CombatComponent->SetIsAttacking(true);
	}

}

void ACreature::AttackButtonReleased()
{
	UE_LOG(LogTemp, Log, TEXT("AttackEndPress"));
	CombatComponent->SetIsAttacking(false);
}

void ACreature::RunButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("Run"));

	if (IsRunning)
	{
		IsRunning = false;
	}
	else
	{
		IsRunning = true;
	}
}

void ACreature::CrouchButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("Crouch"));
	if(GetCharacterMovement()->IsFalling())
		return;

	if (bIsCrouched)
		UnCrouch();
	else
		Crouch();
}

void ACreature::JumpingButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("Jump"));
	if(GetMovementComponent()->IsCrouching())
		CrouchButtonPressed();

	IsJumping = true;
	Jump();
}

void ACreature::JumpingButtonReleased()
{
	if (IsJumping)
	{
		StopJumping();
	}
}

void ACreature::AimOffset(float DeltaTime)
{
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ACreature::SetMovementState(EMovementState value)
{
	MovementState = value;
	switch (value)
	{
		case EMovementState::WALK:
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
			break;
		case EMovementState::RUN:
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
			break;
		case EMovementState::SPRINT:
			GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
			break;
		default:
			break;
	}
	Server_SetMovementState(value);
}

void ACreature::Server_SetMovementState_Implementation(EMovementState value)
{
	MovementState = value;
	switch (value)
	{
	case EMovementState::WALK:
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		break;
	case EMovementState::RUN:
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		break;
	case EMovementState::SPRINT:
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		break;
	default:
		break;
	}
}

void ACreature::OnEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	if (Weapon == nullptr)
		return;

	CombatComponent->GetCurWeapon()->GetWeaponMesh()->AttachToComponent
	(
		GetMesh(), 
		FAttachmentTransformRules::SnapToTargetIncludingScale, 
		CombatComponent->GetCurWeapon()->GetAttachSocketName()
	);

	ServerComponent->AttachComponentToComponent
	(
		CombatComponent->GetCurWeapon()->GetWeaponMesh(), 
		GetMesh(), 
		CombatComponent->GetCurWeapon()->GetAttachSocketName()
	);
}

void ACreature::OnUnEquipWeaponSuccessEvent(AWeapon* Weapon)
{
}

void ACreature::OnDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ServerComponent->SetSimulatedPhysics(GetMesh(), true);
	DisableInput(GetController<APlayerController>());
	if (CombatComponent->GetCurWeapon() != nullptr)
		CombatComponent->UnEquipWeapon(CombatComponent->GetCurWeapon());
	Server_OnDeathEvent(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ACreature::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	UCreatureAnimInstance* CreatureAnim = Cast<UCreatureAnimInstance>(GetMesh()->GetAnimInstance());
	CreatureAnim->PlayLandedMontage();

	Server_Landed(Hit);
}

void ACreature::OnEndCrouch(float HeightAdjust, float ScaledHeightAdjust)
{
	Super::OnEndCrouch(HeightAdjust,ScaledHeightAdjust);

}
 
void ACreature::Server_OnUnEquipWeaponSuccessEvent_Implementation(AWeapon* Weapon)
{
}

void ACreature::Server_OnDeathEvent_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Multicast_OnDeathEvent(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ACreature::Multicast_OnDeathEvent_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
}

void ACreature::Server_Landed_Implementation(const FHitResult& Hit)
{
	Multicast_Landed(Hit);
}

void ACreature::Multicast_Landed_Implementation(const FHitResult& Hit)
{
	UCreatureAnimInstance* CreatureAnim = Cast<UCreatureAnimInstance>(GetMesh()->GetAnimInstance());
	CreatureAnim->PlayLandedMontage();
}

void ACreature::Server_OnEquipWeaponSuccessEvent_Implementation(AWeapon* Weapon)
{
	CombatComponent->GetCurWeapon()->GetWeaponMesh()->AttachToComponent
	(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		CombatComponent->GetCurWeapon()->GetAttachSocketName()
	);
}
