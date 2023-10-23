// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFinishWidget.h"

void UGameFinishWidget::GameFinish(bool bWin)
{
	bWin ? VictoryEvent() : DefeatEvent();

	GameFinishText->SetVisibility(ESlateVisibility::Visible);
}

void UGameFinishWidget::VictoryEvent_Implementation()
{
	GameFinishText->SetText(FText::FromString("Victory"));
}

void UGameFinishWidget::DefeatEvent_Implementation()
{
	GameFinishText->SetText(FText::FromString("Defeat"));
}
