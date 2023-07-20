// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Actor/Bullet/Bullet.h"
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
    float ExplosionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Explosion", meta = (AllowPrivateAccess = "true"))
    float ExplosionDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Explosion", meta = (AllowPrivateAccess = "true"))
    UCurveFloat* ExpolosionDecreaseCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Explosion", meta = (AllowPrivateAccess = "true"))
    class UParticleSystem* ExplosionEffect;
};
