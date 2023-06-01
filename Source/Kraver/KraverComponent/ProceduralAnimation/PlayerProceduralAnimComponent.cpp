// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerProceduralAnimComponent.h"
#include "Kraver/Creature/Creature.h"

// Sets default values for this component's properties
UPlayerProceduralAnimComponent::UPlayerProceduralAnimComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerProceduralAnimComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerCreature = Cast<ACreature>(GetOwner());

	if(OwnerCreature == nullptr)
		return;

	OwnerCreature->OnLand.AddDynamic(this, &ThisClass::OnLandEvent);
	OwnerCreature->OnJump.AddDynamic(this, &ThisClass::OnJumpEvent);
	OwnerCreature->OnCrouchStart.AddDynamic(this, &ThisClass::OnStartCrouchEvent);
	OwnerCreature->OnCrouchEnd.AddDynamic(this, &ThisClass::OnEndCrouchEvent);
}


// Called every frame
void UPlayerProceduralAnimComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	ProceduralAnimTimeLine.TickTimeline(DeltaTime);
}

void UPlayerProceduralAnimComponent::StartProceduralAnim(float Strength)
{
	ProceduralAnimStrength = Strength;

	FOnTimelineFloat TimelineCallback;
	TimelineCallback.BindUFunction(this, FName("ProceduralAnimEvent"));

	ProceduralAnimTimeLine.AddInterpFloat(ProceduralAnimCurve, TimelineCallback);
	ProceduralAnimTimeLine.PlayFromStart();
}

void UPlayerProceduralAnimComponent::ProceduralAnimEvent()
{
	float TimelineValue = ProceduralAnimTimeLine.GetPlaybackPosition();
	float CurveFloatValue = ProceduralAnimCurve->GetFloatValue(TimelineValue);

	ProceduralAnimResultVec.Z = UKismetMathLibrary::Lerp(0.f, -10.f, CurveFloatValue * ProceduralAnimStrength);

	if (OwnerCreature->CombatComponent->GetCurWeapon() && OwnerCreature->CombatComponent->GetCurWeapon()->GetIsSubAttacking())
		ProceduralAnimResultVec.Z *= 0.1f;
}

void UPlayerProceduralAnimComponent::OnLandEvent(const FHitResult& Result)
{
	StartProceduralAnim(0.75f);
}

void UPlayerProceduralAnimComponent::OnJumpEvent()
{
	StartProceduralAnim(-0.75f);
}

void UPlayerProceduralAnimComponent::OnStartCrouchEvent(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	StartProceduralAnim(-0.5f);
}

void UPlayerProceduralAnimComponent::OnEndCrouchEvent(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	StartProceduralAnim(0.5f);
}
