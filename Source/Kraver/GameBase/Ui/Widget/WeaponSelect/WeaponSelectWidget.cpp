// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSelectWidget.h"
#include SaveSubsystem_h

void UWeaponSelectWidget::SelectWeapon(TSubclassOf<AWeapon> WeaponClass, FString WeaponName, float Size, int Index)
{
	switch (Index)
	{
	case 0:
		MainWeaponText->SetText(FText::FromString(WeaponName));
		break;
	case 1:
		SubWeaponText->SetText(FText::FromString(WeaponName));
		break;
	case 2:
		SpecialWeaponText->SetText(FText::FromString(WeaponName));
		break;
	}

	GetGameInstance()->GetSubsystem<USaveSubsystem>()->SetWeaponSelect(WeaponClass, WeaponName, Index);

}
