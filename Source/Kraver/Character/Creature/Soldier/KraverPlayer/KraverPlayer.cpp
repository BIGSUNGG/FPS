// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayer.h"
#include "Kraver/Actor/Weapon/Weapon.h"
#include "Kraver/Actor/Weapon/Gun/Gun.h"
#include "Kraver/Ui/HUD/KraverHUD.h"
#include "Kraver/PlayerController/KraverPlayerController.h"
#include "Kraver/GameMode/KraverGameMode.h"
#include "Kraver/Animation/Creature/CreatureAnimInstance.h"
#include "Kraver/Component/Movement/Advance/AdvanceMovementComponent.h"
#include "Kraver/Animation/Creature/Soldier/SoldierAnimInstance.h"
#include "Kraver/Component/Attachment/Weapon/Magazine/AttachmentMagazineComponent.h"
#include "Kraver/SubSystem/DamageIndicatorSubsystem.h"
#include "Kraver/Component/Skill/Weapon/WeaponReload/WeaponReloadComponent.h"

AKraverPlayer::AKraverPlayer() : ASoldier()
{
	NetUpdateFrequency = 300.f;

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
	Tp_SpringArm->SetRelativeLocation(FVector(0.f, 45.f, 62.f));
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
	ASoldier::BeginPlay();

	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(ArmMesh->GetAnimInstance());
	AnimInstance->OnReload_Grab_Magazine.AddDynamic(this, &AKraverPlayer::OnFP_Reload_Grab_MagazineEvent);
	AnimInstance->OnReload_Insert_Magazine.AddDynamic(this, &AKraverPlayer::OnFP_Reload_Insert_MagazineEvent);

	Fp_SpringArmBasicLocation = Fp_SpringArm->GetRelativeLocation();
	BasicArmLocation = ArmMesh->GetRelativeLocation();
	BasicArmRotation = ArmMesh->GetRelativeRotation();

	if (Controller == GetWorld()->GetFirstPlayerController())
	{
		GetGameInstance()->GetSubsystem<UDamageIndicatorSubsystem>()->OnLocalPlayerBeginPlay(this);

		CombatComponent->SetMaxWeaponSlot(3);

		KraverController = KraverController == nullptr ? Cast<AKraverPlayerController>(Controller) : KraverController;
		if (KraverController)
			HUD = HUD == nullptr ? Cast<AKraverHUD>(KraverController->GetHUD()) : HUD;
		RefreshCurViewType();
	}
}

void AKraverPlayer::Tick(float DeltaTime)
{
	ASoldier::Tick(DeltaTime);

	KraverController = KraverController == nullptr ? Cast<AKraverPlayerController>(Controller) : KraverController;
	if (KraverController)
		HUD = HUD == nullptr ? Cast<AKraverHUD>(KraverController->GetHUD()) : HUD;

	if (IsLocallyControlled() == false)
		Camera->SetRelativeRotation(FRotator(AO_Pitch, AO_Yaw, 0.0f));

	ClientTick(DeltaTime);
	ServerClientTick(DeltaTime);
	LocallyControlTick(DeltaTime);
}

void AKraverPlayer::CameraTick(float DeletaSeconds)
{
	ASoldier::CameraTick(DeletaSeconds);
	
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
	
	WeaponADS(DeltaTime);

	SpringArmTick(DeltaTime);
	ArmMeshTick(DeltaTime);
}

void AKraverPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	ASoldier::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction(TEXT("ChangeView"), EInputEvent::IE_Pressed, this, &AKraverPlayer::ChangeView);
	PlayerInputComponent->BindAction(TEXT("Equip"), EInputEvent::IE_Pressed, this, &AKraverPlayer::EquipButtonPressed);
	PlayerInputComponent->BindAction(TEXT("UnEquip"), EInputEvent::IE_Pressed, this, &AKraverPlayer::UnEquipButtonPressed);
	PlayerInputComponent->BindAction(TEXT("ChangeWeapon1"), EInputEvent::IE_Pressed, this, &AKraverPlayer::ChangeWeapon1Pressed);
	PlayerInputComponent->BindAction(TEXT("ChangeWeapon2"), EInputEvent::IE_Pressed, this, &AKraverPlayer::ChangeWeapon2Pressed);
	PlayerInputComponent->BindAction(TEXT("ChangeWeapon3"), EInputEvent::IE_Pressed, this, &AKraverPlayer::ChangeWeapon3Pressed);
}

void AKraverPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	ASoldier::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AKraverPlayer, ViewType);

}

bool AKraverPlayer::GetCanAttack()
{
	return true;
}

USkeletalMeshComponent* AKraverPlayer::GetCurMainMesh()
{
	switch (ViewType)
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

void AKraverPlayer::SetWeaponVisibility(class AWeapon* Weapon, bool Value)
{
	Super::SetWeaponVisibility(Weapon, Value);

	TMap<FString, UPrimitiveComponent*>** Map = ArmWeaponMeshes.Find(Weapon);
	if(!Map)
		return;

	TMap<FString, UPrimitiveComponent*>& WeaponPrimitiveInfo = **(Map);
	for (auto& Tuple : WeaponPrimitiveInfo)
		Tuple.Value->SetVisibility(Value);
}

void AKraverPlayer::SetViewType(EViewType Type)
{
	ViewType = Type;
	Server_SetViewType(Type);
}

void AKraverPlayer::Server_SetViewType_Implementation(EViewType Type)
{
	ViewType = Type;
}

void AKraverPlayer::EquipButtonPressed()
{
	if(CanInteractWeapon != nullptr)
		CombatComponent->EquipWeapon(CanInteractWeapon);
}

void AKraverPlayer::UnEquipButtonPressed()
{
	if (CombatComponent->GetCurWeapon() != nullptr)
		CombatComponent->UnEquipWeapon(CombatComponent->GetCurWeapon());

}

void AKraverPlayer::ChangeWeapon1Pressed()
{
	CombatComponent->HoldWeapon(0);
}

void AKraverPlayer::ChangeWeapon2Pressed()
{
	CombatComponent->HoldWeapon(1);
}

void AKraverPlayer::ChangeWeapon3Pressed()
{
	CombatComponent->HoldWeapon(2);
}

void AKraverPlayer::CheckCanInteractionWeapon()
{
	if (IsLocallyControlled() == false)
		return;

	CanInteractWeapon = nullptr;
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

	{
		TArray<FHitResult> HitResults;
		FCollisionQueryParams Params(NAME_None, false, this);

		bool bResult = LineTraceMultiByChannel_ExceptWorldObject(
			GetWorld(),
			HitResults,
			Camera->GetComponentLocation(),
			Camera->GetComponentLocation() + Camera->GetForwardVector() * Distance,
			ECC_INTERACTION,
			Params
		);

		if (bResult)
		{
			for (auto& Result : HitResults)
			{
				AWeapon* Weapon = Cast<AWeapon>(Result.GetActor());
				if (Result.bBlockingHit && Weapon && Weapon->GetCanInteracted())
				{
					CanInteractWeapon = Weapon;
					break;
				}
			}
		}
	}

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
				if (Result.bBlockingHit && Weapon && Weapon->GetCanInteracted())
				{
					CanInteractWeapon = Weapon;
					break;
				}
			}
		}
	}

	if (CanInteractWeapon && GEngine)
	{
		FString Text = "CanInteractWeapon : " + CanInteractWeapon->GetName();
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, Text);
	}

	if (HUD)
	{
		if (CanInteractWeapon)
		{
			HUD->SetInteractWidget(true);
		}
		else
		{
			HUD->SetInteractWidget(false);
		}
	}
}

void AKraverPlayer::ChangeView()
{
	switch (ViewType)
	{
	case EViewType::FIRST_PERSON:
		SetViewType(EViewType::THIRD_PERSON);
		Camera->AttachToComponent(Tp_SpringArm, FAttachmentTransformRules::SnapToTargetIncludingScale);

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
	case EViewType::THIRD_PERSON:
		SetViewType(EViewType::FIRST_PERSON);
		Camera->AttachToComponent(Fp_SpringArm, FAttachmentTransformRules::SnapToTargetIncludingScale);

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
	if(!ArmWeaponMeshes.Contains(Weapon))
		return;

	if(!ArmWeaponMeshes[Weapon]->Contains("Root"))
		return;

	UPrimitiveComponent* ArmWeaponMesh = ArmWeaponMeshes[Weapon]->operator[]("Root");

	Server_ThrowWeapon(
		Weapon, 
		ArmWeaponMesh->GetComponentTransform(),
		Camera->GetForwardVector()
		);
}

void AKraverPlayer::RefreshArm()
{
}

void AKraverPlayer::RefreshSpringArm()
{
	Fp_SpringArm->SetRelativeLocation(Fp_SpringArmBasicLocation + FP_SpringArmCrouchLocation);
	Server_RefreshSpringArm(Fp_SpringArmBasicLocation + FP_SpringArmCrouchLocation, Fp_SpringArm->TargetArmLength);
}

void AKraverPlayer::Server_RefreshSpringArm_Implementation(FVector Vector, float Length)
{
	Multicast_RefreshSpringArm(Vector,Length);
}

void AKraverPlayer::Multicast_RefreshSpringArm_Implementation(FVector Vector, float Length)
{
	Fp_SpringArm->SetRelativeLocation(Vector);
	Fp_SpringArm->TargetArmLength = Length;
}

void AKraverPlayer::Server_ThrowWeapon_Implementation(AWeapon* Weapon, FTransform Transform, FVector Direction)
{
	Multicast_ThrowWeapon(Weapon, Transform, Direction);
}

void AKraverPlayer::Multicast_ThrowWeapon_Implementation(AWeapon* Weapon, FTransform Transform, FVector Direction)
{
	Weapon->GetWeaponMesh()->SetWorldTransform(Transform, false, nullptr, ETeleportType::TeleportPhysics);
	Weapon->GetWeaponMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
	Weapon->GetWeaponMesh()->AddImpulse(Direction * WeaponThrowPower * Weapon->GetWeaponMesh()->GetMass());
	Weapon->GetWeaponMesh()->AddAngularImpulseInDegrees(Direction * WeaponThrowAngularPower, NAME_None, true);
}

void AKraverPlayer::Multicast_OnPlayWeaponFppMontageEvent_Implementation(UAnimMontage* PlayedMontage, float Speed)
{
	ASoldier::Multicast_OnPlayWeaponFppMontageEvent_Implementation(PlayedMontage, Speed);

	ArmMesh->GetAnimInstance()->Montage_Play(PlayedMontage, Speed);
}

void AKraverPlayer::RefreshCurViewType()
{
	for(int i = 0; i < 2; i++)
		ChangeView();
}

void AKraverPlayer::OnClientEquipWeaponSuccessEvent(AWeapon* Weapon)
{	
	if(!Weapon)
		return;

	ASoldier::OnClientEquipWeaponSuccessEvent(Weapon);

	int32 Index = Weapon->MakeAdditivePrimitiveInfo();
	
	TMap<FString, UPrimitiveComponent*>* TempInfo = &Weapon->GetAdditiveWeaponPrimitiveInfo()[Index];
	ArmWeaponMeshes.Add(Weapon, TempInfo);
	Weapon->GetAdditiveWeaponPrimitiveInfo()[Index]["Root"]->AttachToComponent(ArmMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, Weapon->GetAttachSocketName());

	for(auto& Map : Weapon->GetAdditiveWeaponPrimitiveInfo()[Index])
	{
		Map.Value->SetOnlyOwnerSee(true);
		Map.Value->SetCastShadow(false);
		Map.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		ShowOnlyFirstPerson.Push(Map.Value);
	}
	
	for (auto& Map : Weapon->GetWeaponPrimitiveInfo())
	{
		ShowOnlyThirdPerson.Push(Map.Value);
	}

	RefreshCurViewType();

}

void AKraverPlayer::OnClientUnEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	ASoldier::OnClientUnEquipWeaponSuccessEvent(Weapon);

	ThrowWeapon(Weapon);

	ShowOnlyThirdPerson.Remove(Weapon->GetWeaponMesh());
	Weapon->GetWeaponMesh()->SetOwnerNoSee(false);

	int32 Index = Weapon->FindAdditivePrimitiveInfo(*ArmWeaponMeshes[Weapon]);

	for (auto& Map : Weapon->GetAdditiveWeaponPrimitiveInfo()[Index])
	{
		ShowOnlyFirstPerson.Remove(Map.Value);
	}

	for (auto& Map : Weapon->GetWeaponPrimitiveInfo())
	{
		ShowOnlyThirdPerson.Remove(Map.Value);
	}

	Weapon->RemoveAdditivePrimitiveInfo(*ArmWeaponMeshes[Weapon]);
	ArmWeaponMeshes.Remove(Weapon);
	RefreshCurViewType();
}

void AKraverPlayer::OnClientHoldWeaponEvent(AWeapon* Weapon)
{
	ASoldier::OnClientHoldWeaponEvent(Weapon);

	RefreshCurViewType();
}

void AKraverPlayer::OnClientHolsterWeaponEvent(AWeapon* Weapon)
{
	ASoldier::OnClientHolsterWeaponEvent(Weapon);

	ArmMesh->GetAnimInstance()->Montage_Stop(0.f, Weapon->GetAttackMontageFpp());
	RefreshCurViewType();
}

void AKraverPlayer::Client_SimulateMesh_Implementation()
{
	ASoldier::Client_SimulateMesh_Implementation();

	GetMesh()->SetOwnerNoSee(false);
	ArmMesh->SetOwnerNoSee(true);

	FAttachmentTransformRules TransformRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	TransformRules.bWeldSimulatedBodies = true;
	TransformRules.RotationRule = EAttachmentRule::KeepWorld;
	Camera->AttachToComponent(GetMesh(), TransformRules, "head");
}

void AKraverPlayer::OnAssassinateEvent(AActor* AssassinatedActor)
{
	ASoldier::OnAssassinateEvent(AssassinatedActor);

	GetMesh()->SetOwnerNoSee(false);
	ArmMesh->SetOwnerNoSee(true);

	CombatComponent->GetCurWeapon()->GetWeaponMesh()->SetOwnerNoSee(false);

}

void AKraverPlayer::OnAssassinateEndEvent()
{
	ASoldier::OnAssassinateEndEvent();

	RefreshCurViewType();

	GetMesh()->GetAnimInstance()->Montage_Stop(0.f);
	ArmMesh->GetAnimInstance()->Montage_Stop(0.f);

}

void AKraverPlayer::OnPlayWeaponFppMontageEvent(UAnimMontage* PlayedMontage, float Speed)
{
	ASoldier::OnPlayWeaponFppMontageEvent(PlayedMontage, Speed);
}

void AKraverPlayer::OnFP_Reload_Grab_MagazineEvent()
{
	if (CombatComponent->GetCurWeapon() && ArmWeaponMeshes.Contains(CombatComponent->GetCurWeapon()) && ArmWeaponMeshes[CombatComponent->GetCurWeapon()]->Contains("Magazine"))
	{
#if TEST_RELOAD
		FTransform BeforeTransform = ArmWeaponMeshes[CombatComponent->GetCurWeapon()]->operator[]("Magazine")->GetComponentTransform();
#endif

		UPrimitiveComponent* MagazineComp = ArmWeaponMeshes[CombatComponent->GetCurWeapon()]->operator[]("Magazine");
		UWeaponReloadComponent* ReloadComp = CombatComponent->GetCurWeapon()->FindComponentByClass<UWeaponReloadComponent>();

		MagazineComp->AttachToComponent(ArmMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, "SOCKET_Magazine");
		MagazineComp->SetRelativeLocation(ReloadComp->GetGrabRelativeLocation());
		MagazineComp->SetRelativeRotation(ReloadComp->GetGrabRelativeRotation());
#if TEST_RELOAD
		FTransform AfterTransform = ArmWeaponMeshes[CombatComponent->GetCurWeapon()]->operator[]("Magazine")->GetComponentTransform();
		FTransform RelativeTransform = BeforeTransform.GetRelativeTransform(AfterTransform);
		FRotator RelativeRotation = RelativeTransform.Rotator();
		FVector RelativeLocation = RelativeTransform.GetLocation();

		KR_LOG_VECTOR(RelativeLocation);
		KR_LOG_ROTATOR(RelativeRotation);
#endif
	}
}

void AKraverPlayer::OnFP_Reload_Insert_MagazineEvent()
{
	if (CombatComponent->GetCurWeapon() && ArmWeaponMeshes.Contains(CombatComponent->GetCurWeapon()) &&ArmWeaponMeshes[CombatComponent->GetCurWeapon()]->Contains("Magazine"))
	{
#if TEST_RELOAD
		FTransform BeforeTransform = ArmWeaponMeshes[CombatComponent->GetCurWeapon()]->operator[]("Magazine")->GetComponentTransform();
#endif

		UAttachmentMagazineComponent* MagazineComp = CombatComponent->GetCurWeapon()->FindComponentByClass<UAttachmentMagazineComponent>();
		ArmWeaponMeshes[CombatComponent->GetCurWeapon()]->operator[]("Magazine")->AttachToComponent(ArmWeaponMeshes[CombatComponent->GetCurWeapon()]->operator[]("Root"), FAttachmentTransformRules::SnapToTargetIncludingScale, MagazineComp->GetMagazineSocketName());

#if TEST_RELOAD
		FTransform AfterTransform = ArmWeaponMeshes[CombatComponent->GetCurWeapon()]->operator[]("Magazine")->GetComponentTransform();
		FTransform RelativeTransform = BeforeTransform.GetRelativeTransform(AfterTransform);
		FRotator RelativeRotation = RelativeTransform.Rotator();
		FVector RelativeLocation = RelativeTransform.GetLocation();

		KR_LOG_VECTOR(RelativeLocation);
		KR_LOG_ROTATOR(RelativeRotation);
#endif
	}
}

void AKraverPlayer::PlayLandedMontage()
{
	ASoldier::PlayLandedMontage();

	UCreatureAnimInstance* CreatureAnim = Cast<UCreatureAnimInstance>(ArmMesh->GetAnimInstance());
	CreatureAnim->PlayLandedMontage();
}

void AKraverPlayer::WeaponADS(float DeltaTime)
{
	if (CombatComponent->GetCurWeapon() && CombatComponent->GetCurWeapon()->GetIsSubAttacking() && Cast<AGun>(CombatComponent->GetCurWeapon()))
	{
		USkeletalMeshComponent* ArmWeaponMesh = dynamic_cast<USkeletalMeshComponent*>(ArmWeaponMeshes[CombatComponent->GetCurWeapon()]->operator[]("Root"));
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
		#else
		if (ViewType == EViewType::FIRST_PERSON && HUD)
			HUD->SetbDrawCrosshair(false);
		#endif

		const TMap<FString, UPrimitiveComponent*>& WeaponPrimitiveInfo = *ArmWeaponMeshes[CombatComponent->GetCurWeapon()];
		if(WeaponPrimitiveInfo.Contains("Scope"))
		{
			UStaticMeshComponent* ScopeMesh = Cast<UStaticMeshComponent>(WeaponPrimitiveInfo["Scope"]);
			if (ScopeMesh)
			{
				FTransform WeaponTransform = ArmWeaponMesh->GetSocketTransform("SOCKET_AIM", ERelativeTransformSpace::RTS_World);
				FTransform ScopeTransform = ScopeMesh->GetSocketTransform("AimOffset", ERelativeTransformSpace::RTS_World);
				FTransform RelativeTransform = ScopeTransform.GetRelativeTransform(WeaponTransform);
				FVector RelativeLocation = RelativeTransform.GetLocation();

				AdsArmLocation.Z = FMath::FInterpTo(AdsArmLocation.Z, -RelativeLocation.Z, DeltaTime, 10.f);
			}
			else
				AdsArmLocation.Z = FMath::FInterpTo(AdsArmLocation.Z, 0.f, DeltaTime, 10.f);
		}
		else
			AdsArmLocation.Z = FMath::FInterpTo(AdsArmLocation.Z, 0.f, DeltaTime, 10.f);


		Camera->SetFieldOfView(FMath::FInterpTo(Camera->FieldOfView, 95.f, DeltaTime, 15.f));
	}
	else
	{
		if (HUD)
			HUD->SetbDrawCrosshair(true);

		AdsArmLocation.Z = FMath::FInterpTo(AdsArmLocation.Z, 0.f, DeltaTime, 10.f);
		Camera->SetFieldOfView(FMath::FInterpTo(Camera->FieldOfView, 110.f, DeltaTime, 15.f));
	}

}

void AKraverPlayer::SpringArmTick(float DeltaTime)
{
	if (GetMovementComponent()->IsCrouching())
		FP_SpringArmCrouchLocation.Z = FMath::FInterpTo(FP_SpringArmCrouchLocation.Z, CrouchCameraHeight, DeltaTime, 20.f);
	else
		FP_SpringArmCrouchLocation.Z = FMath::FInterpTo(FP_SpringArmCrouchLocation.Z, UnCrouchCameraHeight, DeltaTime, 20.f);

	Fp_SpringArm->SetRelativeLocation(Fp_SpringArmBasicLocation + FP_SpringArmCrouchLocation);
	RefreshArm();
}
