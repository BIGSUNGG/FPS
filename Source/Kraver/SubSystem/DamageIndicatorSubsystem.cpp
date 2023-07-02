// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageIndicatorSubsystem.h"
#include "Kraver/Character/Creature/Soldier/KraverPlayer/KraverPlayer.h"
#include "Kraver/Actor/FloatingDamage/FloatingDamage.h"
#include "Kraver/Ui/Widget/DamageDirection/DamageDirectionWidget.h"

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

void UDamageIndicatorSubsystem::OnLocalPlayerBeginPlay(AKraverPlayer* Player)
{
	if (!Player->IsLocallyControlled())
	{
		UE_LOG(LogTemp, Error, TEXT("UDamageIndicatorSubsystem : Player is not LocallyControlled"));
		return;
	}

	KraverPlayer = Player;
	Player->CombatComponent->OnClientGiveDamageSuccess.AddDynamic(this, &UDamageIndicatorSubsystem::OnClientGiveDamageSuccessEvent);
	Player->CombatComponent->OnClientAfterTakeDamageSuccess.AddDynamic(this, &UDamageIndicatorSubsystem::OnClientAfterTakeDamageEvent);
}

void UDamageIndicatorSubsystem::OnClientGiveDamageSuccessEvent(AActor* DamagedActor, float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	if(DamageResult.bAlreadyDead || DamageEvent.DamageType == EKraverDamageType::ASSASSINATION)
		return;

	ACreature* Creature = Cast<ACreature>(DamagedActor);
	if(!Creature)
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

		if(!DuplicationFloatingDamage)
			FloatingWidgets.Add(Creature, DamageWidget);	
	}
	
	DamageWidget->ShowFloatingDamage(DamagedActor, DamageEvent, DamageResult);
}

void UDamageIndicatorSubsystem::OnClientAfterTakeDamageEvent(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
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

void UDamageIndicatorSubsystem::OnDestroyEvent(AActor* Actor)
{
	AFloatingDamage* FloatingDamage = Cast<AFloatingDamage>(Actor);
	if(!FloatingDamage)
		return;

	if(!DuplicationFloatingDamage)
		FloatingWidgets.Remove(FloatingDamage->GetInchargeActor());
}
