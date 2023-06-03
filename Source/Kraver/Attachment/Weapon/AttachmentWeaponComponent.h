// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Attachment/AttachmentComponent.h"
#include "AttachmentWeaponComponent.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UAttachmentWeaponComponent : public UAttachmentComponent
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|First person", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* ScopeMesh;
};
