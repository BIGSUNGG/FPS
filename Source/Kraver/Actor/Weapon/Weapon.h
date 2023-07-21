// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UWeaponComponent;

UCLASS()
class KRAVER_API AWeapon : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual int32 MakeAdditivePrimitiveInfo() final; // 추가적인 PrimitiveInfo를 추가 (Return 값은 추가된 WeaponMesh의 인덱스값)
	virtual int32 RemoveAdditivePrimitiveInfo(const TMap<FString, UPrimitiveComponent*>& Info) final; // 추가적인 PrimitiveInfo를 제거 (Return 값은 제거된 WeaponMesh의 인덱스값)
	virtual int32 FindAdditivePrimitiveInfo(const TMap<FString, UPrimitiveComponent*>& Info) final; // 추가적인 PrimitiveInfo를 찾음 (Return 값은 찾은 WeaponMesh의 인덱스값 못찾았을 경우 -1)

	virtual bool Equipped(ACreature* Character); // Character에게 장착됨 Server에서만 호출됨
	virtual bool UnEquipped(); // 장착해제됨
	virtual bool Hold(); // Character 손에 들려짐
	virtual bool Holster(); // Character 손에서 집어넣어짐

	virtual void AddOnOwnerDelegate();
	virtual void RemoveOnOwnerDelegate();

	virtual void AttackCancel();
	virtual void AddWeaponPrimitive(FString Key, UPrimitiveComponent* Value);

	// Delegate
	UFUNCTION()
		virtual void OnAttackStartEvent(); // 캐릭터의 공격이 시작하였을때 호출되는 함수
	UFUNCTION()
		virtual void OnAttackEndEvent(); // 캐릭터의 공격이 끝났을때 호출되는 함수

	UFUNCTION()
		virtual void OnSubAttackStartEvent(); // 캐릭터의 공격이 시작하였을때 호출되는 함수
	UFUNCTION()
		virtual void OnSubAttackEndEvent(); // 캐릭터의 공격이 끝났을때 호출되는 함수

	UFUNCTION()
		virtual void OnMakeNewPrimitiveInfoEvent(int Index);

protected:
	// Rpc
	UFUNCTION(Client, Reliable)
		void Client_Equipped();
	UFUNCTION(Client, Reliable)
		void Client_UnEquipped();
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_Equipped(ACreature* Character);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UnEquipped();
	UFUNCTION(Server, Reliable)
		void Server_TakeImpulseAtLocation(FVector Impulse, FVector ImpactPoint);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_TakeImpulseAtLocation(FVector Impulse, FVector ImpactPoint);

	// Delegate
	UFUNCTION()
		virtual void OnAttackEvent();

	virtual void Attack() final; // 공격할때 호출되는 함수

public:
	// Getter Setter
	bool GetCanInteracted();
	bool GetIsAttacking() { return IsAttacking; }
	bool GetIsSubAttacking() { return IsSubAttacking; }
	FName GetAttachSocketName() { return AttachSocketName; }
	ACreature* GetOwnerCreature() { return OwnerCreature; }
	EWeaponType GetWeaponType() { return WeaponType; }
	EWeaponState GetWeaponState() { return WeaponState; }
	UPrimitiveComponent* GetWeaponMesh() { return RootMesh; }
	TMap<FString, UPrimitiveComponent*>& GetWeaponPrimitiveInfo() { return WeaponPrimitiveInfo; }
	TArray<TMap<FString, UPrimitiveComponent*>>& GetAdditiveWeaponPrimitiveInfo() { return AdditiveWeaponPrimitiveInfo; }

	UAnimSequence* GetAnimIdleTpp() { return AnimIdleTpp; }
	UAnimSequence* GetAnimIdleFpp() { return AnimIdleFpp; }
	UAnimSequence* GetSubAttackTpp() { return SubAttackAnimTpp; }
	UAnimSequence* GetSubAttackFpp() { return SubAttackAnimFpp; }
	UBlendSpace* GetAnimMovementTpp() { return AnimMovementTpp; }
	UBlendSpace* GetAnimMovementFpp() { return AnimMovementFpp; }

	virtual UAnimMontage* GetAttackMontageTpp() { return AttackMontageTpp; }
	virtual UAnimMontage* GetAttackMontageFpp() { return AttackMontageFpp; }

	void SetOwnerCreature(ACreature* pointer);

public:
	FAttackDele OnAttack;
	FAttackDele OnBeforeAttack;

	FAttackStartDele OnAttackStart;
	FAttackStartDele OnSubAttackStart;
	FAttackEndDele OnAttackEnd;
	FAttackEndDele OnSubAttackEnd;

	FSkillDele OnSkillFirst; // R
	FSkillDele OnSkillSecond; // Q
	FSkillDele OnSkillThird; // E

	FPlayMontageDele OnPlayTppMontage;
	FPlayMontageDele OnPlayFppMontage;

	FAddOnDele OnAddOnDelegate;
	FRemoveOnDele OnRemoveOnDelegate;

	FMakeNewPrimitiveInfoDele OnMakeNewPrimitiveInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Component", Meta = (AllowPrivateAccess = true))
		TArray<UWeaponComponent*> WeaponComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Crosshairs")
		class UTexture2D* CrosshairsCenter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Crosshairs")
		UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Crosshairs")
		UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Crosshairs")
		UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Crosshairs")
		UTexture2D* CrosshairsBottom;

protected:
	UPROPERTY(Replicated)
		class ACreature* OwnerCreature = nullptr;
	UFUNCTION(Server, Reliable)
		void Server_SetOwnerCreature(ACreature* pointer);

	UPROPERTY(Replicated)
	EWeaponState WeaponState = EWeaponState::NONE; // 무기 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|State", meta = (AllowPrivateAccess = "true"))
		EWeaponType WeaponType = EWeaponType::NONE; // 무기 종류

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component", Meta = (AllowPrivateAccess = true))
		USceneComponent* RootSceneComponent;

	// Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
		UAnimMontage* AttackMontageTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
		UAnimMontage* AttackMontageFpp;
	
	// Anim
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
		UAnimSequence* AnimIdleTpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
		UAnimSequence* AnimIdleFpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
		UAnimSequence* SubAttackAnimTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
		UAnimSequence* SubAttackAnimFpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
		UBlendSpace* AnimMovementTpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
		UBlendSpace* AnimMovementFpp;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Mesh", meta = (AllowPrivateAccess = "true"))
		FName AttachSocketName = "SOCKET_Weapon_AR_01"; // Weapon을 Attach할 스켈레탈 본 이름

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|Component|Info", meta = (AllowPrivateAccess = "true"))
		UPrimitiveComponent* RootMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Info", meta = (AllowPrivateAccess = "true"))
		TMap<FString, UPrimitiveComponent*> WeaponPrimitiveInfo;
	TArray<TMap<FString, UPrimitiveComponent*>> AdditiveWeaponPrimitiveInfo; // 추가적인 WeaponMesh를 가지는 배열

	UPROPERTY(Replicated)
		bool IsAttacking = false; // 공격중인지
	void SetIsAttacking(bool Value);
	UFUNCTION(Server, Reliable)
		void Server_SetIsAttacking(bool Value);

	UPROPERTY(Replicated)
		bool IsSubAttacking = false; // 보조 공격중인지
	void SetIsSubAttacking(bool Value);
	UFUNCTION(Server, Reliable)
		void Server_SetIsSubAttacking(bool Value);

	bool bAttackCanceled = false;
	bool bCanAttack = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
		bool bAutomaticAttack = false; // 연사공격이 가능한지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
		bool bFirstAttackDelay = false; // 첫공격에 딜레이가 있는지
	bool bFirstInputAttack = false; // 공격 선입력이 입력되어있는지 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
		bool bCanFirstInputAttack = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
		float AttackDelay = 0.2f; // 공격 딜레이
	float CurAttackDelay = 0.f; // 현재 공격 딜레이

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
		float AttackDamage = 10.f; // 공격을 하였을때 주는 데미지	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
		float AttackImpulse = 150.f; // 공격을 하였을때 주는 충격량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UKraverDamageType> AttackDamageType;

	FTimerHandle AutomaticAttackHandle; // 연사공격을 담당하는 TimerHandle

};
