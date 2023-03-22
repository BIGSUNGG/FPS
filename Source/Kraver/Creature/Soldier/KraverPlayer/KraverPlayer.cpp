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
#include <cmath>

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

	RefreshCurViewType();
}

void AKraverPlayer::BeginPlay()
{
	ASoldier::BeginPlay();

	if (IsLocallyControlled())
	{ 
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

	if (HasAuthority() == false && IsLocallyControlled() == false)
		Camera->SetRelativeRotation(FRotator(AO_Pitch, AO_Yaw, 0.0f));

	ClientEvent(DeltaTime);
	ServerClientEvent(DeltaTime);
	LocallyControlEvent(DeltaTime);
}

void AKraverPlayer::ClientEvent(float DeltaTime)
{
	if(IsLocallyControlled())
		return;

}

void AKraverPlayer::ServerClientEvent(float DeltaTime)
{
	if(!HasAuthority())
		return;
}

void AKraverPlayer::LocallyControlEvent(float DeltaTime)
{
	if(!IsLocallyControlled())
		return;

	CheckCanInteractionWeapon();
	if (GetMovementComponent()->IsCrouching())
		SpringArmCrouchLocation.Z = FMath::FInterpTo(SpringArmCrouchLocation.Z, CrouchCameraHeight, DeltaTime, 20.f);
	else
		SpringArmCrouchLocation.Z = FMath::FInterpTo(SpringArmCrouchLocation.Z, UnCrouchCameraHeight, DeltaTime, 20.f);

	SpringArm->SetRelativeLocation(SpringArmBasicLocation + SpringArmCrouchLocation);

	if (CombatComponent->GetCurWeapon() && CombatComponent->GetCurWeapon()->GetIsSubAttacking())
	{
		USkeletalMeshComponent* ArmWeaponMesh = GetArmWeaponMeshes()[CombatComponent->GetCurWeapon()];
		ArmWeaponMesh->AddRelativeRotation(WeaponAdsRotation * -1);

		FTransform WeaponTransform = ArmWeaponMesh->GetSocketTransform("SOCKET_AIM", ERelativeTransformSpace::RTS_World);
		FTransform CameraTransform = Camera->GetComponentTransform();
		FTransform RelativeTransform = WeaponTransform.GetRelativeTransform(CameraTransform);
		FRotator RelativeRotation = RelativeTransform.Rotator();

		WeaponAdsRotation.Pitch = -RelativeRotation.Pitch;
		WeaponAdsRotation.Roll = -RelativeRotation.Roll;
		WeaponAdsRotation.Yaw = -(90.f + RelativeRotation.Yaw);

		ArmWeaponMesh->AddRelativeRotation(WeaponAdsRotation);
	}

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
		GetArmWeaponMeshes()[CombatComponent->GetCurWeapon()]->SetRelativeRotation(FRotator::ZeroRotator);
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

void AKraverPlayer::ReloadButtonPressed()
{
	ASoldier::ReloadButtonPressed();

	if (CombatComponent->GetCurWeapon() == nullptr ||
		CombatComponent->GetCurWeapon()->GetCanReload() == false ||
		ArmMesh->GetAnimInstance()->Montage_IsPlaying(CombatComponent->GetCurWeapon()->GetReloadMontageFpp()) == true)
	{
		return;
	}

	ArmMesh->GetAnimInstance()->Montage_Play(CombatComponent->GetCurWeapon()->GetReloadMontageFpp(), 1.5f);
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
	UCreatureAnimInstance* CreatureAnim = Cast<UCreatureAnimInstance>(ArmMesh->GetAnimInstance());
	CreatureAnim->PlayLandedMontage();
	ASoldier::Landed(Hit);
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

void AKraverPlayer::OnCurWeaponAttackEvent()
{
	ASoldier::OnCurWeaponAttackEvent();

	RpcComponent->Montage_Play(ArmMesh, CombatComponent->GetCurWeapon()->GetAttackMontageFpp());
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

void AKraverPlayer::Server_OnUnEquipWeaponSuccessEvent_Implementation(AWeapon* Weapon)
{
	ASoldier::Server_OnUnEquipWeaponSuccessEvent_Implementation(Weapon);
	
}

