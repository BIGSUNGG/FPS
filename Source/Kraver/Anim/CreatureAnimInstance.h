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

protected:
	ACreature* Creature;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		float SpeedRatio;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		float UpDownSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		float MovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		bool IsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		bool IsCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		bool IsRunning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Angle, Meta = (AllowPrivateAccess = true))
		float VerticalAngle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Angle, Meta = (AllowPrivateAccess = true))
		float HorizonAngle;
};
