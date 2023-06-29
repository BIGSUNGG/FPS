// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#pragma warning(disable:4263)
#pragma warning(disable:4264)

#include "Kraver/Kraver.h"
#include "Blueprint/UserWidget.h"
#include "DamageDirectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UDamageDirectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		bool Initialize(AActor* InActor, FVector InLocation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void StartWidget();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void EndWidget();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		AActor* Actor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		FVector HitLocation;
};
