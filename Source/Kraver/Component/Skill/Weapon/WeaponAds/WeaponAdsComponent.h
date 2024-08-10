// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include WeaponComponent_h
#include "WeaponAdsComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KRAVER_API UWeaponAdsComponent : public UWeaponComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

protected:
	// Delegate
	UFUNCTION()
	virtual void OnSubAttackStartEvent();
	UFUNCTION()
	virtual void OnSubAttackEndEvent();

public:
	// Getter Setter
	const float& GetReduceFov() { return ReduceFov; }
	const float& GetInterpSpeed() { return InterpSpeed; }
	const FVector& GetAdsLocation() { return AdsLocation; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ads", Meta = (AllowPrivateAccess = true))
	float ReduceFov = 25.f;	// 조준 시 감소할 Fov값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ads", Meta = (AllowPrivateAccess = true))
	float InterpSpeed = 15.f; // 조준 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ads", meta = (AllowPrivateAccess = "true"))
	FVector AdsLocation; // 조준 시 움직일 위치

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scope", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* ScopeMaterial; // 조준 시 설정할 스코프 머테리얼
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scope", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* OriginScopeMaterial; // 기본 스코프 머테리얼
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scope", meta = (AllowPrivateAccess = "true"))
	int ScopeMaterialIndex; // 스코프 머테리얼을 적용할 머테리얼 인덱스

};
