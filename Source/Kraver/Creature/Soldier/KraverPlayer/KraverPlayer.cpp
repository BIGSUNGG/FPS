// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayer.h"
#include "Kraver/Weapon/Gun/Gun.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Kraver/HUD/KraverHud.h"
#include "Kraver/PlayerController/KraverPlayerController.h"
#include "Kraver/GameMode/KraverGameMode.h"
#include "Kraver/Anim/Creature/CreatureAnimInstance.h"
#include "Math/UnrealMathUtility.h"
#include "Math/TransformNonVectorized.h"
#include "Kismet/KismetMathLibrary.h"

AKraverPlayer::AKraverPlayer() : ASoldier()
{
	NetUpdateFrequency = 300.f;

	ArmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMesh"));
	ArmMesh->SetupAttachment(Camera);
	ArmMesh->SetCastShadow(false);
	ArmMesh->SetOnlyOwnerSee(true);
	ArmMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArmMesh->SetScalarParameterValueOnMaterials("Type",0);
		
	ShowOnlyFirstPerson.Push(ArmMesh);

	ShowOnlyThirdPerson.Push(GetMesh());

	GetCharacterMovement()->AirControl = 0.5f;
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(60.f);

	RefreshCurViewType();
}

void AKraverPlayer::BeginPlay()
{
	ASoldier::BeginPlay();

	BasicArmLocation = ArmMesh->GetRelativeLocation();
	BasicArmRotation = ArmMesh->GetRelativeRotation();

	if (IsLocallyControlled())
	{
		CombatComponent->SetMaxWeaponSlot(3);

		KraverController = KraverController == nullptr ? Cast<AKraverPlayerController>(Controller) : KraverController;
		if (KraverController)
			HUD = HUD == nullptr ? Cast<AKraverHUD>(KraverController->GetHUD()) : HUD;
		RefreshCurViewType();

		DefaultGravity = GetCharacterMovement()->GravityScale;
		DefaultGroundFriction = GetCharacterMovement()->GroundFriction;
		DefaultBrakingDecelerationWalking = GetCharacterMovement()->BrakingDecelerationWalking;
	}
}

void AKraverPlayer::Tick(float DeltaTime)
{
	ASoldier::Tick(DeltaTime);

	KraverController = KraverController == nullptr ? Cast<AKraverPlayerController>(Controller) : KraverController;
	if (KraverController)
		HUD = HUD == nullptr ? Cast<AKraverHUD>(KraverController->GetHUD()) : HUD;

	if (HasAuthority() == false && IsLocallyControlled() == false)
		Camera->SetRelativeRotation(FRotator(AO_Pitch, AO_Yaw, 0.0f));

	ClientTick(DeltaTime);
	ServerClientTick(DeltaTime);
	LocallyControlTick(DeltaTime);
}

void AKraverPlayer::CameraTick()
{
	if (CurWallRunState == EWallRunState::WALLRUN_LEFT)
		CameraTilt(15.f);
	else if(CurWallRunState == EWallRunState::WALLRUN_RIGHT)
		CameraTilt(-15.f);		
	else
		CameraTilt(0.f);
	
}

void AKraverPlayer::CameraTilt(float TargetRoll)
{
	FRotator TargetRotation(GetController()->GetControlRotation().Pitch, GetController()->GetControlRotation().Yaw, TargetRoll);
	GetController()->SetControlRotation(FMath::RInterpTo(GetController()->GetControlRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 10.f));

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

	if (bWantToJump == true)
	{
		bWantToJump = false;
		ASoldier::Jump();
	}

	CheckCanInteractionWeapon();
	
	WeaponADS(DeltaTime);
	WallRunUpdate();
	SlideUpdate();

	SpringArmTick(DeltaTime);
	CameraTick();
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
	DOREPLIFETIME(AKraverPlayer, CurWallRunState);
	DOREPLIFETIME(AKraverPlayer, IsSliding);
}

bool AKraverPlayer::GetCanAttack()
{
	return true;
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

void AKraverPlayer::SetCurWallRunState(EWallRunState Value)
{
	CurWallRunState = Value;
	Server_SetCurWallRunState(Value);
}

void AKraverPlayer::Server_SetCurWallRunState_Implementation(EWallRunState Value)
{
	CurWallRunState = Value;
}

void AKraverPlayer::SetIsSliding(bool Value)
{
	IsSliding = Value;
	Server_SetIsSliding(Value);
}

void AKraverPlayer::Server_SetIsSliding_Implementation(bool Value)
{
	IsSliding = Value;
}

void AKraverPlayer::SubAttackButtonPressed()
{
	ASoldier::SubAttackButtonPressed();

}

void AKraverPlayer::SubAttackButtonReleased()
{
	ASoldier::SubAttackButtonReleased();

	if (CombatComponent->GetCurWeapon())
	{
		GetArmWeaponMeshes()[CombatComponent->GetCurWeapon()]->SetRelativeRotation(FRotator::ZeroRotator);
	}
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
	float Distance = InteractionDistance + SpringArm->TargetArmLength;

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
		SpringArm->TargetArmLength = 300.f;
		SpringArmBasicLocation = FVector(-45.f, 0.f, 150.f);
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
		SpringArm->TargetArmLength = 0.f;
		SpringArmBasicLocation = FVector(0.f, 0.f, 150.f);
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
	if (CombatComponent->GetIsDead() == false)
	{
		switch (ViewType)
		{
		case EViewType::FIRST_PERSON:
		{
			USkeletalMeshComponent* ArmWeaponMesh = ArmWeaponMeshes[Weapon];
			RpcComponent->SetSimulatedPhysics(Weapon->GetWeaponMesh(), false);
			RpcComponent->DetachComponentFromComponent(Weapon->GetWeaponMesh());
			RpcComponent->SetComponentLocation(Weapon->GetWeaponMesh(), ArmWeaponMesh->GetComponentLocation());
			RpcComponent->SetComponentRotation(Weapon->GetWeaponMesh(), ArmWeaponMesh->GetComponentRotation());
			GetWorldTimerManager().SetTimer(
				UnEquipWeaponTimerHandle,
				[=]() {					
					RpcComponent->SetSimulatedPhysics(Weapon->GetWeaponMesh(), true);
					RpcComponent->SetPhysicsLinearVelocity(Weapon->GetWeaponMesh(), FVector::ZeroVector);
					RpcComponent->AddImpulse(Weapon->GetWeaponMesh(), (Camera->GetForwardVector() * WeaponThrowPower * Weapon->GetWeaponMesh()->GetMass()));
					RpcComponent->AddAngularImpulseInDegrees(Weapon->GetWeaponMesh(), Camera->GetForwardVector() * WeaponThrowAngularPower, NAME_None, true);
				},
				0.000001f,
					false);
			break;
		}
		case EViewType::THIRD_PERSON:
			Weapon->GetWeaponMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
			Weapon->GetWeaponMesh()->AddImpulse(Camera->GetForwardVector() * WeaponThrowPower, NAME_None, true);
			break;
		default:
			UE_LOG(LogTemp, Fatal, TEXT("Need to support more EViewType"));
			break;
		}
	}

}

void AKraverPlayer::OnDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ASoldier::OnDeathEvent(DamageAmount,DamageEvent,EventInstigator,DamageCauser);

	GetMesh()->SetOwnerNoSee(false);
	ArmMesh->SetOwnerNoSee(true);
	Camera->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,"head");
}

void AKraverPlayer::RefreshArm()
{
	ArmMesh->SetRelativeLocation(BasicArmLocation + WeaponAdsLocation, false);
	ArmMesh->SetRelativeRotation(BasicArmRotation + WeaponAdsRotation, false);
}

void AKraverPlayer::RefreshSpringArm()
{
	SpringArm->SetRelativeLocation(SpringArmBasicLocation + SpringArmCrouchLocation);
	Server_RefreshSpringArm(SpringArmBasicLocation + SpringArmCrouchLocation, SpringArm->TargetArmLength);
}

void AKraverPlayer::Server_RefreshSpringArm_Implementation(FVector Vector, float Length)
{
	Multicast_RefreshSpringArm(Vector,Length);
}

void AKraverPlayer::Multicast_RefreshSpringArm_Implementation(FVector Vector, float Length)
{
	SpringArm->SetRelativeLocation(Vector);
	SpringArm->TargetArmLength = Length;
}

void AKraverPlayer::RefreshCurViewType()
{
	for(int i = 0; i < 2; i++)
		ChangeView();
}

void AKraverPlayer::Landed(const FHitResult& Hit)
{
	ASoldier::Landed(Hit);

	ResetWallRunVerticalSuppression();
	ResetWallRunHorizonSuppression();
	ResetSlideSuppression();
	if (bCrouchButtonPress)
	{
		if(CanSlide())
			SlideStart();
		else
			ASoldier::Crouch();
	}

	bCanDoubleJump = true;
}

void AKraverPlayer::OnEquipWeaponSuccessEvent(AWeapon* Weapon)
{	
	if(!Weapon)
		return;

	ASoldier::OnEquipWeaponSuccessEvent(Weapon);

	int32 Index = Weapon->MakeAdditiveWeaponMesh();
	ArmWeaponMeshes.Add(Weapon, Weapon->GetAdditiveWeaponMesh()[Index]);
	Weapon->GetAdditiveWeaponMesh()[Index]->AttachToComponent(ArmMesh,FAttachmentTransformRules::SnapToTargetIncludingScale, Weapon->GetAttachSocketName());
	Weapon->GetAdditiveWeaponMesh()[Index]->SetCastShadow(false);	
	Weapon->GetAdditiveWeaponMesh()[Index]->SetOnlyOwnerSee(true);
	Weapon->GetAdditiveWeaponMesh()[Index]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShowOnlyFirstPerson.Push(Weapon->GetAdditiveWeaponMesh()[Index]);

	switch (Weapon->GetWeaponType())
	{
	case EWeaponType::GUN:
	{
		AGun* Gun = Cast<AGun>(Weapon);  
		ShowOnlyThirdPerson.Push(Gun->GetFireEffect());
		auto FireEffect = Gun->GetAdditiveFireEffect()[Index];
		Gun->GetAdditiveFireEffect()[Index]->SetOnlyOwnerSee(true);
		ShowOnlyFirstPerson.Push(FireEffect);
	}
	break;
	case EWeaponType::MELEE:
	{

	}
		break;
	default:
		break;
	}
	RefreshCurViewType();

	ShowOnlyThirdPerson.Push(Weapon->GetWeaponMesh());
}

void AKraverPlayer::OnUnEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	ASoldier::OnUnEquipWeaponSuccessEvent(Weapon);
	ShowOnlyThirdPerson.Remove(Weapon->GetWeaponMesh());
	Weapon->GetWeaponMesh()->SetOwnerNoSee(false);

	int32 Index = Weapon->FindAdditiveWeaponMesh(ArmWeaponMeshes[Weapon]);
	if (Index != -1)
	{
		switch (Weapon->GetWeaponType())
		{
		case EWeaponType::GUN:
		{
			AGun* Gun = Cast<AGun>(Weapon);
			ShowOnlyThirdPerson.Remove(Gun->GetFireEffect());
			auto FireEffect = Gun->GetAdditiveFireEffect()[Index];
			Gun->GetAdditiveFireEffect()[Index]->SetOnlyOwnerSee(false);
			ShowOnlyFirstPerson.Remove(FireEffect);
		}
		break;
		case EWeaponType::MELEE:
		{
		}
		break;
		default:
			break;
		}
	}
	ThrowWeapon(Weapon);
	Weapon->RemoveAdditiveWeaponMesh(ArmWeaponMeshes[Weapon]);
	ArmWeaponMeshes.Remove(Weapon);
	RefreshCurViewType();

}

void AKraverPlayer::OnHoldWeaponEvent(AWeapon* Weapon)
{
	ASoldier::OnHoldWeaponEvent(Weapon);

	ArmWeaponMeshes[Weapon]->SetHiddenInGame(false);
	RefreshCurViewType();
}

void AKraverPlayer::OnHolsterWeaponEvent(AWeapon* Weapon)
{
	ASoldier::OnHolsterWeaponEvent(Weapon);
	
	ArmWeaponMeshes[Weapon]->SetHiddenInGame(true);
	RpcComponent->Montage_Stop(ArmMesh, Weapon->GetReloadMontageFpp());
	RpcComponent->Montage_Stop(ArmMesh, Weapon->GetAttackMontageFpp());
	RefreshCurViewType();
}

void AKraverPlayer::Crouch(bool bClientSimulation /*= false*/)
{
	if (GetMovementComponent()->IsFalling() == false && CanSlide())
		SlideStart();
	else if(GetIsWallRunning())
		WallRunEnd(0.3f);
	else if(GetCharacterMovement()->IsFalling() == false)
		ASoldier::Crouch(bClientSimulation);
}

void AKraverPlayer::UnCrouch(bool bClientSimulation /*= false*/)
{
	if(IsSliding)
		SlideEnd(true);
	else if(GetCharacterMovement()->IsFalling() == false)
		ASoldier::UnCrouch(bClientSimulation);
}

void AKraverPlayer::PlayReloadMontage()
{
	ASoldier::PlayReloadMontage();

	if (CombatComponent->GetCurWeapon() == nullptr ||
		CombatComponent->GetCurWeapon()->GetCanReload() == false ||
		ArmMesh->GetAnimInstance()->Montage_IsPlaying(CombatComponent->GetCurWeapon()->GetReloadMontageFpp()) == true)
	{
		return;
	}

	ArmMesh->GetAnimInstance()->Montage_Play(CombatComponent->GetCurWeapon()->GetReloadMontageFpp(), 1.5f);
}

void AKraverPlayer::PlayAttackMontage()
{
	ASoldier::PlayAttackMontage();
	RpcComponent->Montage_Play(ArmMesh, CombatComponent->GetCurWeapon()->GetAttackMontageFpp(), 1.0f);
}

void AKraverPlayer::PlayLandedMontage()
{
	ASoldier::PlayLandedMontage();

	UCreatureAnimInstance* CreatureAnim = Cast<UCreatureAnimInstance>(ArmMesh->GetAnimInstance());
	CreatureAnim->PlayLandedMontage();
}

void AKraverPlayer::StopReloadMontage()
{
	ASoldier::StopReloadMontage();

	if (CombatComponent->GetCurWeapon() == nullptr)
		return;

	RpcComponent->Montage_Stop(ArmMesh, CombatComponent->GetCurWeapon()->GetReloadMontageFpp(), 0.f);
}

void AKraverPlayer::WeaponADS(float DeltaTime)
{
	if (CombatComponent->GetCurWeapon() && CombatComponent->GetCurWeapon()->GetIsSubAttacking() && ArmMesh->GetAnimInstance()->Montage_IsPlaying(CombatComponent->GetCurWeapon()->GetReloadMontageFpp()) == false)
	{
		USkeletalMeshComponent* ArmWeaponMesh = GetArmWeaponMeshes()[CombatComponent->GetCurWeapon()];
		ArmMesh->AddRelativeRotation(WeaponAdsRotation * -1);
		ArmMesh->AddRelativeLocation(WeaponAdsLocation * -1);

		FTransform WeaponTransform = ArmWeaponMesh->GetSocketTransform("SOCKET_AIM", ERelativeTransformSpace::RTS_World);
		FTransform CameraTransform = Camera->GetComponentTransform();
		FTransform RelativeTransform = WeaponTransform.GetRelativeTransform(CameraTransform);
		FRotator RelativeRotation = RelativeTransform.Rotator();
		FVector RelativeLocation = RelativeTransform.GetLocation();

		FRotator TargetRotation;
		FVector TargetLocation;

		TargetRotation.Pitch = -RelativeRotation.Pitch;
		TargetRotation.Roll = -RelativeRotation.Roll;
		TargetRotation.Yaw = -(90.f + RelativeRotation.Yaw);

		TargetLocation.X = -RelativeLocation.X;
		TargetLocation.Y = -RelativeLocation.Y;
		TargetLocation.Z = -RelativeLocation.Z;

		WeaponAdsRotation = FMath::RInterpTo(WeaponAdsRotation, TargetRotation, DeltaTime, 20.f);
		WeaponAdsLocation = FMath::VInterpTo(WeaponAdsLocation, TargetLocation, DeltaTime, 20.f);

		if (HUD)
			HUD->SetbDrawCrosshair(false);
	}
	else if ((CombatComponent->GetCurWeapon() && CombatComponent->GetCurWeapon()->GetIsSubAttacking() && ArmMesh->GetAnimInstance()->Montage_IsPlaying(CombatComponent->GetCurWeapon()->GetReloadMontageFpp())) == false)
	{
		FRotator TargetRotation = FRotator::ZeroRotator;
		FVector TargetLocation = FVector::ZeroVector;

		WeaponAdsRotation = FMath::RInterpTo(WeaponAdsRotation, TargetRotation, DeltaTime, 20.f);
		WeaponAdsLocation = FMath::VInterpTo(WeaponAdsLocation, TargetLocation, DeltaTime, 20.f);

		if (HUD)
			HUD->SetbDrawCrosshair(true);

	}

}

void AKraverPlayer::SpringArmTick(float DeltaTime)
{
	if (GetMovementComponent()->IsCrouching())
		SpringArmCrouchLocation.Z = FMath::FInterpTo(SpringArmCrouchLocation.Z, CrouchCameraHeight, DeltaTime, 20.f);
	else
		SpringArmCrouchLocation.Z = FMath::FInterpTo(SpringArmCrouchLocation.Z, UnCrouchCameraHeight, DeltaTime, 20.f);

	SpringArm->SetRelativeLocation(SpringArmBasicLocation + SpringArmCrouchLocation);
	RefreshArm();
}

void AKraverPlayer::Jump()
{
	if (GetMovementComponent()->IsFalling() == false)
	{
		if (IsSliding)
		{
			SlideEnd(true);
			bWantToJump = true;
		}
		else if (GetCharacterMovement()->IsCrouching())
		{
			UnCrouch();
			bWantToJump = true;
		}
		ASoldier::Jump();
	}
	else if(GetIsWallRunning())
		WallRunJump();
	else if (bCanDoubleJump)
		DoubleJump();
}

void AKraverPlayer::WallRunJump()
{
	if (GetIsWallRunning())
	{
		WallRunEnd(0.35);
		FVector LaunchVector;
		LaunchVector.X = WallRunNormal.X * WallRunJumpOffForce;
		LaunchVector.Y = WallRunNormal.Y * WallRunJumpOffForce;
		LaunchVector.Z = WallRunJumpHeight;
		LaunchCharacter(LaunchVector, false ,true);
		RpcComponent->SetPendingLaunchVelocity(GetCharacterMovement()->PendingLaunchVelocity);
		UCreatureAnimInstance* CreatureAnim = Cast<UCreatureAnimInstance>(GetMesh()->GetAnimInstance());
		RpcComponent->Montage_Play(GetMesh(), CreatureAnim->GetJumpMontage());
	}
	else
	{
		KR_LOG(Warning, TEXT("IsWallRunning is false"))
	}
}

void AKraverPlayer::DoubleJump()
{
	bCanDoubleJump = false;

	FVector LaunchPower(0, 0, DobuleJumpPower.Z);

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();

	FVector Velocity = MovementComp->Velocity;

	FVector Right = GetControlRotation().Vector().GetSafeNormal2D().RotateAngleAxis(-90.0f, FVector::UpVector);
	float RightSpeed = -FVector::DotProduct(Velocity, Right) / Right.Size2D();

	FVector Forward = GetControlRotation().Vector().GetSafeNormal2D();
	float ForwardSpeed = FVector::DotProduct(Velocity, Forward) / Forward.Size2D();

	float ForwardLaunchPower;
	float RightLaunchPower;

	if (CurrentInputForward > 0.f && ForwardSpeed > DobuleJumpPower.X * CurrentInputForward)
		ForwardLaunchPower = ForwardSpeed;
	else if (CurrentInputForward < 0.f && -ForwardSpeed > DobuleJumpPower.X * -CurrentInputForward)
		ForwardLaunchPower = ForwardSpeed;
	else
		ForwardLaunchPower = DobuleJumpPower.X * CurrentInputForward;

	if (CurrentInputRight > 0.f && RightSpeed > DobuleJumpPower.Y * CurrentInputRight)
		RightLaunchPower = RightSpeed;
	else if (CurrentInputRight < 0.f && -RightSpeed > DobuleJumpPower.Y * -CurrentInputRight)
		RightLaunchPower = RightSpeed;
	else
		RightLaunchPower = DobuleJumpPower.Y * CurrentInputRight;

	bool bOverideXY = false;
	if (CurrentInputForward != 0.f)
	{
		FRotator Rotation = GetController()->GetControlRotation();
		FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		LaunchPower += FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * ForwardLaunchPower;
		bOverideXY = true;
	}
	if (CurrentInputRight != 0.f)
	{
		FRotator Rotation = GetController()->GetControlRotation();
		FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		LaunchPower += FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * RightLaunchPower;
		bOverideXY = true;
	}

	LaunchCharacter(LaunchPower, bOverideXY, true);
	RpcComponent->SetPendingLaunchVelocity(GetCharacterMovement()->PendingLaunchVelocity);

	UCreatureAnimInstance* CreatureAnim = Cast<UCreatureAnimInstance>(GetMesh()->GetAnimInstance());
	RpcComponent->Montage_Play(GetMesh(), CreatureAnim->GetJumpMontage());

	KR_LOG(Log, TEXT("Dobule Jump Power : %f %f"), ForwardLaunchPower, RightLaunchPower);
}

bool AKraverPlayer::WallRunUpdate()
{
	if (bJumpButtonPress || GetIsWallRunning())
	{	
		bool WallRunVerticalSuccess = false;
		bool WallRunHorizonSuccess = false;

		if(!bWallRunVerticalSupressed)
		{
			WallRunVerticalSuccess = WallRunVerticalUpdate();
			if (WallRunVerticalSuccess)
			{
				GEngine->AddOnScreenDebugMessage(
					0,
					0.f,
					FColor::White,
					TEXT("WallRunVertical")
				);
				return true;
			}
		}

		if(!bWallRunHorizonSupressed)
		{
			WallRunHorizonSuccess = WallRunHorizonUpdate();
			if (WallRunHorizonSuccess)
			{
				if (bWallRunGravity)
					RpcComponent->SetGravityScale(WallRunTargetGravity);
				if (GetVelocity().Z < 0.f)
				{
					RpcComponent->SetGravityScale(0.f);
					GetCharacterMovement()->PendingLaunchVelocity.Z = 0.f;
					RpcComponent->SetPendingLaunchVelocity(GetCharacterMovement()->PendingLaunchVelocity);
				}

				GEngine->AddOnScreenDebugMessage(
					0,
					0.f,
					FColor::White,
					TEXT("WallRunHorizon")
				);
				return true;
			}
		}
	}

	return false;
}

bool AKraverPlayer::WallRunHorizonUpdate()
{
	if (WallRunHorizonMovement(GetActorLocation(), CalculateRightWallRunEndVector(), -1.f))
	{
		if(!GetIsWallRunning())
			WallRunStart();

		SetCurWallRunState(EWallRunState::WALLRUN_RIGHT);
		return true;
	}
	else if(CurWallRunState == EWallRunState::WALLRUN_RIGHT)
	{
		WallRunHorizonEnd(1.f);
		return false;
	}
	else if (WallRunHorizonMovement(GetActorLocation(), CalculateLeftWallRunEndVector(), 1.f))
	{
		if (!GetIsWallRunning())
			WallRunStart();

		SetCurWallRunState(EWallRunState::WALLRUN_LEFT);
		return true;
	}
	else if (CurWallRunState != EWallRunState::WALLRUN_VERTICAL)
	{
		WallRunHorizonEnd(1.f);
		return false;

	}

	return false;
}

bool AKraverPlayer::WallRunVerticalUpdate()
{
	if (bWallRunVerticalSupressed)
		return false;

	if (WallRunVerticalMovement(GetActorLocation(), CalculateVerticaltWallRunEndVector()))
	{
		if(!GetIsWallRunning())
			WallRunStart();
	
		SetCurWallRunState(EWallRunState::WALLRUN_VERTICAL);
		RpcComponent->SetGravityScale(0.f);
		return true;
	}
	else
	{
		WallRunVerticalEnd(1.f);
		return false;
	}
}

bool AKraverPlayer::WallRunHorizonMovement(FVector Start, FVector End, float WallRunDirection)
{
	if(bWallRunHorizonSupressed)
		return false;

	FCollisionQueryParams ObjectParams(NAME_None, false, this);
	FHitResult Result;
	bool bSuccess = GetWorld()->LineTraceSingleByChannel(
		Result,
		Start,
		End, 
		ECC_ONLY_OBJECT,
		ObjectParams
	);

	if (bSuccess && Result.bBlockingHit)
	{
		if (GetMovementComponent()->IsFalling() && CalculateValidWallVector(Result.Normal))
		{ 
			WallRunNormal = Result.Normal;
			FVector WallRunVec = FVector::CrossProduct(WallRunNormal, FVector(0, 0, 1)) * (WallRunDirection * WallRunHorizonSpeed);
			LaunchCharacter(WallRunVec,true, !bWallRunGravity);

			FRotator Temp = WallRunVec.Rotation();
			KR_LOG(Log, TEXT("%f %f %f"), Temp.Pitch, Temp.Roll, Temp.Yaw);
			RpcComponent->RegistCurMovement();
			return true;
		}
	}
	return false;
}	

void AKraverPlayer::SlideStart()
{
	if(IsSliding)
		return;

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();

	FVector Velocity = MovementComp->Velocity;

	FVector Forward = GetControlRotation().Vector().GetSafeNormal2D();
	float ForwardSpeed = FVector::DotProduct(Velocity, Forward) / Forward.Size2D();

	FVector Right = GetControlRotation().Vector().GetSafeNormal2D().RotateAngleAxis(-90.0f, FVector::UpVector);
	float RightSpeed = -FVector::DotProduct(Velocity, Right) / Right.Size2D();

	IsSliding = true;

	FVector SlidePower;
	if (ForwardSpeed > SlideSpeed)
	{
		FRotator Rotation = GetController()->GetControlRotation();
		FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		SlidePower += FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * ForwardSpeed;
	}
	else
	{
		FRotator Rotation = GetController()->GetControlRotation();
		FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		SlidePower += FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * SlideSpeed;
	}

	FRotator Rotation = GetController()->GetControlRotation();
	FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	SlidePower += FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * RightSpeed;

	InputForwardRatio = 0.4f;
	InputRightRatio = 0.4f;
	GetCharacterMovement()->GroundFriction = SlideGroundFriction;
	GetCharacterMovement()->BrakingDecelerationWalking = SlideBrakingDecelerationWalking;
	RpcComponent->SetGroundFriction(SlideGroundFriction);
	RpcComponent->SetBrakingDecelerationWalking(SlideBrakingDecelerationWalking);
	GetCharacterMovement()->Velocity.X = SlidePower.X;
	GetCharacterMovement()->Velocity.Y = SlidePower.Y;
	RpcComponent->SetVelocity(GetCharacterMovement()->Velocity);
	ASoldier::Crouch(true);

	KR_LOG(Log,TEXT("Slide Start"));
}

void AKraverPlayer::SlideEnd(bool DoUncrouch)
{
	if(!IsSliding || bSlideSupressed)
		return;

	IsSliding = false;

	InputForwardRatio = 1.f;
	InputRightRatio = 1.f;
	GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	GetCharacterMovement()->BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;
	RpcComponent->SetGroundFriction(DefaultGroundFriction);
	RpcComponent->SetBrakingDecelerationWalking(DefaultBrakingDecelerationWalking);

	if(DoUncrouch)
		ASoldier::UnCrouch(true);

	SuppressSlide(0.5f);
	KR_LOG(Log, TEXT("Slide End"));
}

void AKraverPlayer::SuppressSlide(float Delay)
{
	bSlideSupressed = true;
	GetWorldTimerManager().SetTimer(SuppressSlideTimer, this, &AKraverPlayer::ResetSlideSuppression, Delay, false);
}

void AKraverPlayer::ResetSlideSuppression()
{
	bSlideSupressed = false;
	GetWorldTimerManager().ClearTimer(SuppressSlideTimer);
}

bool AKraverPlayer::WallRunVerticalMovement(FVector Start, FVector End)
{
	FCollisionQueryParams ObjectParams(NAME_None, false, this);
	FHitResult Result;
	bool bSuccess = GetWorld()->LineTraceSingleByChannel(
		Result,
		Start,
		End,
		ECC_ONLY_OBJECT,
		ObjectParams
	);

	if (bSuccess && Result.bBlockingHit)
	{
		if (GetMovementComponent()->IsFalling() && CalculateValidWallVector(Result.Normal))
		{
			WallRunNormal = Result.Normal;

			LaunchCharacter(GetActorUpVector() * WallRunVerticalSpeed, true, true);
			RpcComponent->RegistCurMovement();
			return true;
		}
	}
	return false;
}

void AKraverPlayer::WallRunStart()
{
	if(GetIsWallRunning())
		return;

	bCanDoubleJump = true;
}

void AKraverPlayer::WallRunEnd(float ResetTime)
{
	if (GetIsWallRunning() == false)
		return;

	SetCurWallRunState(EWallRunState::NONE);
	RpcComponent->SetGravityScale(DefaultGravity);
	SuppressWallRunHorizion(ResetTime);
	SuppressWallRunVertical(ResetTime);
}

void AKraverPlayer::WallRunHorizonEnd(float ResetTime)
{
	if(GetIsWallRunning() == false)
		return;

	SetCurWallRunState(EWallRunState::NONE);
	RpcComponent->SetGravityScale(DefaultGravity);
	SuppressWallRunHorizion(ResetTime);
}

void AKraverPlayer::WallRunVerticalEnd(float ResetTime)
{
	if (GetIsWallRunning() == false)
		return; 

	SetCurWallRunState(EWallRunState::NONE);
	RpcComponent->SetGravityScale(DefaultGravity);
	SuppressWallRunVertical(ResetTime);
}

void AKraverPlayer::ResetWallRunHorizonSuppression()
{
	bWallRunHorizonSupressed = false;
	GetWorldTimerManager().ClearTimer(SuppressWallRunHorizonTimer);
}

void AKraverPlayer::ResetWallRunVerticalSuppression()
{
	bWallRunVerticalSupressed = false;
	GetWorldTimerManager().ClearTimer(SuppressWallRunVerticalTimer);
}

void AKraverPlayer::SuppressWallRunHorizion(float Delay)
{
	bWallRunHorizonSupressed = true;
	GetWorldTimerManager().SetTimer(SuppressWallRunHorizonTimer, this, &AKraverPlayer::ResetWallRunHorizonSuppression, Delay, false);
}

void AKraverPlayer::SuppressWallRunVertical(float Delay)
{
	bWallRunVerticalSupressed = true;
	GetWorldTimerManager().SetTimer(SuppressWallRunVerticalTimer, this, &AKraverPlayer::ResetWallRunVerticalSuppression, Delay, false);
}

FVector AKraverPlayer::CalculateRightWallRunEndVector()
{
	return (
		GetActorLocation() + 
		GetActorRightVector() * 75.f +
		GetActorForwardVector() * -35.f
		);
}

FVector AKraverPlayer::CalculateLeftWallRunEndVector()
{
	return (
		GetActorLocation() +
		GetActorRightVector() * -75.f +
		GetActorForwardVector() * -35.f
		);
}

FVector AKraverPlayer::CalculateVerticaltWallRunEndVector()
{
	return (
		GetActorLocation() +
		GetActorForwardVector() * 75.f
		);
}

bool AKraverPlayer::CalculateValidWallVector(FVector InVec)
{
	return UKismetMathLibrary::InRange_FloatFloat(InVec.Z, -0.52f, 0.52, false, false);
}

FVector AKraverPlayer::CalculatePlayerToWallVector()
{
	return -WallRunNormal;
}

bool AKraverPlayer::CanSlide()
{
	if(bSlideSupressed)
		return false;

	if(IsSliding)
		return false;

	if(MovementState != EMovementState::SPRINT)
		return false;

	if (CalculateForwardSpeed() < MinSlideRequireSpeed)
		return false;

	return true;
}

void AKraverPlayer::SlideUpdate()
{
	if (!IsSliding)
		return;

	float Speed = GetVelocity().Size();
	if (Speed < GetCharacterMovement()->MaxWalkSpeedCrouched)
	{
		SlideEnd(false);
		return;
	}

	FVector Slope = CaclulateCurrentFllorSlopeVector();
	FVector SlopePhysics = Slope * SlideSlopeSpeed;
	SlopePhysics.Z = 0.f;
	GetCharacterMovement()->Velocity += SlopePhysics;
	RpcComponent->SetVelocity(GetCharacterMovement()->Velocity);
}
