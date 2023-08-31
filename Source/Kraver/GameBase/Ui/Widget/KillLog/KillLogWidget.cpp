// Fill out your copyright notice in the Description page of Project Settings.


#include "KillLogWidget.h"

void UKillLogWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	CurLifeTime -= DeltaTime;

	if(CurLifeTime > 0)
	{
		AttackerNameText->SetText(FText::FromString(AttackerName));
		VictimNameText->SetText(FText::FromString(VictimName));
	}
	else
	{
		CurLifeTime = 0.f;
		SetVisibility(ESlateVisibility::Hidden);
	}
}

bool UKillLogWidget::Initialize(const FString& InAttackerName, const FString& InVictimName)
{
	CurLifeTime = 5.f;
	AttackerName = InAttackerName;
	VictimName = InVictimName;
	SetVisibility(ESlateVisibility::Visible);

	return true;
}
