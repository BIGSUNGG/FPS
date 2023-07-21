// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class KRAVER_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void FireBullet(FVector& Direction, AActor* Target = nullptr);

protected:
	virtual void HitEvent(AActor* OtherActor, UPrimitiveComponent* OtherComponent, const FHitResult& Hit);
	
	UFUNCTION()
		virtual void OnCollisionEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) final;

	virtual void GiveDamage(AActor* OtherActor, UPrimitiveComponent* OtherComponent, const FHitResult& Hit);

public:
	FORCEINLINE void SetDamageType(TSubclassOf<UDamageType> InDamageType) { DamageType = InDamageType; }


public:
	FImpactDele OnImpact;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* BulletMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component", meta = (AllowPrivateAccess = "true"))
		UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Bullet", meta = (AllowPrivateAccess = "true"))
		float BulletDamage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Bullet", meta = (AllowPrivateAccess = "true"))
		int32 MaxHitCount = 1;

	int32 HitCount = 0;
	TSubclassOf<UKraverDamageType> DamageType;
};
