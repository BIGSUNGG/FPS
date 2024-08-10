// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/PlayerState.h"
#include "KraverPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AKraverPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AKraverPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Delegate
	UFUNCTION()
	virtual void OnPawnSetEvent(APlayerState* Player, APawn* NewPawn, APawn* OldPawn) final; // ���ο� Player ĳ���Ͱ� �����Ǿ����� ȣ���

	// Func
	virtual void OnNewPawn(APawn* NewPawn);

public:
	class AKraverPlayer* GetOwnerPLayer();

	void SetDefaultWeapons(const TSubclassOf<class AWeapon>& InValue, int Index);

public:
	FNewKraverPlayer OnNewPlayer;

protected:
	class AKraverPlayer* OwnerPlayer = nullptr; 
	class AKraverPlayerController* OwnerController = nullptr;

	// Default Weapon
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Camera", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class AWeapon>> DefaultWeapons; // �⺻ ����
};
