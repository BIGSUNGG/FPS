// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingDamage.h"
#include "Kraver/Widget/FloatingDamage/FloatingDamageWidget.h"
#include "Kraver/Creature/Creature.h"

// Sets default values
AFloatingDamage::AFloatingDamage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FloatingWidgetComp = CreateDefaultSubobject<UWidgetComponent>("FloatingWidgetComp");
	FloatingWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	FloatingWidgetComp->SetDrawSize(FVector2D(400.0f, 50.0f));
	FloatingWidgetComp->SetDrawAtDesiredSize(true);
	FloatingWidgetComp->SetPivot(FVector2D(0.5f, 0.5f));
	FloatingWidgetComp->SetVisibility(true);

	static ConstructorHelpers::FClassFinder<UFloatingDamageWidget> FloatingDamageFinder(TEXT("/Game/ProjectFile/Widget/WBP_FloatingDamageWidget.WBP_FloatingDamageWidget_C"));
	if (FloatingDamageFinder.Succeeded())
		FloatingWidgetComp->SetWidgetClass(FloatingDamageFinder.Class);

	SetRootComponent(FloatingWidgetComp);
}

// Called when the game starts or when spawned
void AFloatingDamage::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFloatingDamage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(GetActorLocation() + (UpVector * FloatingUpSpeed) + (RightVector * FloatingRightSpeed));
}

void AFloatingDamage::ShowFloatingDamage(AActor* DamagedActor, FKraverDamageEvent const& DamageEvent, FKraverDamageResult const& DamageResult)
{
	GetWorldTimerManager().ClearTimer(DestroyTimer);

	InchargeActor = Cast<ACreature>(DamagedActor);

	UFloatingDamageWidget* Widget = Cast<UFloatingDamageWidget>(FloatingWidgetComp->GetWidget());
	Widget->AddDamage(DamageResult.ActualDamage);
	Widget->SetColor(DamageResult.bCritical ? FColor::Red : FColor::White);


	FVector ForwardVector = DamageEvent.GetHitDirection();
	RightVector = -FVector::CrossProduct(ForwardVector, FVector(0.0f, 0.0f, 1.0f));
	UpVector = FVector::UpVector;

	SetActorLocation(DamageEvent.HitInfo.ImpactPoint + (RightVector * 15.f) + (UpVector * 15.f));
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AFloatingDamage::OnDestroyTimerEvent, LifeTime, false, LifeTime);
}

void AFloatingDamage::OnDestroyTimerEvent()
{
	Destroy(false);
	OnDestroy.Broadcast(this);
}

