// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Components/ActorComponent.h"
#include "PlayerProceduralAnimComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API UPlayerProceduralAnimComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerProceduralAnimComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	void StartProceduralAnim(float Strength);
	UFUNCTION()
		void ProceduralAnimEvent();

	UFUNCTION()
		virtual void OnLandEvent(const FHitResult& Result);
	UFUNCTION()
		virtual void OnJumpEvent();
	UFUNCTION()
		virtual void OnStartCrouchEvent(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	UFUNCTION()
		virtual void OnEndCrouchEvent(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
public:
	// Getter Setter
	FVector GetProceduralAnimResultVec() { return ProceduralAnimResultVec; }

protected:
	class ACreature* OwnerCreature = nullptr;

	FVector ProceduralAnimResultVec;
	FTimeline ProceduralAnimTimeLine;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Curve", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ProceduralAnimCurve;
	float ProceduralAnimStrength = 0.f;
		
};
