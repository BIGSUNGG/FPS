// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Blueprint/UserWidget.h"
#include "RespawnTimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API URespawnTimerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	URespawnTimerWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	void TimerStart();

protected:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	URadialSlider* RespawnTimeSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* RespawnTimeText;

	float RemainRespawnTime;

};
