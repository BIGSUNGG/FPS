// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/Actor.h"
#include "FloatingDamage.generated.h"

UCLASS()
class KRAVER_API AFloatingDamage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingDamage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ShowFloatingDamage(AActor* DamagedActor, FDamageEvent const& DamageEvent, FKraverDamageResult const& DamageResult);

	virtual void OnDestroyTimerEvent();

public:
	class ACreature* GetInchargeActor() { return InchargeActor; }

public:
	FDestroyDele OnDestroy;

protected:
	class ACreature* InchargeActor = nullptr;

	UWidgetComponent* FloatingWidgetComp;
	float FloatingRightSpeed = 1.f;
	float FloatingUpSpeed = 1.5f;
	FVector RightVector;
	FVector UpVector;

	FTimerHandle DestroyTimer;
	float LifeTime = 0.5f;

};
