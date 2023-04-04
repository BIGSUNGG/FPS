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
	if (IsWallRunningL)
	{
		CameraTilt(15.f);
	}
	else
	{
		if(IsWallRunningR)
			CameraTilt(-15.f);
		else
			CameraTilt(0.f);
	}
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

	if(!bWallRunSupressed)
		WallRunUpdate();
	CheckCanInteractionWeapon();
	if (GetMovementComponent()->IsCrouching())
		SpringArmCrouchLocation.Z = FMath::FInterpTo(SpringArmCrouchLocation.Z, CrouchCameraHeight, DeltaTime, 20.f);
	else
		SpringArmCrouchLocation.Z = FMath::FInterpTo(SpringArmCrouchLocation.Z, UnCrouchCameraHeight, DeltaTime, 20.f);

	SpringArm->SetRelativeLocation(SpringArmBasicLocation + SpringArmCrouchLocation);

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

		if(HUD)
			HUD->SetbDrawCrosshair(false);
	}
	else if((CombatComponent->GetCurWeapon() && CombatComponent->GetCurWeapon()->GetIsSubAttacking() && ArmMesh->GetAnimInstance()->Montage_IsPlaying(CombatComponent->GetCurWeapon()->GetReloadMontageFpp())) == false)
	{
		FRotator TargetRotation = FRotator::ZeroRotator;
		FVector TargetLocation = FVector::ZeroVector;

		WeaponAdsRotation = FMath::RInterpTo(WeaponAdsRotation, TargetRotation, DeltaTime, 20.f);
		WeaponAdsLocation = FMath::VInterpTo(WeaponAdsLocation, TargetLocation, DeltaTime, 20.f);

		if (HUD)
			HUD->SetbDrawCrosshair(true);

	}
	CameraTick();
	RefreshArm();

	KR_LOG(Log, TEXT("%f"), GetCharacterMovement()->GravityScale);
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

void AKraverPlayer::SetViewType(EViewType Type)
{
	ViewType = Type;
	Server_SetViewType(Type);
}

void AKraverPlayer::Server_SetViewType_Implementation(EViewType Type)
{
	ViewType = Type;
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

	TArray<FHitResult> WeaponHitResults;
	FCollisionQueryParams WeaponParams(NAME_None, false, this);

	bool bResult = GetWorld()->LineTraceMultiByChannel(
		WeaponHitResults,
		Camera->GetComponentLocation(),
		Camera->GetComponentLocation() + Camera->GetForwardVector() * Distance,
		ECollisionChannel::ECC_GameTraceChannel1,
		WeaponParams
	);

	if (bResult)
	{
		for(auto& Result : WeaponHitResults)
		{ 
			if (Result.bBlockingHit && IsValid(Result.GetActor()))
			{
				auto Weapon = Cast<AWeapon>(Result.GetActor());
				if(Weapon && Weapon->GetCanInteracted())
				{
					// 찾는 범위에 있는 오브젝트 사이에 다른 오브젝트가 있는지
					FHitResult ObjectHitResult;
					FCollisionQueryParams ObjectParams(NAME_None, false, this);
					bool bObjectResult = GetWorld()->LineTraceSingleByChannel(
						ObjectHitResult,
						Camera->GetComponentLocation(),
						Result.ImpactPoint,
						ECollisionChannel::ECC_GameTraceChannel2,
						ObjectParams
					);
					if (bObjectResult == false || IsValid(ObjectHitResult.GetActor()) == false)
					{
						CanInteractWeapon = Weapon;
					}
				}
			}
		}

	}

	if (CanInteractWeapon == nullptr)
	{
		WeaponHitResults.Empty();
		bResult = GetWorld()->SweepMultiByChannel(
			WeaponHitResults,
			Camera->GetComponentLocation(),
			Camera->GetComponentLocation() + Camera->GetForwardVector() * Distance,
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel1,
			FCollisionShape::MakeSphere(Radius),
			WeaponParams
		);

		if (bResult)
		{
			for (auto& Result : WeaponHitResults)
			{
				if (Result.bBlockingHit && IsValid(Result.GetActor()))
				{
					auto Weapon = Cast<AWeapon>(Result.GetActor());
					if (Weapon && Weapon->GetCanInteracted())
					{
						// 찾는 범위에 있는 오브젝트 사이에 다른 오브젝트가 있는지
						FHitResult ObjectHitResult;
						FCollisionQueryParams ObjectParams(NAME_None, false, this);
						bool bObjectResult = GetWorld()->LineTraceSingleByChannel(
							ObjectHitResult,
							Camera->GetComponentLocation(),
							Result.ImpactPoint,
							ECollisionChannel::ECC_GameTraceChannel2,
							ObjectParams
						);
						if (bObjectResult == false || IsValid(ObjectHitResult.GetActor()) == false)
						{
							CanInteractWeapon = Weapon;
						}
					}
				}
			}
		}
	}

	if (CanInteractWeapon != nullptr && GEngine)
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
	if (Weapon->GetWeaponMesh()->IsSimulatingPhysics() == false)
		return;

	if (CombatComponent->GetIsDead() == false)
	{
		switch (ViewType)
		{
		case EViewType::FIRST_PERSON:
			if (Weapon->GetWeaponMesh()->IsSimulatingPhysics() == true)
			{
				RpcComponent->SetSimulatedPhysics(Weapon->GetWeaponMesh(), false);
				Weapon->GetWeaponMesh()->SetWorldLocation(ArmWeaponMeshes[Weapon]->GetComponentLocation());
				Weapon->GetWeaponMesh()->SetWorldRotation(ArmWeaponMeshes[Weapon]->GetComponentRotation());
				GetWorldTimerManager().SetTimer(
					UnEquipWeaponTimerHandle,
					[=]() {					
						RpcComponent->DetachComponentFromComponent(Weapon->GetWeaponMesh());
						RpcComponent->SetSimulatedPhysics(Weapon->GetWeaponMesh(), true);
						RpcComponent->SetPhysicsLinearVelocity(Weapon->GetWeaponMesh(), FVector::ZeroVector);
						RpcComponent->AddImpulse(Weapon->GetWeaponMesh(), (Camera->GetForwardVector() * WeaponThrowPower * Weapon->GetWeaponMesh()->GetMass()));
						Weapon->GetWeaponMesh()->AddAngularImpulseInDegrees(Camera->GetForwardVector() * WeaponThrowAngularPower ,NAME_None,true);
					},
					0.000001f,
						false);
			}
			break;
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

	bCanDoubleJump = true;
	WallRunEnd(0.f);
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
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Need to support more EWeaponType"));
		break;
	}
	RefreshCurViewType();

	ShowOnlyThirdPerson.Push(Weapon->GetWeaponMesh());
}

void AKraverPlayer::Server_OnEquipWeaponSuccessEvent_Implementation(AWeapon* Weapon)
{
	ASoldier::Server_OnEquipWeaponSuccessEvent_Implementation(Weapon);
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
		default:
			UE_LOG(LogTemp, Fatal, TEXT("Need to support more EWeaponType"));
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
	RefreshCurViewType();
}

void AKraverPlayer::SetMovementState(EMovementState value)
{
	ASoldier::SetMovementState(value);

	switch (value)
	{
		case EMovementState::SPRINT:
			CombatComponent->SetIsAttacking(false);
			break;
		default:
			break;
	}
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
	RpcComponent->Montage_Play(ArmMesh, CombatComponent->GetCurWeapon()->GetAttackMontageFpp(), 1.5f);
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

void AKraverPlayer::Jump()
{
	if (GetMovementComponent()->IsFalling() == false)
		ASoldier::Jump();
	else if(IsWallRunning)
		WallRunJump();
	else if (bCanDoubleJump)
		DoubleJump();
}

void AKraverPlayer::WallRunJump()
{
	if (IsWallRunning)
	{
		WallRunEnd(0.35);
		FVector LaunchVector;
		LaunchVector.X = WallRunNormal.X * WallRunJumpOffForce;
		LaunchVector.Y = WallRunNormal.Y * WallRunJumpOffForce;
		LaunchVector.Z = WallRunJumpHeight;
		LaunchCharacter(LaunchVector, false ,true);
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
	KR_LOG(Log, TEXT("Dobule Jump Power : %f %f"), ForwardLaunchPower, RightLaunchPower);
}

void AKraverPlayer::WallRunUpdate()
{
	if (WallRunMovement(GetActorLocation(), CalculateRightWallRunEndVector(), -1.f))
	{
		if(!IsWallRunning)
			WallRunStart();

		IsWallRunningR = true;
		IsWallRunningL = false;
		GetCharacterMovement()->GravityScale = FMath::FInterpTo(GetCharacterMovement()->GravityScale, WallRunTargetGravity, GetWorld()->GetDeltaSeconds(), 40.f);
	}
	else
	{
		if (IsWallRunningR)
		{
			WallRunEnd(1.f);
		}
		else
		{
			if (WallRunMovement(GetActorLocation(), CalculateLeftWallRunEndVector(), 1.f))
			{
				if (!IsWallRunning)
					WallRunStart();

				IsWallRunningR = false;
				IsWallRunningL = true;
				GetCharacterMovement()->GravityScale = FMath::FInterpTo(GetCharacterMovement()->GravityScale, WallRunTargetGravity, GetWorld()->GetDeltaSeconds(), 40.f);
			}
			else
			{
				WallRunEnd(1.f);
			}
		}
	}
}

bool AKraverPlayer::WallRunMovement(FVector Start, FVector End, float WallRunDirection)
{
	FCollisionQueryParams ObjectParams(NAME_None, false, this);
	FHitResult Result;
	bool bSuccess = GetWorld()->LineTraceSingleByChannel(
		Result,
		Start,
		End, 
		ECollisionChannel::ECC_GameTraceChannel2,
		ObjectParams
	);

	if (bSuccess && Result.bBlockingHit)
	{
		if (GetMovementComponent()->IsFalling() && CalculateValidWallVector(Result.Normal))
		{ 
			WallRunNormal = Result.Normal;
			LaunchCharacter(CalculatePlayerToWallVector(),false ,false);
			
			LaunchCharacter(FVector::CrossProduct(WallRunNormal, FVector(0, 0, 1)) * (WallRunDirection * WallRunSpeed),true, !bWallRunGravity);
			return true;
		}
	}
	return false;
}	

void AKraverPlayer::WallRunStart()
{
	IsWallRunning = true;
	bCanDoubleJump = true;
}

void AKraverPlayer::WallRunEnd(float ResetTime)
{
	if(IsWallRunning == false)
		return;

	IsWallRunning = false;
	IsWallRunningL = false;
	IsWallRunningR = false;
	GetCharacterMovement()->GravityScale = DefaultGravity;	
	SuppressWallRun(ResetTime);
}

void AKraverPlayer::ResetWallRunSuppression()
{
	bWallRunSupressed = false;
	GetWorldTimerManager().ClearTimer(SuppressWallRunTimer);
}

void AKraverPlayer::SuppressWallRun(float Delay)
{
	bWallRunSupressed = true;
	GetWorldTimerManager().SetTimer(SuppressWallRunTimer, this, &AKraverPlayer::ResetWallRunSuppression, Delay, false);
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

bool AKraverPlayer::CalculateValidWallVector(FVector InVec)
{
	return UKismetMathLibrary::InRange_FloatFloat(InVec.Z, -0.52f, 0.52, false, false);
}

FVector AKraverPlayer::CalculatePlayerToWallVector()
{
	return (WallRunNormal * (WallRunNormal - GetActorLocation()).Length());
}

void AKraverPlayer::Server_OnUnEquipWeaponSuccessEvent_Implementation(AWeapon* Weapon)
{
	ASoldier::Server_OnUnEquipWeaponSuccessEvent_Implementation(Weapon);
	
}

