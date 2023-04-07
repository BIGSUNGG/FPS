// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Animation/AnimInstance.h"
#include "CreatureAnimInstance.generated.h"

/**
 * 
 */

class ACreature;
enum class EMovementState : uint8;

UCLASS()
class KRAVER_API UCreatureAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UCreatureAnimInstance();
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 몽타주 실행 함수
	virtual void PlayLandedMontage();

public:
	// Getter Setter
	UAnimMontage* GetLandedMontage() { return LandedMontage; }

protected:
	ACreature* Creature;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		float Speed; // 현재 캐릭터의 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		float SpeedRatio; // 현재 속도와 캐릭터의 최대속도 비율
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		float UpDownSpeed; // 현재 캐릭터의 Z축 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		float ForwardSpeed; // 현재 캐릭터가 바라보는 방향기준으로 앞으로 움직이고 있는 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		float RightSpeed; // 현재 캐릭터가 바라보는 방향기준으로 앞으로 움직이고 있는 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		bool IsFalling; // 캐릭터가 떨어지고 있는지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		bool IsCrouching; // 캐릭터가 앉아있는지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		bool IsJumping; // 캐릭터가 점프를 하고있는지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		bool IsRunning; // 캐릭터가 달리고 있는지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		EMovementState MovementState; // 캐릭터의 움직임 상태

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Angle, Meta = (AllowPrivateAccess = true))
		float AO_Pitch; // 캐릭터가 바라보는 방향기준으로 카메라 Pitch의 차이
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Angle, Meta = (AllowPrivateAccess = true))
		float AO_Yaw; // 캐릭터가 바라보는 방향기준으로 카메라 Yaw의 차이

	// Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Montage, Meta = (AllowPrivateAccess = true))
		UAnimMontage* LandedMontage;
};
