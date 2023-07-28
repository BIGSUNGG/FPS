// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Component/Attachment/Weapon/Magazine/AttachmentMagazineComponent.h"
#include "RocketMagazineComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API URocketMagazineComponent : public UAttachmentMagazineComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:
	void SetMagazineVisibility(bool Value);

	// Delegate
	virtual void OnAddOnDelegateEvent(UObject* Object) override;
	virtual void OnRemoveOnDelegateEvent(UObject* Object) override;

	UFUNCTION()
		virtual void OnGrabMagazineEvent();
	UFUNCTION()
		virtual void OnInsertMagazineEvent();
protected:
	class AGun* OwnerGun;
	bool IsGrabMagazine = false;
};
