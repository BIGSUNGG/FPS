// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/Actor.h"
#include "ControlArea.generated.h"

UCLASS()
class KRAVER_API AControlArea : public AActor
{
	GENERATED_BODY()
	
public:	
	AControlArea();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	const int& GetBlueTeamCount() { return BlueTeamCount; }
	const int& GetRedTeamCount() { return RedTeamCount; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* AreaRangeComponent;

	UPROPERTY(Replicated ,VisibleAnywhere, BlueprintReadOnly, Category = "Data|Control", meta = (AllowPrivateAccess = "true"))
	int BlueTeamCount = 0;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Data|Control", meta = (AllowPrivateAccess = "true"))
	int RedTeamCount = 0;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Data|Control", meta = (AllowPrivateAccess = "true"))
	bool IsControlling = false;
};
