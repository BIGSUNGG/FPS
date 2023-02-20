// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Animation/AnimInstance.h"
#include "Kraver/Creature/Creature.h"
#include "CreatureAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UCreatureAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UCreatureAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Play Montage functions
	virtual void PlayDeathMontage();
	virtual void PlayLandedMontage();
protected:
	ACreature* Creature;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		float Speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		float SpeedRatio;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		float UpDownSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		float MovementDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		bool IsFalling;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		bool IsCrouching;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		bool IsJumping;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		bool IsRunning;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		EMovementState MovementState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Angle, Meta = (AllowPrivateAccess = true))
		float AO_Pitch;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Angle, Meta = (AllowPrivateAccess = true))
		float AO_Yaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Montage, Meta = (AllowPrivateAccess = true))
		UAnimMontage* DeadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Montage, Meta = (AllowPrivateAccess = true))
		UAnimMontage* LandedMontage;
};
