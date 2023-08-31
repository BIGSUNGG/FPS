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
		UTextBlock* AttackerNameText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* VictimNameText;

	FString AttackerName;
	FString VictimName;
	float CurLifeTime = 0.f;

};
