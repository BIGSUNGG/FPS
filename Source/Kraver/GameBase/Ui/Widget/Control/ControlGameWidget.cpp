// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlGameWidget.h"
#include ControlGameState_h

void UControlGameWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	ControlGameState = ControlGameState ? ControlGameState : Cast<AControlGameState>(UGameplayStatics::GetGameState(this));
	if (!ControlGameState)
		return;

	RedTeamPointText->SetText(FText::FromString(FString::FromInt(ControlGameState->GetRedTeamPoint()) + "%"));
	BlueTeamPointText->SetText(FText::FromString(FString::FromInt(ControlGameState->GetBlueTeamPoint()) + "%"));

	ControlPointSlider->SetValue(ControlGameState->GetCurControlPoint());
	switch(ControlGameState->GetCurControlTeam())
	{
	case ETeam::RED:
		ControlPointSlider->SetSliderProgressColor(RedTeamColor);
		break;
	case ETeam::BLUE:
		ControlPointSlider->SetSliderProgressColor(BlueTeamColor);
		break;
	default:
		break;
	}
}
