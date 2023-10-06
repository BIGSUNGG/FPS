// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponSelectWidget.h"
#include DataSubsystem_h

TTuple<FString, float> UWeaponSelectWidget::MainWeaponTextInfo = TTuple<FString, float>("AR", 36.f);
TTuple<FString, float> UWeaponSelectWidget::SubWeaponTextInfo = TTuple<FString, float>("Silencer Pistol", 32.f);
TTuple<FString, float> UWeaponSelectWidget::SpecialWeaponTextInfo = TTuple<FString, float>("Grenade Launcher", 26.f);

void UWeaponSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MainWeaponText->SetText(FText::FromString(MainWeaponTextInfo.Key));
	MainWeaponText->Font.Size = MainWeaponTextInfo.Value;

	SubWeaponText->SetText(FText::FromString(SubWeaponTextInfo.Key));
	SubWeaponText->Font.Size = SubWeaponTextInfo.Value;

	SpecialWeaponText->SetText(FText::FromString(SpecialWeaponTextInfo.Key));
	SpecialWeaponText->Font.Size = SpecialWeaponTextInfo.Value;

}

void UWeaponSelectWidget::SelectWeapon(TSubclassOf<AWeapon> WeaponClass, FString WeaponName, float Size, int Index)
{
	switch (Index)
	{
	case 0:
		MainWeaponTextInfo.Key = WeaponName;
		MainWeaponTextInfo.Value = Size;

		MainWeaponText->SetText(FText::FromString(WeaponName));
		MainWeaponText->Font.Size = Size;
		break;
	case 1:
		SubWeaponTextInfo.Key = WeaponName;
		SubWeaponTextInfo.Value = Size;

		SubWeaponText->SetText(FText::FromString(WeaponName));
		SubWeaponText->Font.Size = Size;
		break;
	case 2:
		SpecialWeaponTextInfo.Key = WeaponName;
		SpecialWeaponTextInfo.Value = Size;

		SpecialWeaponText->SetText(FText::FromString(WeaponName));
		SpecialWeaponText->Font.Size = Size;
		break;
	}

	GetGameInstance()->GetSubsystem<UDataSubsystem>()->SetWeaponSelect(WeaponClass, WeaponName, Index);

}
