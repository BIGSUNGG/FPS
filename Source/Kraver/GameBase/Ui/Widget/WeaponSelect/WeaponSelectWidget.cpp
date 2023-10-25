// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponSelectWidget.h"
#include KraverPlayerState_h

void UWeaponSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UWeaponSelectWidget::SelectWeapon(TSubclassOf<AWeapon> WeaponClass, FString WeaponName, float Size, int Index)
{
	AKraverPlayerState* PlayerState = Cast<AKraverPlayerState>(UGameplayStatics::GetPlayerState(this, 0));
	if (!PlayerState) return;

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

	PlayerState->SetDefaultWeapons(WeaponClass, Index);
}
