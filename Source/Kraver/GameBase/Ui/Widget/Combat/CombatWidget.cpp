// Fill out your copyright notice in the Description page of Project Settings.


#include "Kraver/GameBase/Ui/Widget/Combat/CombatWidget.h"
#include Creature_h
#include Gun_h

void UCombatWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	ACreature* Creature = Cast<ACreature>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	if (Creature == nullptr)
		return;

	// HP 텍스트 설정
	CurHpText->SetText(FText::AsNumber(Creature->CombatComponent->GetCurHp()));
	MaxHpText->SetText(FText::AsNumber(Creature->CombatComponent->GetMaxHp()));

	if (Creature->CombatComponent->GetCurWeapon())
	{
		switch (Creature->CombatComponent->GetCurWeapon()->GetWeaponType())
		{
			case EWeaponType::GUN:
			{
				// 탄약 텍스트 설정
				AGun* Weapon = Cast<AGun>(Creature->CombatComponent->GetCurWeapon());
				CurAmmoText->SetText(FText::AsNumber(Weapon->GetCurAmmo()));
				TotalAmmoText->SetText(FText::AsNumber(Weapon->GetTotalAmmo()));
			}
			break;
			default:
				CurAmmoText->SetText(FText::AsCultureInvariant(""));
				TotalAmmoText->SetText(FText::AsCultureInvariant(""));
				break;
		}
	}
	else
	{
		CurAmmoText->SetText(FText::AsCultureInvariant(""));
		TotalAmmoText->SetText(FText::AsCultureInvariant(""));
	}
}
