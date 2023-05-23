// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature.h"
#include "Kraver/Anim/Creature/CreatureAnimInstance.h"
#include "Kraver/Weapon/Melee/Melee.h"
#include "Kraver/KraverComponent/Weapon/WeaponAssassinate/WeaponAssassinateComponent.h"

// Sets default values
ACreature::ACreature()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	CombatComponent = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	CombatComponent->SetIsReplicated(true);

	CombatComponent->OnClientDeath.AddDynamic(this, &ACreature::OnClientDeathEvent);
	CombatComponent->OnServerDeath.AddDynamic(this, &ACreature::OnServerDeathEvent);

	CombatComponent->OnClientEquipWeaponSuccess.AddDynamic(this, &ACreature::OnClientEquipWeaponSuccessEvent);
	CombatComponent->OnServerEquipWeaponSuccess.AddDynamic(this, &ACreature::OnServerEquipWeaponSuccessEvent);
	CombatComponent->OnClientUnEquipWeaponSuccess.AddDynamic(this, &ACreature::OnClientUnEquipWeaponSuccessEvent);	
	CombatComponent->OnServerUnEquipWeaponSuccess.AddDynamic(this, &ACreature::OnServerUnEquipWeaponSuccessEvent);

	CombatComponent->OnClientAfterTakeDamageSuccess.AddDynamic(this, &ACreature::OnClientAfterTakeDamageEvent);

	CombatComponent->OnClientHoldWeapon.AddDynamic(this, &ACreature::OnClientHoldWeaponEvent);
	CombatComponent->OnServerHoldWeapon.AddDynamic(this, &ACreature::OnServerHoldWeaponEvent);
	CombatComponent->OnClientHolsterWeapon.AddDynamic(this, &ACreature::OnClientHolsterWeaponEvent);
	CombatComponent->OnServerHolsterWeapon.AddDynamic(this, &ACreature::OnServerHolsterWeaponEvent);
	
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritRoll = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bDoCollisionTest = false;

	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = true;

	GetCharacterMovement()->AirControl = 0.25f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->SetWalkableFloorAngle(50.f);

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

	float Damage;
	if (DamageEvent.IsOfType(FKraverDamageEvent::ClassID))
	{
		const FKraverDamageEvent* KraverDamageEvent = static_cast<const FKraverDamageEvent*>(&DamageEvent);
		Damage = CombatComponent->TakeDamage(DamageAmount, *KraverDamageEvent, EventInstigator, DamageCauser);
	}
	else
	{
		Damage = 0.f;
		KR_LOG(Error, TEXT("Use only FKraverDamageEvent"));
	}
	return Damage;
}

void ACreature::Assassinated(ACreature* Attacker, FAssassinateInfo AssassinateInfo)
{
	Server_Assassinated(Attacker, AssassinateInfo);
}

void ACreature::AssassinatedEnd()
{
	Server_OnAssassinatedEndEvent();
}

void ACreature::OwnOtherActor(AActor* Actor)
{
	Actor->SetOwner(this);
	Server_OwnOtherActor(Actor);
}

// Called when the game starts or when spawned
void ACreature::BeginPlay()
{
	Super::BeginPlay();

	DefaultGravity = GetCharacterMovement()->GravityScale;
	DefaultGroundFriction = GetCharacterMovement()->GroundFriction;
	DefaultBrakingDecelerationWalking = GetCharacterMovement()->BrakingDecelerationWalking;
	DefaultCameraLocation = Camera->GetRelativeLocation();

}

void ACreature::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

// Called every frame
void ACreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CameraTick(DeltaTime);
	AimOffset(DeltaTime);
}

void ACreature::CameraTick(float DeltaTime)
{
	Camera->SetRelativeLocation(FMath::VInterpTo(Camera->GetRelativeLocation(), TargetCameraRelativeLocation, DeltaTime, 5.f));
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
	PlayerInputComponent->BindAction(TEXT("Run"), EInputEvent::IE_Released, this, &ACreature::RunButtonReleased);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &ACreature::CrouchButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Released, this, &ACreature::CrouchButtonReleased);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACreature::JumpingButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ACreature::JumpingButtonReleased);
	PlayerInputComponent->BindAction(TEXT("SubAttack"), EInputEvent::IE_Pressed, this, &ACreature::SubAttackButtonPressed);
	PlayerInputComponent->BindAction(TEXT("SubAttack"), EInputEvent::IE_Released, this, &ACreature::SubAttackButtonReleased);
	PlayerInputComponent->BindAction(TEXT("HolsterWeapon"), EInputEvent::IE_Pressed, this, &ACreature::HolsterWeaponPressed);

}

void ACreature::OnAssassinateEvent(AActor* AssassinatedActor)
{	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	DisableInput(PlayerController);

	Camera->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "head");

	GetMovementComponent()->Velocity = FVector::ZeroVector;
}

void ACreature::OnAssassinateEndEvent()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	EnableInput(PlayerController);

	Camera->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Camera->AttachToComponent(SpringArm, FAttachmentTransformRules::SnapToTargetIncludingScale);

	FTransform HeadTransform = GetMesh()->GetSocketTransform("head", ERelativeTransformSpace::RTS_World);
	FTransform CameraTransform = Camera->GetComponentTransform();
	FTransform RelativeTransform = HeadTransform.GetRelativeTransform(CameraTransform);
	FVector RelativeLocation = RelativeTransform.GetLocation();
	Camera->SetRelativeLocation(RelativeLocation);

	TargetCameraRelativeLocation = FVector::ZeroVector;
}

bool ACreature::GetCanAttack()
{
	if (MovementState == EMovementState::SPRINT && GetMovementComponent()->IsFalling() == false)
		return false;

	return true;
}

float ACreature::CalculateForwardSpeed()
{
	FVector Velocity = GetCharacterMovement()->Velocity;

	FVector Forward = GetControlRotation().Vector().GetSafeNormal2D();
	return FVector::DotProduct(Velocity, Forward) / Forward.Size2D();
}

float ACreature::CalculateRightSpeed()
{
	FVector Velocity = GetCharacterMovement()->Velocity;

	FVector Right = GetControlRotation().Vector().GetSafeNormal2D().RotateAngleAxis(-90.0f, FVector::UpVector);
	return -FVector::DotProduct(Velocity, Right) / Right.Size2D();
}

float ACreature::CalculateCurrentFloorSlope()
{
	if(GetCharacterMovement()->IsFalling())
		return 0.f;

	FHitResult HitResult;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 60.f);
	FCollisionQueryParams QueryParams;
	GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_ONLY_OBJECT, QueryParams);

	// Calculate floor slope if hit result is valid
	if (HitResult.IsValidBlockingHit())
	{
		FVector FloorNormal = HitResult.Normal;
		float FloorSlope = FMath::Acos(FloorNormal.Z) * 180.0f / PI; // Convert to degrees
		return FloorSlope;
	}

	return 0.0f;
}

FVector ACreature::CaclulateCurrentFllorSlopeVector()
{
	UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement();

	if(CharacterMovementComp->IsFalling())
		return FVector::ZeroVector;

	if (CharacterMovementComp)
	{
		FHitResult HitResult;
		FVector StartLocation = GetActorLocation();
		FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 60.f);
		FCollisionQueryParams QueryParams;
		QueryParams.bFindInitialOverlaps = true;

		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_ONLY_OBJECT, QueryParams))
		{
			FVector FloorNormal = HitResult.ImpactNormal;
			return FloorNormal;
		}
	}
	return FVector::ZeroVector;
}

void ACreature::MoveForward(float NewAxisValue)
{
	NewAxisValue *= InputForwardRatio;
	CurrentInputForward = NewAxisValue;
	
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
	NewAxisValue *= InputRightRatio;
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
	if(CombatComponent->GetCurWeapon() == nullptr)
		return;

	CombatComponent->GetCurWeapon()->OnSkillFirst.Broadcast();
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
	IsRunning = true;
}

void ACreature::RunButtonReleased()
{
	IsRunning = false;
}

void ACreature::CrouchButtonPressed()
{
	bCrouchButtonPress = true;
	Crouch();
}

void ACreature::CrouchButtonReleased()
{
	bCrouchButtonPress = false;
	UnCrouch();
}

void ACreature::JumpingButtonPressed()
{
	bJumpButtonPress = true;

	if(GetMovementComponent()->IsFalling() == false)
		SetIsJumping(true);

	Jump();
}

void ACreature::JumpingButtonReleased()
{
	bJumpButtonPress = false;

	if (IsJumping)
		StopJumping();
}

void ACreature::HolsterWeaponPressed()
{
	if(CombatComponent->GetCurWeapon())
		CombatComponent->HolsterWeapon(CombatComponent->GetCurWeapon());
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
			GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchWalkSpeed;
			break;
		case EMovementState::RUN:
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
			GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchRunSpeed;
			break;
		case EMovementState::SPRINT:
			GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
			GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSprintSpeed;
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
		GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchWalkSpeed;
		break;
	case EMovementState::RUN:
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchRunSpeed;
		break;
	case EMovementState::SPRINT:
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSprintSpeed;
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

void ACreature::OnClientEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	if (Weapon == nullptr)
		return;

}

void ACreature::OnServerEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	if (IS_SERVER() == false)
	{
		KR_LOG(Error, TEXT("Called on client"));
		return;
	}

	Weapon->SetOwner(this);

	Weapon->GetWeaponMesh()->AttachToComponent
	(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		Weapon->GetAttachSocketName()
	);
}

void ACreature::OnServerUnEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	if (IS_SERVER() == false)
	{
		KR_LOG(Error, TEXT("Called on client"));
		return;
	}

	Weapon->GetWeaponMesh()->SetHiddenInGame(false);

	Multicast_OnUnEquipWeaponSuccessEvent(Weapon);
}

void ACreature::OnClientUnEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	Weapon->GetWeaponMesh()->SetHiddenInGame(false);
}

void ACreature::OnClientHoldWeaponEvent(AWeapon* Weapon)
{
	Weapon->GetWeaponMesh()->SetHiddenInGame(false);
	Weapon->OnPlayTppMontage.AddDynamic(this, &ACreature::OnPlayWeaponTppMontageEvent);
	Weapon->OnPlayFppMontage.AddDynamic(this, &ACreature::OnPlayWeaponFppMontageEvent);

	switch (Weapon->GetWeaponType())
	{
	case EWeaponType::NONE:
		break;
	case EWeaponType::GUN:
		break;
	case EWeaponType::MELEE:
		break;
	default:
		break;
	}

	UWeaponAssassinateComponent* AssassinateComp = Weapon->FindComponentByClass<UWeaponAssassinateComponent>();
	if (AssassinateComp)
	{
		AssassinateComp->OnAssassinate.AddDynamic(this, &ACreature::OnAssassinateEvent);
		AssassinateComp->OnAssassinateEnd.AddDynamic(this, &ACreature::OnAssassinateEndEvent);
	}
}

void ACreature::OnServerHoldWeaponEvent(AWeapon* Weapon)
{
	Multicast_HoldWeaponEvent(Weapon);
}

void ACreature::OnClientHolsterWeaponEvent(AWeapon* Weapon)
{
	if(!Weapon)
		return;

	Weapon->OnPlayTppMontage.RemoveDynamic(this, &ACreature::OnPlayWeaponTppMontageEvent);
	Weapon->OnPlayFppMontage.RemoveDynamic(this, &ACreature::OnPlayWeaponFppMontageEvent);

	Weapon->GetWeaponMesh()->SetHiddenInGame(true);
	GetMesh()->GetAnimInstance()->Montage_Stop(0.f, Weapon->GetAttackMontageTpp());

	switch (Weapon->GetWeaponType())
	{
	case EWeaponType::NONE:
		break;
	case EWeaponType::GUN:
		break;
	case EWeaponType::MELEE:
		break;
	default:
		break;
	}

	UWeaponAssassinateComponent* AssassinateComp = Weapon->FindComponentByClass<UWeaponAssassinateComponent>();
	if (AssassinateComp)
	{
		AssassinateComp->OnAssassinate.RemoveDynamic(this, &ACreature::OnAssassinateEvent);
		AssassinateComp->OnAssassinateEnd.RemoveDynamic(this, &ACreature::OnAssassinateEndEvent);
	}
}

void ACreature::OnServerHolsterWeaponEvent(AWeapon* Weapon)
{
	Multicast_HolsterWeaponEvent(Weapon);
}

void ACreature::OnClientDeathEvent(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	DisableInput(GetController<APlayerController>());
	if (CombatComponent->GetCurWeapon() != nullptr)
		CombatComponent->UnEquipWeapon(CombatComponent->GetCurWeapon());

	GetCapsuleComponent()->SetCollisionProfileName(FName("DeadPawn"));
}

void ACreature::OnServerDeathEvent(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	if (DamageEvent.bCanSimulate)
	{
		SimulateMesh();
	}

	Multicast_OnDeathEvent(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ACreature::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	SetIsJumping(false);
	PlayLandedMontage();
}

void ACreature::OnClientAfterTakeDamageEvent(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	Server_OnAfterTakeDamageEvent(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ACreature::OnEndCrouch(float HeightAdjust, float ScaledHeightAdjust)
{
	Super::OnEndCrouch(HeightAdjust,ScaledHeightAdjust);

}
 
void ACreature::OnPlayWeaponTppMontageEvent(UAnimMontage* PlayedMontage, float Speed)
{
	Server_OnPlayWeaponTppMontageEvent(PlayedMontage, Speed);
}

void ACreature::OnPlayWeaponFppMontageEvent(UAnimMontage* PlayedMontage, float Speed)
{
	Server_OnPlayWeaponFppMontageEvent(PlayedMontage, Speed);
}

void ACreature::Server_OwnOtherActor_Implementation(AActor* Actor)
{
	Actor->SetOwner(this);
}

void ACreature::Server_Assassinated_Implementation(ACreature* Attacker, FAssassinateInfo AssassinateInfo)
{
	SetActorLocationAndRotation
	(
		Attacker->GetActorLocation() + Attacker->GetActorForwardVector() * 100,
		Attacker->GetActorRotation()
	);

	Client_Assassinated(Attacker, AssassinateInfo);
	Multicast_Assassinated(Attacker, AssassinateInfo);
}

void ACreature::Multicast_Assassinated_Implementation(ACreature* Attacker, FAssassinateInfo AssassinateInfo)
{
	GetMesh()->GetAnimInstance()->Montage_Play(AssassinateInfo.AssassinatedMontagesTpp);
}

void ACreature::Client_Assassinated_Implementation(ACreature* Attacker, FAssassinateInfo AssassinateInfo)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	DisableInput(PlayerController);
}

void ACreature::Multicast_OnUnEquipWeaponSuccessEvent_Implementation(AWeapon* Weapon)
{
	Weapon->GetWeaponMesh()->SetHiddenInGame(false);
}

void ACreature::Multicast_OnDeathEvent_Implementation(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	GetCapsuleComponent()->SetCollisionProfileName(FName("DeadPawn"));
}

void ACreature::Server_OnAfterTakeDamageEvent_Implementation(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Multicast_OnAfterTakeDamageEvent(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ACreature::Multicast_OnAfterTakeDamageEvent_Implementation(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (GetMesh()->IsSimulatingPhysics())
	{
		FVector Direction = DamageEvent.GetHitDirection();

		GetMesh()->AddImpulseAtLocation(
			Direction * DamageEvent.DamageImpulse * GetMesh()->GetMass() * ImpulseResistanceRatio,
			DamageEvent.HitInfo.ImpactPoint
		);
	}
}

void ACreature::Server_Jump_Implementation()
{
	Multicast_Jump();
}

void ACreature::Multicast_Jump_Implementation()
{
	UCreatureAnimInstance* CreatureAnim = Cast<UCreatureAnimInstance>(GetMesh()->GetAnimInstance());
	GetMesh()->GetAnimInstance()->Montage_Play(CreatureAnim->GetJumpMontage());
}

void ACreature::Server_OnPlayWeaponTppMontageEvent_Implementation(UAnimMontage* PlayedMontage, float Speed)
{
	Multicast_OnPlayWeaponTppMontageEvent(PlayedMontage, Speed);
}

void ACreature::Multicast_OnPlayWeaponTppMontageEvent_Implementation(UAnimMontage* PlayedMontage, float Speed)
{
	GetMesh()->GetAnimInstance()->Montage_Play(PlayedMontage, Speed);
}

void ACreature::Server_OnPlayWeaponFppMontageEvent_Implementation(UAnimMontage* PlayedMontage, float Speed)
{
	Multicast_OnPlayWeaponFppMontageEvent(PlayedMontage, Speed);
}

void ACreature::Multicast_OnPlayWeaponFppMontageEvent_Implementation(UAnimMontage* PlayedMontage, float Speed)
{

}

void ACreature::Multicast_HoldWeaponEvent_Implementation(AWeapon* Weapon)
{
	Weapon->GetWeaponMesh()->SetHiddenInGame(false);
}

void ACreature::Multicast_HolsterWeaponEvent_Implementation(AWeapon* Weapon)
{
	Weapon->GetWeaponMesh()->SetHiddenInGame(true);
}

void ACreature::Client_SimulateMesh_Implementation()
{

}

void ACreature::Multicast_SimulateMesh_Implementation()
{
	GetMesh()->SetSimulatePhysics(true);
}

void ACreature::Server_OnAssassinatedEndEvent_Implementation()
{
	SimulateMesh();
}

void ACreature::PlayLandedMontage()
{
	UCreatureAnimInstance* CreatureAnim = Cast<UCreatureAnimInstance>(GetMesh()->GetAnimInstance());
	GetMesh()->GetAnimInstance()->Montage_Play(CreatureAnim->GetLandedMontage());
}

void ACreature::SimulateMesh()
{
	if (IS_SERVER() == false)
	{
		KR_LOG(Error, TEXT("Called on server"));
		return;
	}

	Client_SimulateMesh();
	Multicast_SimulateMesh();
}

void ACreature::Jump()
{
	ACharacter::Jump();
}
