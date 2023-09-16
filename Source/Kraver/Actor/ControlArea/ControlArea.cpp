// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlArea.h"
#include CombatComponent_h

// Sets default values
AControlArea::AControlArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AreaRangeComponent = CreateDefaultSubobject<UBoxComponent>("AreaRangeComponent");
	AreaRangeComponent->SetCollisionProfileName(PROFILE_ControlArea);

}

// Called when the game starts or when spawned
void AControlArea::BeginPlay()
{
	Super::BeginPlay();
	
	if (IS_SERVER())
	{
		AreaRangeComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
		AreaRangeComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);
	}
}

// Called every frame
void AControlArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AControlArea::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AControlArea, RedTeamCount);
	DOREPLIFETIME(AControlArea, BlueTeamCount);
}

void AControlArea::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UCombatComponent* CombatComp = FindComponentByClassIncludeOwner<UCombatComponent>(OtherActor);
	if (CombatComp)
	{
		switch (CombatComp->GetCurTeamInfo().CurTeam)
		{
		case ETeam::RED:
			++RedTeamCount;
			KR_LOG(Log, TEXT("Red team in area"));
			break;
		case ETeam::BLUE:
			++BlueTeamCount;
			KR_LOG(Log, TEXT("Blue team in area"));
			break;
		default:
			KR_LOG(Error, TEXT("Default team in area"));
			break;
		}
	}
}

void AControlArea::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UCombatComponent* CombatComp = FindComponentByClassIncludeOwner<UCombatComponent>(OtherActor);
	if (CombatComp)
	{
		switch (CombatComp->GetCurTeamInfo().CurTeam)
		{
		case ETeam::RED:
			--RedTeamCount;
			KR_LOG(Log, TEXT("Red team out area"));
			break;
		case ETeam::BLUE:
			--BlueTeamCount;
			KR_LOG(Log, TEXT("Blue team out area"));
			break;
		default:
			KR_LOG(Error, TEXT("Default team out area"));
			break;
		}
	}
}

