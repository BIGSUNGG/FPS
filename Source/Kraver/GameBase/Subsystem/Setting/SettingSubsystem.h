// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SettingSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API USettingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	void ApplySettings();

public:
	// Getter Setter
	FORCEINLINE const FSettingInfo& GetSettingInfo() { return SettingInfo; }

	FORCEINLINE void SetSettingInfo(const FSettingInfo& InInfo) { SettingInfo = InInfo; }

protected:
	FSettingInfo SettingInfo;

};
