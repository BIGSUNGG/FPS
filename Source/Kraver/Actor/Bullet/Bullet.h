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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void FireBullet(FVector& Direction, AActor* Target = nullptr);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* BulletMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component", meta = (AllowPrivateAccess = "true"))
		UProjectileMovementComponent* ProjectileMovementComponent;

};
