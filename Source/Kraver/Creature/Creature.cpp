// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature.h"
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
	ServerComponent = CreateDefaultSubobject<UServerComponent>("ServerComponent");

	CombatComponent = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	CombatComponent->SetIsReplicated(true);
	CombatComponent->OnEquipWeaponSuccess.AddDynamic(this, &ACreature::OnEquipWeaponSuccess);
	CombatComponent->OnServerEquipWeaponSuccess.AddDynamic(this, &ACreature::Server_OnEquipWeaponSuccess);
	CombatComponent->OnUnEquipWeaponSuccess.AddDynamic(this, &ACreature::OnUnEquipWeaponSuccess);
	CombatComponent->OnServerUnEquipWeaponSuccess.AddDynamic(this, &ACreature::Server_OnUnEquipWeaponSuccess);

	SpringArm->SetupAttachment(GetMesh());
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritRoll = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bDoCollisionTest = true;
	bUseControllerRotationYaw = false;

	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.f, 0.0f);
	GetCharacterMovement()->AirControl = 0.25f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}	


void ACreature::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACreature, IsSprint);
}

// Called when the game starts or when spawned
void ACreature::BeginPlay()
{
	Super::BeginPlay();

	GetController()->SetControlRotation(GetActorRotation());
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

}

bool ACreature::GetCanAttack()
{
	return !(IsRunning || IsSprint);
}

void ACreature::SetIsSprint(bool value)
{
	IsSprint = value;
	OnServer_SetIsSprint(value);
}

void ACreature::MoveForward(float NewAxisValue)
{
	if(!GetMovementComponent()->IsFalling())
	{ 
		SetIsSprint(false);
		ServerComponent->SetCharacterWalkSpeed(this, WalkSpeed);

		if(NewAxisValue == 0 || Controller == nullptr)
			return;

		if(IsRunning)
		{ 
			if (NewAxisValue >= 0.5f)
			{
				SetIsSprint(true);
				ServerComponent->SetCharacterWalkSpeed(this, SprintSpeed);
			}
			else
				ServerComponent->SetCharacterWalkSpeed(this, RunSpeed);
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
	CombatComponent->Reload();
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
	CombatComponent->SetIsAttacking(false);

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

	if(bIsCrouched)
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
		IsJumping = false;
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

void ACreature::OnServer_SetIsSprint_Implementation(bool value)
{
	IsSprint = value;

}

void ACreature::OnEquipWeaponSuccess(AWeapon* Weapon)
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

void ACreature::OnUnEquipWeaponSuccess(AWeapon* Weapon)
{
}

void ACreature::Server_OnUnEquipWeaponSuccess_Implementation(AWeapon* Weapon)
{
}

void ACreature::Server_OnEquipWeaponSuccess_Implementation(AWeapon* Weapon)
{
	CombatComponent->GetCurWeapon()->GetWeaponMesh()->AttachToComponent
	(
		GetMesh(), 
		FAttachmentTransformRules::SnapToTargetIncludingScale, 
		CombatComponent->GetCurWeapon()->GetAttachSocketName()
	);
}
