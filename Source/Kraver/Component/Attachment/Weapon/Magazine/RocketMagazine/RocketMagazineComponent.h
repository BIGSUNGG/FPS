// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Component/Attachment/Weapon/Magazine/AttachmentMagazineComponent.h"
#include "RocketMagazineComponent.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API URocketMagazineComponent : public UAttachmentMagazineComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	class AGun* OwnerGun;
};
