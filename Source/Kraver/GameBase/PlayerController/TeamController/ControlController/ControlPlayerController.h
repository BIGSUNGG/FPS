// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include TeamPlayerController_h
#include "ControlPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AControlPlayerController : public ATeamPlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	class AControlGameState* ControlGameState = nullptr;

};
