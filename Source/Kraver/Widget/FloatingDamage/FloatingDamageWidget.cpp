// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingDamageWidget.h"

void UFloatingDamageWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

}

void UFloatingDamageWidget::SetDamage(int32 Damage)
{
	DamageText->SetText(UKismetTextLibrary::Conv_IntToText(Damage));
}

void UFloatingDamageWidget::SetColor(const FColor Color)
{
	DamageText->SetColorAndOpacity(Color);
}
