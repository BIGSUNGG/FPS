// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponSelectWidget.h"
#include KraverPlayerController_h

void UWeaponSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

}

void UWeaponSelectWidget::SelectWeapon(TSubclassOf<AWeapon> WeaponClass, FString WeaponName, float Size, int Index)
{
	AKraverPlayerController* PC = Cast<AKraverPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PC) return;

	switch (Index)
	{
	case 0:
		MainWeaponText->SetText(FText::FromString(WeaponName));
		MainWeaponText->Font.Size = Size;
		break;
	case 1:
		SubWeaponText->SetText(FText::FromString(WeaponName));
		SubWeaponText->Font.Size = Size;
		break;
	case 2:
		SpecialWeaponText->SetText(FText::FromString(WeaponName));
		SpecialWeaponText->Font.Size = Size;
		break;
	}

	PC->SetDefaultWeapons(WeaponClass, Index);
}
