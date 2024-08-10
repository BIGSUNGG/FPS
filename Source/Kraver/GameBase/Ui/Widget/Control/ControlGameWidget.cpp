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

	// ���� �õ� ���� �� ����
	switch(ControlGameState->GetTryControlTeam())
	{
	case ETeam::RED:
		ControlPointSlider->SetSliderProgressColor(RedTeamColor);
		break;
	case ETeam::BLUE:
		ControlPointSlider->SetSliderProgressColor(BlueTeamColor);
		break;
	case ETeam::NONE:
		ControlPointSlider->SetSliderProgressColor(NoneTeamColor);
		break;
	default:
		break;
	}

	// ���� ������ ������ �� ����
	switch (ControlGameState->GetCurControlTeam())
	{
	case ETeam::RED:
		ControlPointSlider->SetSliderBarColor(RedTeamColor);
		break;
	case ETeam::BLUE:
		ControlPointSlider->SetSliderBarColor(BlueTeamColor);
		break;
	case ETeam::NONE:
		ControlPointSlider->SetSliderBarColor(NoneTeamColor);
		break;
	default:
		break;
	}
}
