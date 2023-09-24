// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include KraverHud_h
#include "ControlHud.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AControlHud : public AKraverHud
{
	GENERATED_BODY()
	
public:
	AControlHud();
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Widget", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> ControlWidgetClass; // CombatWidget의 클래스 레퍼런스를 가지는 변수
	UUserWidget* ControlWidget; // PlayerCharacter의 상태를 알려주는 위젯

};
