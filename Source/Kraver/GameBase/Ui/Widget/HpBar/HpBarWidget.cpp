// Fill out your copyright notice in the Description page of Project Settings.


#include "HpBarWidget.h"
#include Creature_h

void UHpBarWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (OwnerCreature)
	{
		// HP Bar ¼³Á¤
		float HpRatio = (float)OwnerCreature->CombatComponent->GetCurHp() / (float)OwnerCreature->CombatComponent->GetMaxHp();
		HpBar->SetPercent(HpRatio);

		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		ACreature* LocalCreature = Cast<ACreature>(PC->GetPawn());
		if (LocalCreature)
		{
			if (FTeamInfo::CheckIsTeam(OwnerCreature->CombatComponent->GetTeamInfo().CurTeam, LocalCreature->CombatComponent->GetTeamInfo().CurTeam))
				HpBar->SetFillColorAndOpacity(FLinearColor::Blue);
			else
				HpBar->SetFillColorAndOpacity(FLinearColor::Red);
		}
	}

}
