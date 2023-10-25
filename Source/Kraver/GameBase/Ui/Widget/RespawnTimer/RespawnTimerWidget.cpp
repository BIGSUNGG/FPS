// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnTimerWidget.h"
#include KraverGameMode_h

URespawnTimerWidget::URespawnTimerWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetVisibility(ESlateVisibility::Hidden);
}

void URespawnTimerWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (GetVisibility() == ESlateVisibility::Visible)
	{
		RemainRespawnTime -= DeltaTime;
		if (RemainRespawnTime <= 0.f)
		{
			SetVisibility(ESlateVisibility::Hidden);
			RemainRespawnTime = 0.f;
		}

		RespawnTimeSlider->SetValue((RemainRespawnTime / AKraverGameMode::GetRespawnTime()));
		RespawnTimeText->SetText(UKismetTextLibrary::Conv_IntToText((int)RemainRespawnTime));
	}
}

void URespawnTimerWidget::TimerStart()
{
	SetVisibility(ESlateVisibility::Visible);
	RemainRespawnTime = AKraverGameMode::GetRespawnTime();
}
