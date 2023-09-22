// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Blueprint/UserWidget.h"
#include "HpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UHpBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

public:
	// Getter Setter
	void SetOwnerCreature(ACreature* Value) { OwnerCreature = Value; }

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HpBar;

	class ACreature* OwnerCreature;
};
