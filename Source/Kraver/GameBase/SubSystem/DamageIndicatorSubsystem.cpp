// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageIndicatorSubsystem.h"
#include KraverPlayer_h
#include FloatingDamage_h
#include "Kraver/GameBase/Ui/Widget/DamageDirection/DamageDirectionWidget.h"

UDamageIndicatorSubsystem::UDamageIndicatorSubsystem()
{
	static ConstructorHelpers::FClassFinder<UDamageDirectionWidget> DamagedDirectionFinder(TEXT("/Game/ProjectFile/Widget/WBP_DamagedDirection.WBP_DamagedDirection_C"));
	if(DamagedDirectionFinder.Succeeded())
		DamagedDirectionClass = DamagedDirectionFinder.Class;

}

void UDamageIndicatorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

}

void UDamageIndicatorSubsystem::SetLocalPlayer(AKraverPlayer* Player)
{
	if(!IsValid(Player))
		return;

	if (!Player->IsLocallyControlled())
	{
		UE_LOG(LogTemp, Error, TEXT("UDamageIndicatorSubsystem : Player is not LocallyControlled"));
		return;
	}

	KraverPlayer = Player;
	Player->CombatComponent->OnClientGivePointDamageSuccess.AddDynamic(this, &UDamageIndicatorSubsystem::OnClientGiveDamagePointSuccessEvent);
	Player->CombatComponent->OnClientGiveRadialDamageSuccess.AddDynamic(this, &UDamageIndicatorSubsystem::OnClientGiveDamageRadialSuccessEvent);
	Player->CombatComponent->OnClientAfterTakePointDamageSuccess.AddDynamic(this, &UDamageIndicatorSubsystem::OnClientAfterTakePointDamageEvent);
	Player->CombatComponent->OnClientAfterTakeRadialDamageSuccess.AddDynamic(this, &UDamageIndicatorSubsystem::OnClientAfterTakeRadialDamageEvent);
}

void UDamageIndicatorSubsystem::OnClientGiveDamagePointSuccessEvent(AActor* DamagedActor, float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	OnClientGiveDamageSuccessEvent(DamagedActor, DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UDamageIndicatorSubsystem::OnClientGiveDamageRadialSuccessEvent(AActor* DamagedActor, float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	OnClientGiveDamageSuccessEvent(DamagedActor, DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void UDamageIndicatorSubsystem::OnClientAfterTakePointDamageEvent(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	if (DamageResult.bAlreadyDead)
		return;

	ACreature* Creature;
	AActor* FindActor = DamageCauser;
	while (true)
	{
		Creature = Cast<ACreature>(FindActor);
		if(Creature)
			break;

		if(!FindActor->GetOwner())
			return;
		
		FindActor = FindActor->GetOwner();
	}

	if (DamagedDirectionWidgets.Contains(Creature))
	{
		UDamageDirectionWidget* ExistWidget = DamagedDirectionWidgets[Creature];
		if(ExistWidget)
			ExistWidget->EndWidget();
	}

	UDamageDirectionWidget* Widget = Cast<UDamageDirectionWidget>(CreateWidget<UDamageDirectionWidget>(GetWorld(), DamagedDirectionClass));
	Widget->Initialize(KraverPlayer, DamageEvent.HitInfo.TraceStart);
	Widget->AddToViewport();
	DamagedDirectionWidgets.Add(Creature, Widget);
}	

void UDamageIndicatorSubsystem::OnClientAfterTakeRadialDamageEvent(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	if (DamageResult.bAlreadyDead)
		return;

	ACreature* Creature;
	AActor* FindActor = DamageCauser;
	while (true)
	{
		Creature = Cast<ACreature>(FindActor);
		if (Creature)
			break;

		if (!FindActor->GetOwner())
			return;

		FindActor = FindActor->GetOwner();
	}

	if (DamagedDirectionWidgets.Contains(Creature))
	{
		UDamageDirectionWidget* ExistWidget = DamagedDirectionWidgets[Creature];
		if (ExistWidget)
			ExistWidget->EndWidget();
	}

	UDamageDirectionWidget* Widget = Cast<UDamageDirectionWidget>(CreateWidget<UDamageDirectionWidget>(GetWorld(), DamagedDirectionClass));
	Widget->Initialize(KraverPlayer, DamageEvent.Origin);
	Widget->AddToViewport();
	DamagedDirectionWidgets.Add(Creature, Widget);
}

void UDamageIndicatorSubsystem::OnDestroyEvent(AActor* Actor)
{
	AFloatingDamage* FloatingDamage = Cast<AFloatingDamage>(Actor);
	if(!FloatingDamage)
		return;

	if(!DuplicationFloatingDamage)
		FloatingWidgets.Remove(FloatingDamage->GetInchargeActor());
}

void UDamageIndicatorSubsystem::OnClientGiveDamageSuccessEvent(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	if (DamageResult.ActualDamage == 0)
		return;

	UKraverDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UKraverDamageType>();

	if (DamageResult.bAlreadyDead || DamageType->AttackType == EKraverDamageType::ASSASSINATION)
		return;

	ACreature* Creature = Cast<ACreature>(DamagedActor);
	if (!Creature)
		return;

	AFloatingDamage* DamageWidget;

	if (!DuplicationFloatingDamage && FloatingWidgets.Contains(Creature))
		DamageWidget = FloatingWidgets[Creature];
	else
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = DamageCauser;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		DamageWidget = GetWorld()->SpawnActor<AFloatingDamage>(SpawnParams);
		DamageWidget->OnDestroy.AddDynamic(this, &UDamageIndicatorSubsystem::OnDestroyEvent);

		if (!DuplicationFloatingDamage)
			FloatingWidgets.Add(Creature, DamageWidget);
	}

	DamageWidget->ShowFloatingDamage(DamagedActor, DamageEvent, DamageResult);
}
