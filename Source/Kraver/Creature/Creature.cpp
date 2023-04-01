// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature.h"
#include "Kraver/Anim/Creature/CreatureAnimInstance.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACreature::ACreature()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	RpcComponent = CreateDefaultSubobject<URpcComponent>("ServerComponent");

	CombatComponent = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	CombatComponent->SetIsReplicated(true);
	CombatComponent->OnDeath.AddDynamic(this, &ACreature::OnDeathEvent);
	CombatComponent->OnEquipWeaponSuccess.AddDynamic(this, &ACreature::OnEquipWeaponSuccessEvent);
	CombatComponent->OnUnEquipWeaponSuccess.AddDynamic(this, &ACreature::OnUnEquipWeaponSuccessEvent);
	CombatComponent->OnAfterTakePointDamage.AddDynamic(this, &ACreature::OnAfterTakePointDamageEvent);
	CombatComponent->OnHoldWeapon.AddDynamic(this, &ACreature::OnHoldWeaponEvent);
	CombatComponent->OnHolsterWeapon.AddDynamic(this, &ACreature::OnHolsterWeaponEvent);

	SpringArm->SetupAttachment(GetMesh());
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
	DOREPLIFETIME(ACreature, IsJumping);
	DOREPLIFETIME(ACreature, AO_Pitch);
	DOREPLIFETIME(ACreature, AO_Yaw);
}

float ACreature::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	float Damage = CombatComponent->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
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
	PlayerInputComponent->BindAction(TEXT("SubAttack"), EInputEvent::IE_Pressed, this, &ACreature::SubAttackButtonPressed);
	PlayerInputComponent->BindAction(TEXT("SubAttack"), EInputEvent::IE_Released, this, &ACreature::SubAttackButtonReleased);

}

bool ACreature::GetCanAttack()
{
	if (MovementState == EMovementState::SPRINT && GetMovementComponent()->IsFalling() == false)
		return false;
	if (CombatComponent->GetCurWeapon() && GetMesh()->GetAnimInstance()->Montage_IsPlaying(CombatComponent->GetCurWeapon()->GetReloadMontageTpp()))
		return false;

	return true;
}

void ACreature::MoveForward(float NewAxisValue)
{
	CurrentInputForward =NewAxisValue;
	
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
	CurrentInputRight = NewAxisValue;

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
	if (CombatComponent->GetCurWeapon() == nullptr ||
		CombatComponent->GetCurWeapon()->GetCanReload() == false ||
		GetMesh()->GetAnimInstance()->Montage_IsPlaying(CombatComponent->GetCurWeapon()->GetReloadMontageTpp()) == true)
	{
		return;
	}

	CombatComponent->SetIsAttacking(false);
	RpcComponent->Montage_Play(GetMesh(), CombatComponent->GetCurWeapon()->GetReloadMontageTpp(), 1.5f);
}

void ACreature::AttackButtonPressed()
{
	if (GetCanAttack())
	{
		CombatComponent->SetIsAttacking(true);
	}

}

void ACreature::AttackButtonReleased()
{
	CombatComponent->SetIsAttacking(false);
}

void ACreature::SubAttackButtonPressed()
{
	CombatComponent->SetIsSubAttacking(true);
}

void ACreature::SubAttackButtonReleased()
{
	CombatComponent->SetIsSubAttacking(false);
}

void ACreature::RunButtonPressed()
{
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
	if(GetCharacterMovement()->IsFalling())
		return;

	if (bIsCrouched)
		UnCrouch();
	else
		Crouch();
}

void ACreature::JumpingButtonPressed()
{
	if(GetMovementComponent()->IsCrouching())
		CrouchButtonPressed();

	if(GetMovementComponent()->IsFalling() == false)
		SetIsJumping(true);

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
	if(!IsLocallyControlled())
		return;

	FRotator Rotator = Camera->GetComponentRotation() - GetMesh()->GetComponentRotation();
	SetAO_Pitch(Rotator.Pitch);
	SetAO_Yaw(Rotator.Yaw - 90.f);
}

void ACreature::SetAO_Yaw(float value)
{
	AO_Yaw = value;
	Server_SetAO_Yaw(value);
}

void ACreature::Server_SetAO_Yaw_Implementation(float value)
{
	AO_Yaw = value;
}

void ACreature::SetAO_Pitch(float value)
{
	AO_Pitch = value;
	Server_SetAO_Pitch(value);
}

void ACreature::Server_SetAO_Pitch_Implementation(float value)
{
	AO_Pitch = value;
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

void ACreature::SetIsJumping(bool value)
{
	IsJumping = value;
	Server_SetIsJumping(value);
}

void ACreature::Server_SetIsJumping_Implementation(bool value)
{
	IsJumping = value;
}

void ACreature::OnEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	if (Weapon == nullptr)
		return;

	CombatComponent->GetCurWeapon()->OnAttack.AddDynamic(this, &ACreature::OnCurWeaponAttackEvent);
	Server_OnEquipWeaponSuccessEvent(Weapon);
}

void ACreature::OnUnEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	Weapon->OnAttack.RemoveDynamic(this, &ACreature::OnCurWeaponAttackEvent);
	RpcComponent->SetHiddenInGame(Weapon->GetWeaponMesh(), false);
	Server_OnUnEquipWeaponSuccessEvent(Weapon);
}

void ACreature::OnHoldWeaponEvent(AWeapon* Weapon)
{
	RpcComponent->SetHiddenInGame(Weapon->GetWeaponMesh(), false);

}

void ACreature::OnHolsterWeaponEvent(AWeapon* Weapon)
{
	if(!Weapon)
		return;

	RpcComponent->SetHiddenInGame(Weapon->GetWeaponMesh(), true);
	RpcComponent->Montage_Stop(GetMesh(), Weapon->GetReloadMontageTpp());
}

void ACreature::OnDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	RpcComponent->SetSimulatedPhysics(GetMesh(), true);
	DisableInput(GetController<APlayerController>());
	if (CombatComponent->GetCurWeapon() != nullptr)
		CombatComponent->UnEquipWeapon(CombatComponent->GetCurWeapon());
	RpcComponent->SetCollisionProfileName(GetCapsuleComponent(), FName("DeadPawn"));
	RpcComponent->SetCollisionProfileName(GetMesh(), FName("Ragdoll"));

	Server_OnDeathEvent(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ACreature::OnCurWeaponAttackEvent()
{
	if(CombatComponent->GetCurWeapon() == nullptr)
		UE_LOG(LogTemp,Fatal,TEXT("Cur weapon is nullptr but it attacked"));

	RpcComponent->Montage_Play(GetMesh(), CombatComponent->GetCurWeapon()->GetAttackMontageTpp());
}

void ACreature::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	UCreatureAnimInstance* CreatureAnim = Cast<UCreatureAnimInstance>(GetMesh()->GetAnimInstance());
	RpcComponent->Montage_Play(GetMesh(), CreatureAnim->GetLandedMontage());

	SetIsJumping(false);
}

void ACreature::OnAfterTakePointDamageEvent(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	AWeapon* Weapon = Cast<AWeapon>(DamageCauser);
	if (Weapon && CombatComponent->GetIsDead())
	{
		RpcComponent->AddImpulseAtLocation
		(
			GetMesh(),
			DamageEvent.ShotDirection * Weapon->GetAttackImpulse() * GetMesh()->GetMass() * ImpulseResistanceRatio,
			DamageEvent.HitInfo.ImpactPoint
		);
	}
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

void ACreature::Server_OnEquipWeaponSuccessEvent_Implementation(AWeapon* Weapon)
{
	Weapon->GetWeaponMesh()->AttachToComponent
	(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		Weapon->GetAttachSocketName()
	);

}
