// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature.h"
#include "Kraver/Animation/Creature/CreatureAnimInstance.h"
#include "Kraver/Actor/Weapon/Melee/Melee.h"
#include "Kraver/Component/Skill/Weapon/WeaponAssassinate/WeaponAssassinateComponent.h"
#include "Kraver/Component/Movement/CreatureMovementComponent.h"
#include "Kraver/Component/Skill/Weapon/WeaponReload/WeaponReloadComponent.h"
#include "Kraver/GameBase/GameMode/KraverGameMode.h"

// Sets default values
ACreature::ACreature()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Fp_Root = CreateDefaultSubobject<USceneComponent>(TEXT("Fp_Root"));
	Fp_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Fp_SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Fp_Camera"));

	CreatureMovementComponent = CreateDefaultSubobject<UCreatureMovementComponent>("CreatureMovementComponent");
	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	CombatComponent->SetIsReplicated(true);

	CombatComponent->OnClientDeath.AddDynamic(this, &ACreature::OnClientDeathEvent);
	CombatComponent->OnServerDeath.AddDynamic(this, &ACreature::OnServerDeathEvent);

	CombatComponent->OnClientEquipWeaponSuccess.AddDynamic(this, &ACreature::OnClientEquipWeaponSuccessEvent);
	CombatComponent->OnServerEquipWeaponSuccess.AddDynamic(this, &ACreature::OnServerEquipWeaponSuccessEvent);
	CombatComponent->OnClientUnEquipWeaponSuccess.AddDynamic(this, &ACreature::OnClientUnEquipWeaponSuccessEvent);	
	CombatComponent->OnServerUnEquipWeaponSuccess.AddDynamic(this, &ACreature::OnServerUnEquipWeaponSuccessEvent);

	CombatComponent->OnClientAfterTakePointDamageSuccess.AddDynamic(this, &ACreature::OnClientAfterTakePointDamageEvent);
	CombatComponent->OnClientAfterTakeRadialDamageSuccess.AddDynamic(this, &ACreature::OnClientAfterTakeRadialDamageEvent);

	CombatComponent->OnClientUnholsterWeapon.AddDynamic(this, &ACreature::OnClientUnholsterWeaponEvent);
	CombatComponent->OnServerUnholsterWeapon.AddDynamic(this, &ACreature::OnServerUnholsterWeaponEvent);
	CombatComponent->OnClientHolsterWeapon.AddDynamic(this, &ACreature::OnClientHolsterWeaponEvent);
	CombatComponent->OnServerHolsterWeapon.AddDynamic(this, &ACreature::OnServerHolsterWeaponEvent);

	CombatComponent->OnRepCurWeapon.AddDynamic(this, &ThisClass::OnRepCurWeaponEvent);
	CombatComponent->OnRepWeaponSlot.AddDynamic(this, &ThisClass::OnRepWeaponSlotEvent);

	Fp_Root->SetupAttachment(GetCapsuleComponent());

	Fp_SpringArm->SetupAttachment(Fp_Root);
	Fp_SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 75.f));
	Fp_SpringArm->bUsePawnControlRotation = true;
	Fp_SpringArm->bInheritPitch = true;
	Fp_SpringArm->bInheritRoll = true;
	Fp_SpringArm->bInheritYaw = true;
	Fp_SpringArm->bDoCollisionTest = false;
	Fp_SpringArm->TargetArmLength = 0.f;

	Camera->SetupAttachment(Fp_SpringArm);
	Camera->bUsePawnControlRotation = true;
	Camera->SetFieldOfView(110.f);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	CreatureMovementComponent->SetIsReplicated(true);

	GetCharacterMovement()->AirControl = 0.25f;
	GetCharacterMovement()->MaxWalkSpeed = CreatureMovementComponent->GetWalkSpeed();
	GetCharacterMovement()->MaxWalkSpeedCrouched = CreatureMovementComponent->GetCrouchWalkSpeed();
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->SetWalkableFloorAngle(50.f);

}	


void ACreature::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

float ACreature::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	float Damage = CombatComponent->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

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

void ACreature::EquipEvent(AWeapon* Weapon)
{
	if(!IsValid(Weapon))
		return;

	if (IsValid(Weapon->GetWeaponMesh()) == false || IsValid(GetMesh()) == false)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([=](){ EquipEvent(Weapon); });
		return;
	}

	Weapon->GetWeaponMesh()->SetSimulatePhysics(false);
	GetWorld()->GetTimerManager().SetTimerForNextTick([=]() 
	{ 	
			Weapon->GetWeaponMesh()->AttachToComponent
			(
				GetMesh(),
				FAttachmentTransformRules::SnapToTargetIncludingScale,
				Weapon->GetTppHandSocketName()
			);
	}
	);

}

void ACreature::UnEquipEvent(AWeapon* Weapon)
{
	if (!IsValid(Weapon))
		return;

	SetWeaponVisibility(Weapon, true);
	Weapon->GetWeaponMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void ACreature::HolsterEvent(AWeapon* Weapon)
{
	if (!IsValid(Weapon))
		return;

	SetWeaponVisibility(Weapon, false);
}

void ACreature::UnholsterEvent(AWeapon* Weapon)
{
	if (!IsValid(Weapon))
		return;

	SetWeaponVisibility(Weapon, true);
}

void ACreature::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void ACreature::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ACreature::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
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

	StartingAimRotation = GetActorRotation();
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

void ACreature::Jump()
{
	Super::Jump();

	Server_Jump();
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
	Camera->AttachToComponent(Fp_SpringArm, FAttachmentTransformRules::SnapToTargetIncludingScale);

	FTransform HeadTransform = GetMesh()->GetSocketTransform("head", ERelativeTransformSpace::RTS_World);
	FTransform CameraTransform = Camera->GetComponentTransform();
	FTransform RelativeTransform = HeadTransform.GetRelativeTransform(CameraTransform);
	FVector RelativeLocation = RelativeTransform.GetLocation();
	Camera->SetRelativeLocation(RelativeLocation);

	TargetCameraRelativeLocation = FVector::ZeroVector;
}

bool ACreature::CanAttack()
{
	return true;
}

bool ACreature::CanRun()
{
	if(CombatComponent->GetCurWeapon())
	{
		if (bAttackButtonPress && CombatComponent->GetCurWeapon()->GetbAttackWhileSprint() == false)
			return false;

		if (bSubAttackButtonPress && CombatComponent->GetCurWeapon()->GetbSubAttackWhileSprint() == false)
			return false;

		UWeaponReloadComponent* ReloadComp = CombatComponent->GetCurWeapon()->GetComponentByClass<UWeaponReloadComponent>();
		if (ReloadComp && GetMesh()->GetAnimInstance()->Montage_IsPlaying(ReloadComp->GetReloadMontageTpp()))
			return false;
	}

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

void ACreature::SetWeaponVisibility(class AWeapon* Weapon, bool Value)
{
	for (auto& Tuple : Weapon->GetTppWeaponPrimitiveInfo())
	{
		Tuple.Value->SetVisibility(Value);
	}
}

void ACreature::MoveForward(float NewAxisValue)
{
	CurrentInputForward = NewAxisValue;
	
	CreatureMovementComponent->MoveForward(NewAxisValue);
}

void ACreature::MoveRight(float NewAxisValue)
{
	CurrentInputRight = NewAxisValue;

	CreatureMovementComponent->MoveRight(NewAxisValue);
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
	if(!CombatComponent->GetCurWeapon())
		return;
	
	if(CreatureMovementComponent->GetMovementState() == EMovementState::SPRINT)
	{
		UWeaponReloadComponent* ReloadComp = CombatComponent->GetCurWeapon()->GetComponentByClass<UWeaponReloadComponent>();
		if (ReloadComp && !ReloadComp->GetbReloadWhileSprint())
			CreatureMovementComponent->SetMovementState(EMovementState::WALK);
	}

	CombatComponent->GetCurWeapon()->OnSkillFirst.Broadcast();
}

void ACreature::AttackButtonPressed()
{
	bAttackButtonPress = true;
	AttackStart();
}

void ACreature::AttackButtonReleased()
{
	bAttackButtonPress = false;
	AttackEnd();
}

void ACreature::SubAttackButtonPressed()
{
	bSubAttackButtonPress = true;
	SubAttackStart();
}

void ACreature::SubAttackButtonReleased()
{
	bSubAttackButtonPress = false;
	SubAttackEnd();
}

void ACreature::RunButtonPressed()
{
	bRunButtonPress = true;
}

void ACreature::RunButtonReleased()
{
	bRunButtonPress = false;
}

void ACreature::CrouchButtonPressed()
{
	bCrouchButtonPress = true;
	CreatureMovementComponent->Crouch();
}

void ACreature::CrouchButtonReleased()
{
	bCrouchButtonPress = false;
	CreatureMovementComponent->UnCrouch();
}

void ACreature::JumpingButtonPressed()
{
	bJumpButtonPress = true;

	CreatureMovementComponent->JumpStart();
}

void ACreature::JumpingButtonReleased()
{
	bJumpButtonPress = false;

	CreatureMovementComponent->JumpEnd();
}

void ACreature::AimOffset(float DeltaTime)
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();
	if (Speed == 0.f && !bIsInAir) // standing still, not jumping
	{
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = (DeltaAimRotation.Yaw);
		bUseControllerRotationYaw = false;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) // running, or jumping
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = (0.f);
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
	AO_Pitch = (GetBaseAimRotation().Pitch);
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = (FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch));
	}
}

void ACreature::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) <= 0.2f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void ACreature::OnClientEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	if (Weapon == nullptr)
		return;

	EquipEvent(Weapon);
}

void ACreature::OnServerEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	Weapon->SetOwner(this);
	EquipEvent(Weapon);
}

void ACreature::OnServerUnEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	UnEquipEvent(Weapon);
}

void ACreature::OnClientUnEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	UnEquipEvent(Weapon);
}

void ACreature::OnClientUnholsterWeaponEvent(AWeapon* Weapon)
{
	UnholsterEvent(Weapon);

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

void ACreature::OnServerUnholsterWeaponEvent(AWeapon* Weapon)
{
	UnholsterEvent(Weapon);
}

void ACreature::OnClientHolsterWeaponEvent(AWeapon* Weapon)
{
	if(!Weapon)
		return;

	HolsterEvent(Weapon);

	Weapon->OnPlayTppMontage.RemoveDynamic(this, &ACreature::OnPlayWeaponTppMontageEvent);
	Weapon->OnPlayFppMontage.RemoveDynamic(this, &ACreature::OnPlayWeaponFppMontageEvent);

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
	HolsterEvent(Weapon);
}

void ACreature::OnClientDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	DisableInput(GetController<APlayerController>());
	if (CombatComponent->GetCurWeapon() != nullptr)
		CombatComponent->HolsterWeapon(CombatComponent->GetCurWeapon());

	GetCapsuleComponent()->SetCollisionProfileName(FName("DeadPawn"));
}

void ACreature::OnServerDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	UKraverDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UKraverDamageType>();

	if (DamageType->bCanSimulate)
	{
		SimulateMesh();
	}

	Multicast_OnDeathEvent(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ACreature::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	CreatureMovementComponent->Landed(Hit);

	PlayLandedMontage();

	OnLand.Broadcast(Hit);
}

void ACreature::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	OnCrouchStart.Broadcast(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ACreature::OnEndCrouch(float HeightAdjust, float ScaledHeightAdjust)
{
	Super::OnEndCrouch(HeightAdjust, ScaledHeightAdjust);

	OnCrouchEnd.Broadcast(HeightAdjust, ScaledHeightAdjust);
}

void ACreature::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();

	OnJump.Broadcast();
}

void ACreature::OnClientAfterTakePointDamageEvent(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	Server_OnAfterTakePointDamageEvent(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ACreature::OnClientAfterTakeRadialDamageEvent(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	Server_OnAfterTakeRadialDamageEvent(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

}

void ACreature::OnPlayWeaponTppMontageEvent(UAnimMontage* PlayedMontage, float Speed)
{
	Server_OnPlayWeaponTppMontageEvent(PlayedMontage, Speed);
}

void ACreature::OnPlayWeaponFppMontageEvent(UAnimMontage* PlayedMontage, float Speed)
{
	Server_OnPlayWeaponFppMontageEvent(PlayedMontage, Speed);
}

void ACreature::OnRepCurWeaponEvent(AWeapon* PrevWeapon, AWeapon* CurWeapon)
{
	if(IsLocallyControlled())
		return;

	if(PrevWeapon)
		HolsterEvent(PrevWeapon);

	if (CurWeapon)
		UnholsterEvent(CurWeapon);
}

void ACreature::OnRepWeaponSlotEvent(const TArray<AWeapon*>& PrevWeaponSlot, const TArray<AWeapon*>& CurWeaponSlot)
{
	if (IsLocallyControlled())
		return;

	for (int i = 0; i < CurWeaponSlot.Num(); i++)
	{
		if (CurWeaponSlot[i] && !PrevWeaponSlot[i]) // 장착한 경우
		{
			EquipEvent(CurWeaponSlot[i]);
		}
		else if (!CurWeaponSlot[i] && PrevWeaponSlot[i]) // 장착해제한 경우
		{
			UnEquipEvent(PrevWeaponSlot[i]);
		}
	}
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

void ACreature::Multicast_OnDeathEvent_Implementation(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UKraverDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UKraverDamageType>();

	GetCapsuleComponent()->SetCollisionProfileName(FName("DeadPawn"));

}

void ACreature::Server_OnAfterTakePointDamageEvent_Implementation(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Multicast_OnAfterTakePointDamageEvent(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ACreature::Multicast_OnAfterTakePointDamageEvent_Implementation(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (GetMesh()->IsSimulatingPhysics())
	{
		UKraverDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UKraverDamageType>();
		if (!DamageType)
		{
			KR_LOG(Error, TEXT("Damage Type is not UKraverDamageType Class"));
			return;
		}

		GetMesh()->AddImpulseAtLocation(
			DamageEvent.ShotDirection * DamageType->DamageImpulse * GetMesh()->GetMass() * ImpulseResistanceRatio,
			DamageEvent.HitInfo.ImpactPoint,
			DamageEvent.HitInfo.BoneName
		);
	}
}

void ACreature::Server_OnAfterTakeRadialDamageEvent_Implementation(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Multicast_OnAfterTakeRadialDamageEvent(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ACreature::Multicast_OnAfterTakeRadialDamageEvent_Implementation(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (GetMesh()->IsSimulatingPhysics())
	{
		UKraverDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UKraverDamageType>();
		if (!DamageType)
		{
			KR_LOG(Error, TEXT("Damage Type is not UKraverDamageType Class"));
			return;
		}

		FVector Direction = DamageEvent.ComponentHits[0].ImpactPoint - DamageEvent.Origin;
		Direction.Normalize();

		GetMesh()->AddImpulseAtLocation(
			Direction * DamageType->DamageImpulse * GetMesh()->GetMass() * ImpulseResistanceRatio,
			DamageEvent.ComponentHits[0].ImpactPoint
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

void ACreature::Client_SimulateMesh_Implementation()
{
	GetMesh()->SetSimulatePhysics(true);
}

void ACreature::Multicast_SimulateMesh_Implementation()
{
	GetMesh()->SetSimulatePhysics(true);

	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		for (int i = 0; i < GetMesh()->GetMaterials().Num(); i++)
			GetMesh()->SetMaterial(i, DynamicDissolveMaterialInstance);

		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.f);
	}
	StartDissolve();
}

void ACreature::AttackStart()
{
	if(!CombatComponent->GetCurWeapon())
		return;

	if (CombatComponent->GetCurWeapon()->GetbAttackWhileSprint() == false)
		CreatureMovementComponent->SetMovementState(EMovementState::WALK);

	if(CanAttack())
		CombatComponent->SetIsAttacking(true);
}

void ACreature::AttackEnd()
{
	CombatComponent->SetIsAttacking(false);
}

void ACreature::SubAttackStart()
{
	if (!CombatComponent->GetCurWeapon())
		return;

	if (CombatComponent->GetCurWeapon()->GetbSubAttackWhileSprint() == false)
		CreatureMovementComponent->SetMovementState(EMovementState::WALK);

	CombatComponent->SetIsSubAttacking(true);
}

void ACreature::SubAttackEnd()
{
	CombatComponent->SetIsSubAttacking(false);
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
	if (!IS_SERVER())
	{
		KR_LOG(Error, TEXT("Function Called on client"));
		return;
	}

	AKraverGameMode* GameMode = GetWorld()->GetAuthGameMode<AKraverGameMode>();
	if(GameMode)
	{
		FTimerHandle RespawnTimer;
		GetWorldTimerManager().SetTimer(
			RespawnTimer,
			[=]() { GameMode->RequestRespawn(this, Controller); },
			5.f,
			false,
			5.f
		);
	}
	else
	{
		KR_LOG(Warning, TEXT("GameMode is nullptr"));
	}

	Client_SimulateMesh();
	Multicast_SimulateMesh();
}