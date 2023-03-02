// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayer.h"
#include "Kraver/Weapon/Gun/Gun.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Kraver/HUD/KraverHud.h"
#include "Kraver/PlayerController/KraverPlayerController.h"
#include "Kraver/GameMode/KraverGameMode.h"
#include "Kraver/Anim/Creature/CreatureAnimInstance.h"

AKraverPlayer::AKraverPlayer() : ASoldier()
{
	ArmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMesh"));
	ArmMesh->SetupAttachment(Camera);
	ArmMesh->SetCastShadow(false);
	ArmMesh->SetOnlyOwnerSee(true);
	ArmMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShowOnlyFirstPerson.Push(ArmMesh);

	ArmWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmWeaponMesh"));
	ArmWeaponMesh->SetCastShadow(false);
	ArmWeaponMesh->SetOnlyOwnerSee(true);
	ArmWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShowOnlyFirstPerson.Push(ArmWeaponMesh);

	ShowOnlyThirdPerson.Push(GetMesh());
}

void AKraverPlayer::BeginPlay()
{
	ASoldier::BeginPlay();

	if(IsLocallyControlled())
		RefreshCurViewType();
}

void AKraverPlayer::Tick(float DeltaTime)
{
	ASoldier::Tick(DeltaTime);
	
	CheckCanInteractionWeapon();
}

void AKraverPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	ASoldier::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("ChangeView"), EInputEvent::IE_Pressed, this, &AKraverPlayer::ChangeView);
	PlayerInputComponent->BindAction(TEXT("Equip"), EInputEvent::IE_Pressed, this, &AKraverPlayer::EquipButtonPressed);
	PlayerInputComponent->BindAction(TEXT("UnEquip"), EInputEvent::IE_Pressed, this, &AKraverPlayer::UnEquipButtonPressed);
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

void AKraverPlayer::SetUnEquipWeaponThrowPower(float Value)
{
	UnEquipWeaponThrowPower = Value;
	Server_SetUnEquipWeaponThrowPower(Value);
}

void AKraverPlayer::Server_SetUnEquipWeaponThrowPower_Implementation(float Value)
{
	UnEquipWeaponThrowPower = Value;
}

void AKraverPlayer::EquipButtonPressed()
{
	if (CombatComponent->GetCurWeapon() == nullptr)
	{
		if(CanInteractWeapon != nullptr)
			EquipWeapon(CanInteractWeapon);
	}
}

void AKraverPlayer::UnEquipButtonPressed()
{
	if (CombatComponent->GetCurWeapon() != nullptr)
		CombatComponent->UnEquipWeapon(CombatComponent->GetCurWeapon());

}

void AKraverPlayer::CheckCanInteractionWeapon()
{
	if(IsLocallyControlled() == false)
		return;

	CanInteractWeapon = nullptr;

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
				if(Weapon && Weapon->GetCanInteracted() && CombatComponent->GetCanEquipWeapon())
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
					if (Weapon && Weapon->GetCanInteracted() && CombatComponent->GetCanEquipWeapon())
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

	KraverController = KraverController == nullptr ? Cast<AKraverPlayerController>(Controller) : KraverController;
	if(KraverController)
	{ 
		HUD = HUD == nullptr ? Cast<AKraverHUD>(KraverController->GetHUD()) : HUD;
		if(HUD)
		{
			if (CanInteractWeapon == nullptr)
			{
				HUD->SetInteractWidget(false);
			}
			else
			{
				HUD->SetInteractWidget(true);
			}
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
		SpringArmBasicLocation = FVector(0.f, 80.f, 62.f);
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
		SpringArmBasicLocation = FVector(0.f, 0.f, 76.f);
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

void AKraverPlayer::OnDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ASoldier::OnDeathEvent(DamageAmount,DamageEvent,EventInstigator,DamageCauser);
}

void AKraverPlayer::RefreshSpringArm()
{
	SpringArm->SetRelativeLocation(SpringArmBasicLocation + SpringArmAdditiveLocation);
	Server_RefreshSpringArm(SpringArmBasicLocation + SpringArmAdditiveLocation, SpringArm->TargetArmLength);
}

void AKraverPlayer::Server_RefreshSpringArm_Implementation(FVector Vector, float Length)
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

	int32 Index = CombatComponent->GetCurWeapon()->AddAdditiveWeaponMesh(ArmWeaponMesh);

	ArmWeaponMesh->SetHiddenInGame(false);
	ShowOnlyThirdPerson.Push(CombatComponent->GetCurWeapon()->GetWeaponMesh());
	ServerComponent->AttachComponentToComponent(ArmWeaponMesh,ArmMesh, WeaponAttachSocketName);

	switch (CombatComponent->GetCurWeapon()->GetWeaponType())
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
}

void AKraverPlayer::Server_OnEquipWeaponSuccessEvent_Implementation(AWeapon* Weapon)
{
	ASoldier::Server_OnEquipWeaponSuccessEvent_Implementation(Weapon);

	ArmWeaponMesh->SetSkeletalMesh(CombatComponent->GetCurWeapon()->GetWeaponMesh()->GetSkeletalMeshAsset());
}

void AKraverPlayer::OnUnEquipWeaponSuccessEvent(AWeapon* Weapon)
{
	ASoldier::OnUnEquipWeaponSuccessEvent(Weapon);
	ArmWeaponMesh->SetHiddenInGame(true);
	ShowOnlyThirdPerson.Remove(Weapon->GetWeaponMesh());

	Weapon->GetWeaponMesh()->SetOwnerNoSee(false);
	if (!HasAuthority() && ViewType == EViewType::FIRST_PERSON)
	{
		Weapon->GetWeaponMesh()->SetSimulatePhysics(false);
		Weapon->GetWeaponMesh()->AttachToComponent(ArmWeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
	}

	int32 Index = Weapon->FindAdditiveWeaponMesh(ArmWeaponMesh);
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
	Weapon->RemoveAdditiveWeaponMesh(ArmWeaponMesh);
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
	
	if (CombatComponent->IsDead() == false)
	{
		switch (ViewType)
		{
		case EViewType::FIRST_PERSON:
			if (Weapon->GetWeaponMesh()->IsSimulatingPhysics() == true)
			{
				ServerComponent->SetSimulatedPhysics(Weapon->GetWeaponMesh(), false);
				Weapon->GetWeaponMesh()->AttachToComponent(ArmWeaponMesh,FAttachmentTransformRules::SnapToTargetIncludingScale);
				GetWorldTimerManager().SetTimer(
					UnEquipWeaponTimerHandle,
					[=]() {
						Weapon->GetWeaponMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				ServerComponent->SetSimulatedPhysics(Weapon->GetWeaponMesh(), true);
				ServerComponent->SetPhysicsLinearVelocity(Weapon->GetWeaponMesh(), FVector::ZeroVector);
				ServerComponent->AddImpulse(Weapon->GetWeaponMesh(), (Camera->GetForwardVector() + FVector(0, 0, 0.35f)) * UnEquipWeaponThrowPower * Weapon->GetWeaponMesh()->GetMass());
					},
					0.000001f,
						false);
			}
			break;
		case EViewType::THIRD_PERSON:
			Weapon->GetWeaponMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
			Weapon->GetWeaponMesh()->AddImpulse(Camera->GetForwardVector() * UnEquipWeaponThrowPower, NAME_None, true);
			break;
		default:
			UE_LOG(LogTemp, Fatal, TEXT("Need to support more EViewType"));
			break;
		}
	}
}

