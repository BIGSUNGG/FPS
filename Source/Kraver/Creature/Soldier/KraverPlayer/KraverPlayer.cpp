// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayer.h"
#include "DrawDebugHelpers.h"

AKraverPlayer::AKraverPlayer() : ASoldier()
{
	ArmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMesh"));
	ArmMesh->SetupAttachment(Camera);
	ArmMesh->SetCastShadow(false);
	ArmMesh->SetOnlyOwnerSee(true);

	InteractionWidget = CreateDefaultSubobject<UInteractionWidget>(TEXT("InteractionWidget"));

	ArmWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmWeaponMesh"));
	ArmWeaponMesh->SetCastShadow(false);
	ArmWeaponMesh->SetOnlyOwnerSee(true);

}

void AKraverPlayer::BeginPlay()
{
	ASoldier::BeginPlay();

	ChangeView();
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

void AKraverPlayer::CrouchButtonPressed()
{
	ASoldier::CrouchButtonPressed();

	if (bIsCrouched)
	{
		SpringArmAdditiveLocation += FVector(0.f, 0.f, 20.f);
		RefreshSpringArm();
	}
	else
	{
		SpringArmAdditiveLocation += FVector(0.f, 0.f, -20.f);
		RefreshSpringArm();
	}
}

void AKraverPlayer::EquipButtonPressed()
{
	if(CanInteractWeapon == nullptr) 
		return;	

	EqiupWeapon(CanInteractWeapon);                 
}

void AKraverPlayer::CheckCanInteractionWeapon()
{
	if (InteractionWidget == nullptr)                                                       
	{
		//UE_LOG(LogTemp, Fatal , TEXT("InteractionWidget is nullptr"));
	}
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	float Radius = 5.f;
	float Distance = InteractionDistance + SpringArm->TargetArmLength;
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		Camera->GetComponentLocation(),
		Camera->GetComponentLocation() + Camera->GetForwardVector() * Distance,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	CanInteractWeapon = nullptr;

	bool bCheckedInterctionObject = false;
	if (bResult)
	{
		if (IsValid(HitResult.GetActor()))
		{
			auto weapon = Cast<AWeapon>(HitResult.GetActor());
			if(weapon && weapon->GetCanInteract())
			{ 
				CanInteractWeapon = weapon;
				bCheckedInterctionObject = true;
				if(InteractionWidget && !InteractionWidget->IsVisible())
					InteractionWidget->AddToViewport();
			}
		}
	}

	if(InteractionWidget && !bCheckedInterctionObject && InteractionWidget->IsVisible())
		InteractionWidget->RemoveFromParent();
}

void AKraverPlayer::ChangeView()
{
	switch (ViewType)
	{
	case EViewType::FIRST_PERSON:
		ViewType = EViewType::THIRD_PERSON;
		SpringArm->TargetArmLength = 300.f;
		SpringArmBasicLocation = FVector(-80.0, 0.f, 150.0);
		RefreshSpringArm();
		GetMesh()->SetOwnerNoSee(false);
		ArmMesh->SetOwnerNoSee(true);
		ArmWeaponMesh->SetOwnerNoSee(true);
		if (CurWeapon)
			CurWeapon->GetWeaponMesh()->SetOwnerNoSee(false);
		break;
	case EViewType::THIRD_PERSON:
		ViewType = EViewType::FIRST_PERSON;
		SpringArm->TargetArmLength = 0.f;
		SpringArmBasicLocation = FVector(0.f, 0.f, 165.f);
		RefreshSpringArm();
		GetMesh()->SetOwnerNoSee(true);
		ArmMesh->SetOwnerNoSee(false);
		ArmWeaponMesh->SetOwnerNoSee(false);
		if(CurWeapon)
			CurWeapon->GetWeaponMesh()->SetOwnerNoSee(true);
		break;
	default:
		break;
	}
}

void AKraverPlayer::RefreshSpringArm()
{
	SpringArm->SetRelativeLocation(SpringArmBasicLocation + SpringArmAdditiveLocation);
}

void AKraverPlayer::RefreshCurViewType()
{
	for(int i = 0; i < 2; i++)
		ChangeView();
}

void AKraverPlayer::EqiupWeapon(AWeapon* Weapon)
{	
	if(!Weapon)
		return;

	ASoldier::EqiupWeapon(Weapon);
	ArmWeaponMesh->SetSkeletalMesh(CurWeapon->GetWeaponMesh()->GetSkeletalMeshAsset());
	ArmWeaponMesh->AttachToComponent(ArmMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, CurWeapon->GetAttachSocketName());
	RefreshCurViewType();
}