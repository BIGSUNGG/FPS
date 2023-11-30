// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlHud.h"
#include ControlGameWidget_h

AControlHud::AControlHud()
{
	static ConstructorHelpers::FClassFinder<UControlGameWidget> CONTROL_WIDGET(TEXT("/Game/ProjectFile/GameBase/Widget/WBP_CombatWidget.WBP_CombatWidget_C"));
	if (CONTROL_WIDGET.Succeeded())
		ControlWidgetClass = CONTROL_WIDGET.Class;
}

void AControlHud::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetOwningPlayerController();
	ControlWidget = CreateWidget<UUserWidget>(PlayerController, ControlWidgetClass);
	if (ControlWidget)
		ControlWidget->AddToViewport();

}
