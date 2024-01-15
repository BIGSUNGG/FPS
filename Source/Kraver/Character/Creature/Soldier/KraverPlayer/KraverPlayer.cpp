// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayer.h"
#include AttachmentMagazineComponent_h
#include AdvanceMovementComponent_h
#include AttachmentScopeComponent_h
#include KraverPlayerController_h
#include WeaponReloadComponent_h
#include CreatureAnimInstance_h
#include SoldierAnimInstance_h
#include WeaponAdsComponent_h
#include KraverGameMode_h
#include KraverHud_h
#include Weapon_h

AKraverPlayer::AKraverPlayer() : Super()
{
	NetUpdateFrequency = 300.f;

	static ConstructorHelpers::FObjectFinder<ULevelSequence> LEVEL_FADE_SEQUENCE(TEXT("/Script/LevelSequence.LevelSequence'/Game/InfimaGames/LowPolyShooterPack/Data/Sequences/LS_Fade_Level.LS_Fade_Level'"));

	if (LEVEL_FADE_SEQUENCE.Succeeded())
		LevelFadeSquence = LEVEL_FADE_SEQUENCE.Object;

	Tp_Root = CreateDefaultSubobject<USceneComponent>(TEXT("Tp_Root"));
	Tp_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Tp_SpringArm"));

	ArmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMesh"));
	ArmMesh->SetupAttachment(Fp_SpringArm);
	ArmMesh->SetRelativeLocation(FVector(0.f,0.f, -160.516068f));
	ArmMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	ArmMesh->SetCastShadow(false);
	ArmMesh->SetOnlyOwnerSee(true);
	ArmMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArmMesh->SetScalarParameterValueOnMaterials("Type",0);
		
	ShowOnlyFirstPerson.Push(ArmMesh);
	ShowOnlyThirdPerson.Push(GetMesh());

	Tp_Root->SetupAttachment(GetCapsuleComponent());

	Tp_SpringArm->SetupAttachment(Tp_Root);
	Tp_SpringArm->bUsePawnControlRotation = true;
	Tp_SpringArm->bInheritPitch = true;
	Tp_SpringArm->bInheritRoll = true;
	Tp_SpringArm->bInheritYaw = true;
	Tp_SpringArm->bDoCollisionTest = false;
	Tp_SpringArm->TargetArmLength = 300.f;

	GetCharacterMovement()->AirControl = 0.5f;
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(60.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

}

void AKraverPlayer::BeginPlay()
{
	Super::BeginPlay();

	CameraBasicFov = Camera->FieldOfView;
	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(GetMesh()->GetAnimInstance());
	if(AnimInstance)
	{
		AnimInstance->OnWeapon_Holster.AddDynamic(this, &ThisClass::OnTp_Weapon_HolsterEvent);
	}
	else
	{
		KR_LOG(Error ,TEXT("AnimInstance is not USoldierAnimInstance class"));
	}

	Fp_SpringArmBasicLocation = Fp_SpringArm->GetRelativeLocation();
	BasicArmLocation = ArmMesh->GetRelativeLocation();
	BasicArmRotation = ArmMesh->GetRelativeRotation();
	RefreshCurViewType();

	if (IsLocallyControlled())
	{
		KraverController = KraverController == nullptr ? Cast<AKraverPlayerController>(Controller) : KraverController;
		if (KraverController)
			HUD = HUD == nullptr ? Cast<AKraverHud>(KraverController->GetHUD()) : HUD;

	}


	if (IsLocallyControlled() && IsPlayerControlled())
	{
		// Start Level Fade
		if (LevelFadeSquence)
		{
			ALevelSequenceActor* OutActor = nullptr;
			ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(this, LevelFadeSquence, FMovieSceneSequencePlaybackSettings(), OutActor);

			//Sequence Play
			if (SequencePlayer)
			{
				SequencePlayer->Play();
			}
		}
	}
}

void AKraverPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	KraverController = KraverController == nullptr ? Cast<AKraverPlayerController>(Controller) : KraverController;
	if (KraverController)
		HUD = HUD == nullptr ? Cast<AKraverHud>(KraverController->GetHUD()) : HUD;

	ClientTick(DeltaTime);
	ServerClientTick(DeltaTime);
	LocallyControlTick(DeltaTime);

	SpringArmTick(DeltaTime);
	WeaponADS(DeltaTime);
	ArmMeshTick(DeltaTime);
}

void AKraverPlayer::CameraTick(float DeletaSeconds)
{
	Super::CameraTick(DeletaSeconds);

	UAdvanceMovementComponent* AdvanceMovementComp = Cast<UAdvanceMovementComponent>(CreatureMovementComponent);

	if(AdvanceMovementComp)
	{
		if (AdvanceMovementComp->GetCurWallRunState() == EWallRunState::WALLRUN_LEFT)
			CameraTilt(15.f);
		else if (AdvanceMovementComp->GetCurWallRunState() == EWallRunState::WALLRUN_RIGHT)
			CameraTilt(-15.f);
		else
			CameraTilt(0.f);
	}
}

void AKraverPlayer::ArmMeshTick(float DeletaTime)
{
	ArmMesh->SetRelativeLocation(BasicArmLocation + AdsArmLocation);
	ArmMesh->SetRelativeRotation(BasicArmRotation);
}

void AKraverPlayer::CameraTilt(float TargetRoll)
{
	if(!Controller)
		return;

	FRotator TargetRotation(Controller->GetControlRotation().Pitch, Controller->GetControlRotation().Yaw, TargetRoll);
	Controller->SetControlRotation(FMath::RInterpTo(Controller->GetControlRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 10.f));

}

void AKraverPlayer::WeaponADS(float DeltaTime)
{
	// 정조준 중일때
	if (CombatComponent->GetCurWeapon() && CombatComponent->GetCurWeapon()->IsSubAttacking() && CombatComponent->GetCurWeapon()->FindComponentByClass<UWeaponAdsComponent>())
	{
		UWeaponAdsComponent* AdsComp = CombatComponent->GetCurWeapon()->FindComponentByClass<UWeaponAdsComponent>();
		USkeletalMeshComponent* ArmWeaponMesh = dynamic_cast<USkeletalMeshComponent*>(CombatComponent->GetCurWeapon()->GetFppWeaponMesh());

#if TEST_ADS
		{
			FTransform WeaponTransform = ArmWeaponMesh->GetSocketTransform("SOCKET_AIM", ERelativeTransformSpace::RTS_World);
			FTransform CameraTransform = Camera->GetComponentTransform();
			FTransform RelativeTransform = WeaponTransform.GetRelativeTransform(CameraTransform);
			FRotator RelativeRotation = RelativeTransform.Rotator();
			FVector RelativeLocation = RelativeTransform.GetLocation();

			KR_LOG_VECTOR(RelativeLocation);
			KR_LOG_ROTATOR(RelativeRotation);
		}
#endif
		// 크로스헤어 끄기
		if (CurViewType == EViewType::FIRST_PERSON && HUD)
			HUD->SetbDrawCrosshair(false);

		// 스코프 조준점에 맞춰 ArmMesh이동
		const TMap<FString, UPrimitiveComponent*>& WeaponPrimitiveInfo = CombatComponent->GetCurWeapon()->GetFppWeaponPrimitiveInfo();
		if (WeaponPrimitiveInfo.Contains("Scope"))
		{
			UStaticMeshComponent* ScopeMesh = Cast<UStaticMeshComponent>(WeaponPrimitiveInfo["Scope"]);
			if (ScopeMesh)
			{
				FTransform WeaponTransform = ArmWeaponMesh->GetSocketTransform("SOCKET_Scope", ERelativeTransformSpace::RTS_World);
				FTransform ScopeTransform = ScopeMesh->GetSocketTransform("SOCKET_Aim", ERelativeTransformSpace::RTS_World);
				FTransform RelativeTransform = ScopeTransform.GetRelativeTransform(WeaponTransform);
				FVector RelativeLocation = RelativeTransform.GetLocation();

				UAttachmentScopeComponent* ScopeComp = CombatComponent->GetCurWeapon()->FindComponentByClass<UAttachmentScopeComponent>();
				AdsArmLocation.Z = FMath::FInterpTo(AdsArmLocation.Z, -RelativeLocation.Z + AdsComp->GetAdsLocation().Z, DeltaTime, 10.f);
			}
			else
				AdsArmLocation.Z = FMath::FInterpTo(AdsArmLocation.Z, 0.f, DeltaTime, 10.f);
		}
		else
			AdsArmLocation.Z = FMath::FInterpTo(AdsArmLocation.Z, 0.f, DeltaTime, 10.f);

		// 카메라 줌인
		Camera->SetFieldOfView(FMath::FInterpTo(Camera->FieldOfView, CameraBasicFov - AdsComp->GetReduceFov(), DeltaTime, AdsComp->GetInterpSpeed()));
	}
	else
	{
		if (HUD)
			HUD->SetbDrawCrosshair(true);

		AdsArmLocation.Z = FMath::FInterpTo(AdsArmLocation.Z, 0.f, DeltaTime, 10.f);
		// 카메라 줌아웃
		Camera->SetFieldOfView(FMath::FInterpTo(Camera->FieldOfView, CameraBasicFov, DeltaTime, 15.f));
	}

	ScopeCaptureComponent->FOVAngle = Camera->FieldOfView;
	FppCaptureComponent->FOVAngle = Camera->FieldOfView;
}

void AKraverPlayer::SpringArmTick(float DeltaTime)
{
	if (GetCharacterMovement()->IsCrouching())
		FP_SpringArmCrouchLocation.Z = FMath::FInterpTo(FP_SpringArmCrouchLocation.Z, CrouchCameraHeight, DeltaTime, 20.f);
	else
		FP_SpringArmCrouchLocation.Z = FMath::FInterpTo(FP_SpringArmCrouchLocation.Z, UnCrouchCameraHeight, DeltaTime, 20.f);

	RefreshSpringArm();
}

void AKraverPlayer::ClientTick(float DeltaTime)
{
	if(IsLocallyControlled())
		return;

}

void AKraverPlayer::ServerClientTick(float DeltaTime)
{
	if(!HasAuthority())
		return;
}

void AKraverPlayer::LocallyControlTick(float DeltaTime)
{
	if(!IsLocallyControlled())
		return;

	CheckCanInteractionWeapon();

}

void AKraverPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction(TEXT("ChangeView"), EInputEvent::IE_Pressed, this, &AKraverPlayer::ChangeView);
	PlayerInputComponent->BindAction(TEXT("Holster"), EInputEvent::IE_Pressed, this, &AKraverPlayer::HolsterButtonPressed);
	PlayerInputComponent->BindAction(TEXT("Equip"), EInputEvent::IE_Pressed, this, &AKraverPlayer::EquipButtonPressed);
	PlayerInputComponent->BindAction(TEXT("UnEquip"), EInputEvent::IE_Pressed, this, &AKraverPlayer::UnEquipButtonPressed);
	PlayerInputComponent->BindAction(TEXT("ChangeWeapon1"), EInputEvent::IE_Pressed, this, &AKraverPlayer::ChangeWeapon1Pressed);
	PlayerInputComponent->BindAction(TEXT("ChangeWeapon2"), EInputEvent::IE_Pressed, this, &AKraverPlayer::ChangeWeapon2Pressed);
	PlayerInputComponent->BindAction(TEXT("ChangeWeapon3"), EInputEvent::IE_Pressed, this, &AKraverPlayer::ChangeWeapon3Pressed);
}

void AKraverPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

USkeletalMeshComponent* AKraverPlayer::GetCurMainMesh()
{
	switch (CurViewType)
	{
	case EViewType::FIRST_PERSON:
		return ArmMesh;
		break;
	case EViewType::THIRD_PERSON:
		return GetMesh();
		break;
	default:
		return nullptr;
		break;
	}
}


void AKraverPlayer::HolsterButtonPressed()
{
	UnholsterIndex = -1;
	HolsterWeapon();
}

void AKraverPlayer::EquipButtonPressed()
{
	if (CanInteractWeapon != nullptr)
	{
		CombatComponent->SetUnholsterWhenEquip(true);
		CombatComponent->EquipWeapon(CanInteractWeapon);
	}
}

void AKraverPlayer::UnEquipButtonPressed()
{
	if (CombatComponent->GetCurWeapon() != nullptr && CombatComponent->CountWeapon() > 1)
		CombatComponent->UnEquipWeapon(CombatComponent->GetCurWeapon());

}

void AKraverPlayer::ChangeWeapon1Pressed()
{
	ChangeWeapon(0);
}

void AKraverPlayer::ChangeWeapon2Pressed()
{
	ChangeWeapon(1);
}

void AKraverPlayer::ChangeWeapon3Pressed()
{
	ChangeWeapon(2);
}

void AKraverPlayer::CheckCanInteractionWeapon()
{
	if (IsLocallyControlled() == false)
		return;

	if(CanInteractWeapon)
	{
		for (auto& Comp : CanInteractWeapon->GetTppWeaponPrimitiveInfo())
		{
			Comp.Value->SetRenderCustomDepth(false);
		}

		CanInteractWeapon = nullptr;
	}

	if (CombatComponent->GetCanEquipWeapon() == false)
	{
		if (HUD)
		{
			HUD->SetInteractWidget(false);
		}
		return;
	}

	float Radius = InteractionRadius;
	float Distance = InteractionDistance + Fp_SpringArm->TargetArmLength;

	if (CanInteractWeapon == nullptr)
	{
		TArray<FHitResult> HitResults;
		FCollisionQueryParams Params(NAME_None, false, this);

		bool bResult = SweepMultiByChannel_ExceptWorldObject(
			GetWorld(),
			HitResults,
			Camera->GetComponentLocation(),
			Camera->GetComponentLocation() + Camera->GetForwardVector() * Distance,
			FQuat::Identity,
			ECC_INTERACTION,
			FCollisionShape::MakeSphere(Radius),
			Params
		);

		if (bResult)
		{
			for (auto& Result : HitResults)
			{
				AWeapon* Weapon = Cast<AWeapon>(Result.GetActor());
				if (Result.bBlockingHit && Weapon && Weapon->CanInteracted())
				{
					CanInteractWeapon = Weapon;
					break;
				}
			}
		}
	}

	if (HUD)
	{
		if (CanInteractWeapon)
		{
			HUD->SetInteractWidget(true);
			for (auto& Comp : CanInteractWeapon->GetTppWeaponPrimitiveInfo())
			{
				Comp.Value->SetRenderCustomDepth(true);
			}
		}
		else
		{
			HUD->SetInteractWidget(false);
		}
	}
}

void AKraverPlayer::ChangeView()
{
	switch (CurViewType)
	{
	case EViewType::FIRST_PERSON: // Set View type to Tpp
		CurViewType = EViewType::THIRD_PERSON;
		Camera->AttachToComponent(Tp_SpringArm, FAttachmentTransformRules::SnapToTargetIncludingScale);
		Camera->SetRelativeLocation(Camera->GetRelativeLocation() + FVector(0.f, 45.f, 62.f));
		if (IsLocallyControlled() && IsPlayerControlled())
			GetMesh()->SetVisibility(true);

		RefreshSpringArm();
		for (auto& TempMesh : ShowOnlyFirstPerson)
		{
			if(TempMesh != nullptr)
				TempMesh->SetOwnerNoSee(true);
		}
		for (auto& TempMesh : ShowOnlyThirdPerson)	
		{
			if (TempMesh != nullptr)
				TempMesh->SetOwnerNoSee(false);
		}
		break;
	case EViewType::THIRD_PERSON: // Set View type to Fpp
		CurViewType = EViewType::FIRST_PERSON;
		Camera->AttachToComponent(Fp_SpringArm, FAttachmentTransformRules::SnapToTargetIncludingScale);
		Camera->SetRelativeLocation(FVector::ZeroVector);
		if (IsLocallyControlled() && IsPlayerControlled())
			GetMesh()->SetVisibility(false);

		RefreshSpringArm();
		for (auto TempMesh : ShowOnlyFirstPerson)
		{
			if (TempMesh != nullptr)
				TempMesh->SetOwnerNoSee(false);
		}
		for (auto TempMesh : ShowOnlyThirdPerson)
		{
			if (TempMesh != nullptr)
				TempMesh->SetOwnerNoSee(true);
		}
		break;
	default:
		break;
	}
}

void AKraverPlayer::ThrowWeapon(AWeapon* Weapon)
{
	UPrimitiveComponent* FppWeaponMesh = Weapon->GetFppWeaponMesh();
	FTransform Transform = FppWeaponMesh->GetComponentTransform();
	FVector Direction = Camera->GetForwardVector();

	GetWorldTimerManager().SetTimerForNextTick
	(
		[=]() {
		Weapon->GetTppWeaponMesh()->SetWorldTransform(Transform, false, nullptr, ETeleportType::TeleportPhysics);
		Weapon->GetTppWeaponMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
		Weapon->GetTppWeaponMesh()->AddImpulse(Direction * WeaponThrowPower * Weapon->GetTppWeaponMesh()->GetMass());
		Weapon->GetTppWeaponMesh()->AddAngularImpulseInDegrees(Direction * WeaponThrowAngularPower, NAME_None, true);
		}
	);


	Server_ThrowWeapon(
		Weapon, 
		Transform,
		Direction
	);
}

void AKraverPlayer::RefreshSpringArm()
{
	Fp_SpringArm->SetRelativeLocation(Fp_SpringArmBasicLocation + FP_SpringArmCrouchLocation);
}

void AKraverPlayer::Server_ThrowWeapon_Implementation(AWeapon* Weapon, FTransform Transform, FVector Direction)
{
	Multicast_ThrowWeapon(
		Weapon,
		Transform,
		Direction
	);
}


void AKraverPlayer::Multicast_ThrowWeapon_Implementation(AWeapon* Weapon, FTransform Transform, FVector Direction)
{
	if (IsLocallyControlled()) return;

	GetWorldTimerManager().SetTimerForNextTick
	(
		[=]() {
			Weapon->GetTppWeaponMesh()->SetWorldTransform(Transform, false, nullptr, ETeleportType::TeleportPhysics);
			Weapon->GetTppWeaponMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
			Weapon->GetTppWeaponMesh()->AddImpulse(Direction * WeaponThrowPower * Weapon->GetTppWeaponMesh()->GetMass());
			Weapon->GetTppWeaponMesh()->AddAngularImpulseInDegrees(Direction * WeaponThrowAngularPower, NAME_None, true);
		}
	);
}

void AKraverPlayer::Server_HolsterWeapon_Implementation()
{
	Multicast_HolsterWeapon();
}


void AKraverPlayer::Multicast_HolsterWeapon_Implementation()
{
	if (!IsLocallyControlled())
		HolsterWeaponEvent();
}


void AKraverPlayer::Server_UnholsterWeapon_Implementation()
{
	Multicast_UnholsterWeapon();
}


void AKraverPlayer::Multicast_UnholsterWeapon_Implementation()
{
	if (!IsLocallyControlled())
		UnholsterWeaponEvent();
}

void AKraverPlayer::Client_Assassinated_Implementation(ACreature* Attacker, FAssassinateInfo AssassinateInfo)
{
	Super::Client_Assassinated_Implementation(Attacker, AssassinateInfo);

	ArmMesh->SetOwnerNoSee(true);

}

void AKraverPlayer::RefreshCurViewType()
{
	EViewType TargetViewType = CurViewType;
	do 
	{
		ChangeView();
	} while (TargetViewType != CurViewType);
}

void AKraverPlayer::OnEquipWeaponSuccessEvent(AWeapon* Weapon)
{	
	if (!IsValid(Weapon))
		return;

	if (IsValid(Weapon->GetTppWeaponMesh()) == false || IsValid(GetMesh()) == false || !IsValid(Weapon->GetFppWeaponMesh()))
	{
		GetWorldTimerManager().SetTimerForNextTick([=]() { OnEquipWeaponSuccessEvent(Weapon); });
		return;
	}

	Super::OnEquipWeaponSuccessEvent(Weapon);

	if(IsLocallyControlled())
	{
		UnholsterWeaponEvent();
		
		Weapon->GetFppWeaponMesh()->AttachToComponent(ArmMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, Weapon->GetFppHandSocketName());
	

		// 1인칭으로만 보이는 컴포넌트 추가
		for(auto& Tuple : Weapon->GetFppWeaponPrimitiveInfo())
		{
			if(!Tuple.Value)
				continue;
	
			ShowOnlyFirstPerson.Push(Tuple.Value);
		}
		
		// 3인칭으로만 보이는 컴포넌트 추가
		for (auto& Tuple : Weapon->GetTppWeaponPrimitiveInfo())
		{
			if (!Tuple.Value)
				continue;
	
			ShowOnlyThirdPerson.Push(Tuple.Value);
		}
	
		RefreshCurViewType();
	}

}

void AKraverPlayer::OnUnEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	if (!IsValid(Weapon))
		return;

	Super::OnUnEquipWeaponSuccessEvent(Weapon);

	if (IsLocallyControlled())
	{
		ThrowWeapon(Weapon);
		ShowOnlyThirdPerson.Remove(Weapon->GetTppWeaponMesh());
		Weapon->GetTppWeaponMesh()->SetOwnerNoSee(false);

		for (auto& Tuple : Weapon->GetFppWeaponPrimitiveInfo())
		{
			ShowOnlyFirstPerson.Remove(Tuple.Value);
		}

		for (auto& Map : Weapon->GetTppWeaponPrimitiveInfo())
		{
			Map.Value->SetOnlyOwnerSee(false);
			Map.Value->SetOwnerNoSee(false);

			ShowOnlyThirdPerson.Remove(Map.Value);
		}

		RefreshCurViewType();

		int TargetIndex = -1;
		for (int i = 0; i < CombatComponent->GetWeaponSlot().Num(); i++)
		{
			if (CombatComponent->GetWeaponSlot()[i] && CombatComponent->GetWeaponSlot()[i] != Weapon)
			{
				TargetIndex = i;
				break;
			}
		}
		if(TargetIndex != -1)
			ChangeWeapon(TargetIndex);
	}
	else
	{
		HolsterWeaponEvent();
	}
}


void AKraverPlayer::OnUnholsterWeaponEvent(AWeapon* Weapon)
{
	if (!IsValid(Weapon))
		return;

	Super::OnUnholsterWeaponEvent(Weapon);
	FppCaptureComponent->ShowOnlyComponent(ArmMesh);
}

void AKraverPlayer::OnHolsterWeaponEvent(AWeapon* Weapon)
{
	if (!IsValid(Weapon))
		return;

	Super::OnHolsterWeaponEvent(Weapon);

}

void AKraverPlayer::Client_SimulateMesh_Implementation()
{
	Super::Client_SimulateMesh_Implementation();

	GetMesh()->SetOwnerNoSee(false);
	GetMesh()->SetOnlyOwnerSee(false);
	GetMesh()->SetVisibility(true);
	GetMesh()->SetHiddenInGame(false);
	ArmMesh->SetOwnerNoSee(true);

	FAttachmentTransformRules TransformRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	TransformRules.bWeldSimulatedBodies = true;
	TransformRules.RotationRule = EAttachmentRule::KeepWorld;
	Camera->AttachToComponent(GetMesh(), TransformRules, "head");
}

void AKraverPlayer::OnAssassinateEvent(AActor* AssassinatedActor)
{
	Super::OnAssassinateEvent(AssassinatedActor);

	GetMesh()->SetOwnerNoSee(false);
	ArmMesh->SetOwnerNoSee(true);

	CombatComponent->GetCurWeapon()->GetTppWeaponMesh()->SetOwnerNoSee(false);
	CombatComponent->GetCurWeapon()->GetFppWeaponMesh()->SetOwnerNoSee(true);

}

void AKraverPlayer::OnAssassinateEndEvent()
{
	Super::OnAssassinateEndEvent();

	ArmMesh->GetAnimInstance()->Montage_Stop(0.f);
	RefreshCurViewType();

}

void AKraverPlayer::OnPlayWeaponFppMontageEvent(UAnimMontage* PlayedMontage, float Speed)
{
	Super::OnPlayWeaponFppMontageEvent(PlayedMontage, Speed);

	ArmMesh->GetAnimInstance()->Montage_Play(PlayedMontage, Speed);
}

void AKraverPlayer::OnReload_Grab_MagazineEvent()
{
	Super::OnReload_Grab_MagazineEvent();

	if (CombatComponent->GetCurWeapon() && CombatComponent->GetCurWeapon()->GetFppWeaponPrimitiveInfo().Contains("Magazine"))
	{
		UPrimitiveComponent* MagazineComp = CombatComponent->GetCurWeapon()->GetFppWeaponPrimitiveInfo()["Magazine"];
		UWeaponReloadComponent* ReloadComp = CombatComponent->GetCurWeapon()->FindComponentByClass<UWeaponReloadComponent>();

		MagazineComp->AttachToComponent(ArmMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, "SOCKET_Magazine");
		MagazineComp->SetRelativeLocation(ReloadComp->GetGrabRelativeLocation());
		MagazineComp->SetRelativeRotation(ReloadComp->GetGrabRelativeRotation());
	}
}

void AKraverPlayer::OnReload_Insert_MagazineEvent()
{
	Super::OnReload_Insert_MagazineEvent();

	if (CombatComponent->GetCurWeapon() && CombatComponent->GetCurWeapon()->GetFppWeaponPrimitiveInfo().Contains("Magazine"))
	{
		UAttachmentMagazineComponent* MagazineComp = CombatComponent->GetCurWeapon()->FindComponentByClass<UAttachmentMagazineComponent>();
		CombatComponent->GetCurWeapon()->GetFppWeaponPrimitiveInfo()["Magazine"]->AttachToComponent(CombatComponent->GetCurWeapon()->GetFppWeaponMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, MagazineComp->GetMagazineSocketName());
	}
}

void AKraverPlayer::OnTp_Weapon_HolsterEvent()
{
	if (!IsLocallyControlled()) return;

	CombatComponent->HolsterWeapon();

	if(UnholsterIndex == -1)
		return;

	GetWorldTimerManager().SetTimerForNextTick([=]() 
	{
		UnholsterWeapon();
		UnholsterIndex = -1;
	});
}

void AKraverPlayer::PlayLandedMontage()
{
	Super::PlayLandedMontage();

	UCreatureAnimInstance* CreatureAnim = Cast<UCreatureAnimInstance>(ArmMesh->GetAnimInstance());
	CreatureAnim->PlayLandedMontage();
}

void AKraverPlayer::ChangeWeapon(int8 Index)
{
	if (CombatComponent->GetWeaponSlot()[Index] == CombatComponent->GetCurWeapon())
	{
		KR_LOG(Log,TEXT("Trying to change weapon to cur weapon"));
		return;
	}

	if (!CombatComponent->GetWeaponSlot()[Index])
		return;

	KR_LOG(Log, TEXT("Trying to change weapon to %d weapon"), Index);
	UnholsterIndex = CombatComponent->GetWeaponSlot()[Index] ? Index : -1;

	if (CombatComponent->GetCurWeapon())
		HolsterWeapon();
	else
		UnholsterWeapon();

}


void AKraverPlayer::HolsterWeapon()
{
	HolsterWeaponEvent();
	Server_HolsterWeapon();
}

void AKraverPlayer::HolsterWeaponEvent()
{
	if (!CombatComponent->GetCurWeapon())
	{
		KR_LOG(Warning, TEXT("Cur Wep is null"));
		return;
	}

	Cast<USoldierAnimInstance>(ArmMesh->GetAnimInstance())->PlayHolsteWeaponrMontage();
	Cast<USoldierAnimInstance>(GetMesh()->GetAnimInstance())->PlayHolsteWeaponrMontage();
}


void AKraverPlayer::UnholsterWeapon()
{
	if(UnholsterIndex != -1)
		CombatComponent->UnholsterWeapon(UnholsterIndex);

	UnholsterWeaponEvent();
	Server_UnholsterWeapon();
}

void AKraverPlayer::UnholsterWeaponEvent()
{
	Cast<USoldierAnimInstance>(ArmMesh->GetAnimInstance())->PlayUnholsteWeaponrMontage();
	Cast<USoldierAnimInstance>(GetMesh()->GetAnimInstance())->PlayUnholsteWeaponrMontage();
}
