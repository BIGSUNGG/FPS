// Fill out your copyright notice in the Description page of Project Settings.


#include "Kraver/Ui/Widget/Combat/CombatWidget.h"
#include "Kraver/Character/Creature/Creature.h"
#include "Kraver/Actor/Weapon/Gun/Gun.h"

void UCombatWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	ACreature* Creature = Cast<ACreature>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	if (Creature == nullptr)
		return;

	CurHpText->SetText(FText::AsNumber(Creature->CombatComponent->GetCurHp()));
	MaxHpText->SetText(FText::AsNumber(Creature->CombatComponent->GetMaxHp()));

	if (Creature->CombatComponent->GetCurWeapon())
	{
		switch (Creature->CombatComponent->GetCurWeapon()->GetWeaponType())
		{
			case EWeaponType::GUN:
			{
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
