// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Blueprint/UserWidget.h"
#include "GameFinishWidget.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UGameFinishWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void GameFinish(bool bWin);

protected:
	UFUNCTION(BlueprintNativeEvent)
	void VictoryEvent();
	UFUNCTION(BlueprintNativeEvent)
	void DefeatEvent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* GameFinishText;

};
