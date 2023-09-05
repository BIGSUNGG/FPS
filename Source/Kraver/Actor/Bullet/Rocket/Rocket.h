// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include Bullet_h
#include "Rocket.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API ARocket : public ABullet
{
	GENERATED_BODY()
	
protected:
    virtual void GiveDamage(AActor* OtherActor, UPrimitiveComponent* OtherComponent, const FHitResult& Hit) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Explosion", meta = (AllowPrivateAccess = "true"))
	float ExplosionMaxDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Explosion", meta = (AllowPrivateAccess = "true"))
	float ExplosionMinDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Explosion", meta = (AllowPrivateAccess = "true"))
	float ExplosionInnerRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Explosion", meta = (AllowPrivateAccess = "true"))
	float ExplosionOuterRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Explosion", meta = (AllowPrivateAccess = "true"))
	float ExplosionFalloff = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UKraverDamageType> ExplosionDamageType;
};
