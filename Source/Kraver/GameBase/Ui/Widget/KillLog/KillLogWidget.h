// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#pragma warning(disable:4263)
#pragma warning(disable:4264)

#include "Kraver/Kraver.h"
#include "Blueprint/UserWidget.h"
#include "KillLogWidget.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UKillLogWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	bool Initialize(const FString& InAttackerName, const FString& InVictimName);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AttackerNameText; // 공격한 캐릭터 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* VictimNameText; // 사망한 캐릭터

	FString AttackerName; // 공격한 캐릭터 이름
	FString VictimName; // 사망한 캐릭터 이름
	float LifeTIme = 5.f; // 로그 유지 시간
	float CurLifeTime = 0.f; // 남은 유지 시간

};
