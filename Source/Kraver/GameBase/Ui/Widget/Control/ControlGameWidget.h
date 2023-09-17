// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Blueprint/UserWidget.h"
#include "ControlGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UControlGameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

protected:
	class AControlGameState* ControlGameState = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* RedTeamPointText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* RedTeamBorder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* BlueTeamPointText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBorder* BlueTeamBorder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	URadialSlider* ControlPointSlider;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color", meta = (AllowPrivateAccess = "true"))
	FLinearColor BlueTeamColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color", meta = (AllowPrivateAccess = "true"))
	FLinearColor RedTeamColor;
};
