// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayer.h"
#include "Kraver/Weapon/Gun/Gun.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

AKraverPlayer::AKraverPlayer() : ASoldier()
{
	ArmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMesh"));
	ArmMesh->SetupAttachment(Camera);
	ArmMesh->SetCastShadow(false);
	ArmMesh->SetOnlyOwnerSee(true);
	ArmMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShowOnlyFirstPerson.Push(ArmMesh);

	static ConstructorHelpers::FClassFinder<UInteractionWidget> WBP_INTERACTION(TEXT("/Game/ProjectFile/HUD/WBP_InteractionWidget.WBP_InteractionWidget_C"));
	if (WBP_INTERACTION.Succeeded())
	{
		InteractionWidget = WBP_INTERACTION.Class.GetDefaultObject();
	}

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

void AKraverPlayer::CrouchButtonPressed()
{
	if (GetCharacterMovement()->IsFalling())
		return;

	ASoldier::CrouchButtonPressed();

	if (bIsCrouched)
	{
		SpringArmAdditiveLocation = (FVector(0.f, 0.f, 0.f));
		RefreshSpringArm();
	}
	else 
	{
		SpringArmAdditiveLocation = (FVector(0.f, 0.f, -20.f));
		RefreshSpringArm();
	}
}

void AKraverPlayer::EquipButtonPressed()
{
	if (CombatComponent->GetCurWeapon() == nullptr)
	{
		if(CanInteractWeapon != nullptr)
			EquipWeapon(CanInteractWeapon);
	}
	else
	{
		CombatComponent->UnEquipWeapon(CombatComponent->GetCurWeapon());
	}
}

void AKraverPlayer::CheckCanInteractionWeapon()
{
	CanInteractWeapon = nullptr;

	float Radius = InteractionRadius;
	float Distance = InteractionDistance + SpringArm->TargetArmLength;

	TArray<FHitResult> WeaponHitResults;
	FCollisionQueryParams WeaponParams(NAME_None, false, this);
	bool bResult = GetWorld()->SweepMultiByChannel(
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
		for(auto& Result : WeaponHitResults)
		{ 
			if (IsValid(Result.GetActor()))
			{
				auto Weapon = Cast<AWeapon>(Result.GetActor());
				if(Weapon && Weapon->GetCanInteracted() && CombatComponent->GetCanEquipWeapon())
				{ 
					FHitResult ObjectHitResult;
					FCollisionQueryParams ObjectParams(NAME_None, false, this);
					bool bObjectResult = GetWorld()->SweepSingleByChannel(
						ObjectHitResult,
						Camera->GetComponentLocation(),
						Result.ImpactPoint,
						FQuat::Identity,
						ECollisionChannel::ECC_GameTraceChannel2,
						FCollisionShape::MakeSphere(0.f),
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

	if (CanInteractWeapon != nullptr)
	{
		if (InteractionWidget && !InteractionWidget->IsVisible())
			InteractionWidget->AddToViewport();
	}
	else if(InteractionWidget && InteractionWidget->IsVisible())
		InteractionWidget->RemoveFromParent();
}

void AKraverPlayer::ChangeView()
{
	switch (ViewType)
	{
	case EViewType::FIRST_PERSON:
		SetViewType(EViewType::THIRD_PERSON);
		SpringArm->TargetArmLength = 300.f;
		SpringArmBasicLocation = FVector(-80.0, 0.f, 150.0);
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
		SpringArmBasicLocation = FVector(0.f, 0.f, 164.f);
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

void AKraverPlayer::OnEquipWeaponSuccess(AWeapon* Weapon)
{	
	if(!Weapon)
		return;
	ASoldier::OnEquipWeaponSuccess(Weapon);

	ArmWeaponMesh->SetHiddenInGame(false);
	ArmWeaponMesh->SetSkeletalMesh(CombatComponent->GetCurWeapon()->GetWeaponMesh()->GetSkeletalMeshAsset());
	ServerComponent->AttachComponentToComponent(ArmWeaponMesh,ArmMesh, CombatComponent->GetCurWeapon()->GetAttachSocketName());
	ShowOnlyThirdPerson.Push(CombatComponent->GetCurWeapon()->GetWeaponMesh());

	int32 Index = CombatComponent->GetCurWeapon()->AddAdditiveWeaponMesh(ArmWeaponMesh);
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

void AKraverPlayer::Server_OnEquipWeaponSuccess_Implementation(AWeapon* Weapon)
{
	ASoldier::Server_OnEquipWeaponSuccess_Implementation(Weapon);

	ArmWeaponMesh->SetSkeletalMesh(CombatComponent->GetCurWeapon()->GetWeaponMesh()->GetSkeletalMeshAsset());
}

void AKraverPlayer::OnUnEquipWeaponSuccess(AWeapon* Weapon)
{
	ASoldier::OnUnEquipWeaponSuccess(Weapon);
	ArmWeaponMesh->SetHiddenInGame(true);
	ShowOnlyThirdPerson.Remove(Weapon->GetWeaponMesh());

	Weapon->GetWeaponMesh()->SetOwnerNoSee(false);
	if (!HasAuthority() && ViewType == EViewType::FIRST_PERSON)
	{
		Weapon->GetWeaponMesh()->SetSimulatePhysics(false);
		Weapon->GetWeaponMesh()->AttachToComponent(ArmWeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
	}
	Weapon->RemoveAdditiveWeaponMesh(ArmWeaponMesh);
}

void AKraverPlayer::Server_OnUnEquipWeaponSuccess_Implementation(AWeapon* Weapon)
{
	ASoldier::Server_OnUnEquipWeaponSuccess_Implementation(Weapon);

	switch (ViewType)
	{
	case EViewType::FIRST_PERSON:
		if (Weapon->GetWeaponMesh()->IsSimulatingPhysics() == true)
		{
			ServerComponent->SetSimulatedPhysics(Weapon->GetWeaponMesh(), false);
			ServerComponent->AttachComponentToComponent(Weapon->GetWeaponMesh(), ArmWeaponMesh);
			GetWorldTimerManager().SetTimer(
				UnEquipWeaponTimerHandle,
				[=]() {
					Weapon->GetWeaponMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
					ServerComponent->SetSimulatedPhysics(Weapon->GetWeaponMesh(),true);
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

