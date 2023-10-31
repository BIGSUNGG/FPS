// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature.h"
#include LookCameraWidgetComponent_h
#include CreatureMovementComponent_h
#include WeaponReloadComponent_h
#include CreatureAnimInstance_h
#include KraverGameMode_h
#include HpBarWidget_h
#include Melee_h

// Sets default values
ACreature::ACreature()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Fp_Root = CreateDefaultSubobject<USceneComponent>(TEXT("Fp_Root"));
	Fp_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Fp_SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Fp_Camera"));

	CreatureMovementComponent = CreateDefaultSubobject<UCreatureMovementComponent>("CreatureMovementComponent");
	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	CombatComponent->SetIsReplicated(true);
	HpBarWidget = CreateDefaultSubobject<ULookCameraWidgetComponent>("HpBarWidet");

	CombatComponent->OnClientDeath.AddDynamic(this, &ThisClass::OnClientDeathEvent);
	CombatComponent->OnServerDeath.AddDynamic(this, &ThisClass::OnServerDeathEvent);
	CombatComponent->OnMulticastDeath.AddDynamic(this, &ThisClass::OnMulticastDeathEvent);

	CombatComponent->OnEquipWeaponSuccess.AddDynamic(this, &ThisClass::OnEquipWeaponSuccessEvent);
	CombatComponent->OnServerEquipWeaponSuccess.AddDynamic(this, &ThisClass::OnServerEquipWeaponSuccessEvent);
	CombatComponent->OnUnEquipWeaponSuccess.AddDynamic(this, &ThisClass::OnUnEquipWeaponSuccessEvent);
	CombatComponent->OnServerUnEquipWeaponSuccess.AddDynamic(this, &ThisClass::OnServerUnEquipWeaponSuccessEvent);

	CombatComponent->OnClientAfterTakePointDamageSuccess.AddDynamic(this, &ThisClass::OnClientAfterTakePointDamageEvent);
	CombatComponent->OnClientAfterTakeRadialDamageSuccess.AddDynamic(this, &ThisClass::OnClientAfterTakeRadialDamageEvent);
	CombatComponent->OnServerAfterTakePointDamageSuccess.AddDynamic(this, &ThisClass::OnServerAfterTakePointDamageEvent);
	CombatComponent->OnServerAfterTakeRadialDamageSuccess.AddDynamic(this, &ThisClass::OnServerAfterTakeRadialDamageEvent);
	CombatComponent->OnMulticastAfterTakePointDamageSuccess.AddDynamic(this, &ThisClass::OnMulticastAfterTakePointDamageEvent);
	CombatComponent->OnMulticastAfterTakeRadialDamageSuccess.AddDynamic(this, &ThisClass::OnMulticastAfterTakeRadialDamageEvent);

	CombatComponent->OnUnholsterWeapon.AddDynamic(this, &ThisClass::OnUnholsterWeaponEvent);
	CombatComponent->OnServerUnholsterWeapon.AddDynamic(this, &ThisClass::OnServerUnholsterWeaponEvent);
	CombatComponent->OnHolsterWeapon.AddDynamic(this, &ThisClass::OnHolsterWeaponEvent);
	CombatComponent->OnServerHolsterWeapon.AddDynamic(this, &ThisClass::OnServerHolsterWeaponEvent);

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

	HpBarWidget->SetupAttachment(GetCapsuleComponent());
	HpBarWidget->SetRelativeLocation(FVector(20.f, 0.f, 95.f));
	HpBarWidget->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	HpBarWidget->SetOwnerNoSee(true);
}	


void ACreature::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

float ACreature::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	float Damage = IS_SERVER() ? CombatComponent->OnServer_TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser) : 0.f;
	return Damage;
}

void ACreature::OnServer_Assassinated(ACreature* Attacker, FAssassinateInfo AssassinateInfo)
{
	ERROR_IF_CALLED_ON_CLIENT();

	Client_Assassinated(Attacker, AssassinateInfo);
	Multicast_Assassinated(Attacker, AssassinateInfo);
}

void ACreature::AssassinatedEnd()
{
	Server_OnAssassinatedEndEvent();
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

	// HpBar 할당
	UHpBarWidget* HpBar = Cast<UHpBarWidget>(HpBarWidget->GetWidget());
	if (HpBar)
		HpBar->SetOwnerCreature(this);

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

	if (CombatComponent->GetCurWeapon())
	{
		if (CombatComponent->GetCurWeapon()->GetIsAttacking() && !CanAttack()) // 공격이 불가능할 시 공격 종료
			CombatComponent->SetIsAttacking(false);

		if (CombatComponent->GetCurWeapon()->GetIsSubAttacking() && !CanSubAttack()) // 보조 공격이 불가능할 시 공격 종료
			CombatComponent->SetIsSubAttacking(false);
	}

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
	// DisableInput
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	DisableInput(PlayerController);

	// 카메라 설정
	GetMesh()->SetVisibility(true);
	Camera->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "head");

	// 캐릭터 멈추기
	GetMovementComponent()->Velocity = FVector::ZeroVector;
}

void ACreature::OnAssassinateEndEvent()
{
	// EnableInput
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	EnableInput(PlayerController);

	// 카메라 설정
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
	if (CombatComponent->GetCurWeapon())
	{
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(CombatComponent->GetCurWeapon()->GetHolsterMontageTpp()))
			return false;
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(CombatComponent->GetCurWeapon()->GetUnholsterMontageTpp()))
			return false;
	}

	return true;
}

bool ACreature::CanSubAttack()
{
	if (CombatComponent->GetCurWeapon())
	{
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(CombatComponent->GetCurWeapon()->GetHolsterMontageTpp()))
			return false;
		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(CombatComponent->GetCurWeapon()->GetUnholsterMontageTpp()))
			return false;
	}

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
		if (ReloadComp && ReloadComp->GetbReloadWhileSprint() == false && ReloadComp->IsReloading())
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

	FVector Right = GetControlRotation().Vector().GetSafeNormal2D().RotateAngleAxis(90.0f, FVector::UpVector);
	return FVector::DotProduct(Velocity, Right) / Right.Size2D();
}

float ACreature::CalculateCurrentFloorSlope()
{
	if(GetCharacterMovement()->IsFalling()) // 공중에 있을 경우 바닥이 없으므로 0 반환
		return 0.f;

	// 바닥 오브젝트 구하기
	FHitResult HitResult;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 60.f);
	FCollisionQueryParams QueryParams;
	GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_ONLY_OBJECT, QueryParams);

	if (HitResult.IsValidBlockingHit())
	{
		// 바닥 각도 구하기
		FVector FloorNormal = HitResult.Normal;
		float FloorSlope = FMath::Acos(FloorNormal.Z) * 180.0f / PI; // 각도 변환
		return FloorSlope;
	}

	return 0.0f;
}

FVector ACreature::CaclulateCurrentFllorSlopeVector()
{
	UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement();

	if(CharacterMovementComp->IsFalling()) // 공중에 있을 경우 바닥이 없으므로 ZeroVector 반환
		return FVector::ZeroVector;

	// 바닥 오브젝트 구하기
	if (CharacterMovementComp)
	{
		FHitResult HitResult;
		FVector StartLocation = GetActorLocation();
		FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 60.f);
		FCollisionQueryParams QueryParams;
		QueryParams.bFindInitialOverlaps = true;

		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_ONLY_OBJECT, QueryParams))
			return HitResult.ImpactNormal;

	}
	return FVector::ZeroVector;
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
	if (Speed == 0.f && !bIsInAir) // 가만히 있을 경우
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

void ACreature::OnEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	if (!IsValid(Weapon))
		return;

	if (IsValid(Weapon->GetTppWeaponMesh()) == false || IsValid(GetMesh()) == false) // 무기가 아직 생성이 안되었는지 확인
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([=]() { OnEquipWeaponSuccessEvent(Weapon); });
		return;
	}

	// 무기의 TppMesh를 3인칭 메쉬에 장착
	GetWorld()->GetTimerManager().SetTimerForNextTick([=]()
		{
			Weapon->GetTppWeaponMesh()->AttachToComponent
			(
				GetMesh(),
				FAttachmentTransformRules::SnapToTargetIncludingScale,
				Weapon->GetTppHandSocketName()
			);
		}
	);
}

void ACreature::OnServerEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	Weapon->SetOwner(this);
}

void ACreature::OnServerUnEquipWeaponSuccessEvent(AWeapon* Weapon)
{
}

void ACreature::OnUnEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	if (!IsValid(Weapon))
		return;

	Weapon->GetTppWeaponMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void ACreature::OnUnholsterWeaponEvent(AWeapon* Weapon)
{
	if (!IsValid(Weapon))
		return;

	// 무기의 델리게이트에 함수 등록
	Weapon->OnPlayTppMontage.AddDynamic(this, &ACreature::OnPlayWeaponTppMontageEvent);
	Weapon->OnPlayFppMontage.AddDynamic(this, &ACreature::OnPlayWeaponFppMontageEvent);

	if(IsLocallyControlled())
	{
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
}

void ACreature::OnServerUnholsterWeaponEvent(AWeapon* Weapon)
{
}

void ACreature::OnHolsterWeaponEvent(AWeapon* Weapon)
{
	if (!IsValid(Weapon))
		return;

	// 무기의 델리게이트에 함수 제거
	Weapon->OnPlayTppMontage.RemoveDynamic(this, &ACreature::OnPlayWeaponTppMontageEvent);
	Weapon->OnPlayFppMontage.RemoveDynamic(this, &ACreature::OnPlayWeaponFppMontageEvent);
	GetMesh()->GetAnimInstance()->Montage_Stop(0.f, Weapon->GetAttackMontageTpp());

	if (IsLocallyControlled())
	{
	
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
}

void ACreature::OnServerHolsterWeaponEvent(AWeapon* Weapon)
{
}

void ACreature::OnClientDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	DisableInput(GetController<APlayerController>());
	CombatComponent->HolsterWeapon();

	GetCapsuleComponent()->SetCollisionProfileName(FName("DeadPawn"));
}

void ACreature::OnServerDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	UKraverDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UKraverDamageType>();

	if (DamageType->bCanSimulate)
	{
		OnServer_SimulateMesh();
	}

}

void ACreature::OnMulticastDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	UKraverDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UKraverDamageType>();

	GetCapsuleComponent()->SetCollisionProfileName(FName("DeadPawn"));

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
}

void ACreature::OnClientAfterTakeRadialDamageEvent(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
}

void ACreature::OnServerAfterTakePointDamageEvent(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
}

void ACreature::OnMulticastAfterTakePointDamageEvent(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
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

void ACreature::OnServerAfterTakeRadialDamageEvent(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
}

void ACreature::OnMulticastAfterTakeRadialDamageEvent(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
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

void ACreature::OnPlayWeaponTppMontageEvent(UAnimMontage* PlayedMontage, float Speed)
{
	GetMesh()->GetAnimInstance()->Montage_Play(PlayedMontage, Speed);
}

void ACreature::OnPlayWeaponFppMontageEvent(UAnimMontage* PlayedMontage, float Speed)
{
}

void ACreature::Server_OwnOtherActor_Implementation(AActor* Actor)
{
	Actor->SetOwner(this);
}

void ACreature::Multicast_Assassinated_Implementation(ACreature* Attacker, FAssassinateInfo AssassinateInfo)
{
	SetActorLocationAndRotation
	(
		Attacker->GetActorLocation() + Attacker->GetActorForwardVector() * 100,
		Attacker->GetActorRotation()
	);

	GetMesh()->GetAnimInstance()->Montage_Play(AssassinateInfo.AssassinatedMontagesTpp);
}

void ACreature::Client_Assassinated_Implementation(ACreature* Attacker, FAssassinateInfo AssassinateInfo)
{
	// DisableInput
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if(PlayerController)
		DisableInput(PlayerController);

	// 3인칭 메쉬 보이기
	GetMesh()->SetOwnerNoSee(false);
	GetMesh()->SetOnlyOwnerSee(false);
	GetMesh()->SetVisibility(true);
	GetMesh()->SetHiddenInGame(false);

	// 카메라 설정
	FAttachmentTransformRules TransformRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	TransformRules.bWeldSimulatedBodies = true;
	TransformRules.RotationRule = EAttachmentRule::KeepWorld;
	Camera->AttachToComponent(GetMesh(), TransformRules, "head");
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

void ACreature::Client_SimulateMesh_Implementation()
{
	GetMesh()->SetSimulatePhysics(true);
}

void ACreature::Multicast_SimulateMesh_Implementation()
{
	KR_LOG(Log, TEXT("Simulate mesh"));

	// 3인칭 메쉬보이기
	GetMesh()->SetSimulatePhysics(true);

	// HpBar 숨기기
	HpBarWidget->SetVisibility(false);

	// 3인칭 메쉬 Dissolve 실행
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
	OnServer_SimulateMesh();
}

void ACreature::PlayLandedMontage()
{
	UCreatureAnimInstance* CreatureAnim = Cast<UCreatureAnimInstance>(GetMesh()->GetAnimInstance());
	GetMesh()->GetAnimInstance()->Montage_Play(CreatureAnim->GetLandedMontage());
}

void ACreature::OnServer_SimulateMesh()
{
	if (!IS_SERVER())
	{
		KR_LOG(Error, TEXT("Function Called on client"));
		return;
	}

	Client_SimulateMesh();
	Multicast_SimulateMesh();
}