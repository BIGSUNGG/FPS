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

	virtual bool OnServer_Equipped(ACreature* Character); // Character에게 장착됨 Server에서만 호출됨
	virtual bool OnServer_UnEquipped(); // 장착해제됨
	virtual bool Unholster(); // Character 손에 들려짐
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
protected:
	// Rpc
	UFUNCTION(Server, Reliable)
	virtual void Server_OnAttackStartEvent();
	UFUNCTION(Server, Reliable)
	virtual void Server_OnAttackEndEvent(); 

	UFUNCTION(Server, Reliable)
	virtual void Server_OnSubAttackStartEvent(); 
	UFUNCTION(Server, Reliable)
	virtual void Server_OnSubAttackEndEvent();

	UFUNCTION(Server, Reliable)
	virtual void Server_OnAttackEvent();
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_OnAttackEvent();

	UFUNCTION(Server, Reliable)
	void Server_TakeImpulseAtLocation(FVector Impulse, FVector ImpactPoint);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TakeImpulseAtLocation(FVector Impulse, FVector ImpactPoint);

	// Delegate
	UFUNCTION()
	virtual void OnAttackEvent();

	virtual void Attack() final; // 공격할때 호출되는 함수

	// OnRep
	UFUNCTION()
	virtual void OnRep_WeaponState(EWeaponState PrevWeaponState);

	// Func
	virtual void MakeFppPrimitiveInfo() final; // 1인칭 PrimitiveInfo를 추가
	virtual void EquipEvent();
	virtual void UnEquipEvent();

public:
	// Getter Setter
	virtual bool CanAttack();
	virtual bool CanSubAttack();
	bool CanInteracted();
	bool GetIsAttacking() { return IsAttacking; }
	bool GetIsSubAttacking() { return IsSubAttacking; }
	bool GetbAttackWhileSprint() { return bAttackWhileSprint; }
	bool GetbSubAttackWhileSprint() { return bSubAttackWhileSprint; }

	FName GetFppHandSocketName() { return FppHandSocketName; }
	FName GetTppHandSocketName() { return TppHandSocketName; }
	ACreature* GetOwnerCreature() { return OwnerCreature; }
	EWeaponType GetWeaponType() { return WeaponType; }
	EWeaponState GetWeaponState() { return WeaponState; }
	USkeletalMeshComponent* GetTppWeaponMesh() { return TppWeaponMesh; }
	UPrimitiveComponent* GetFppWeaponMesh() { return FppWeaponMesh; }
	TMap<FString, UPrimitiveComponent*>& GetTppWeaponPrimitiveInfo() { return WeaponTppPrimitiveInfo; }
	TMap<FString, UPrimitiveComponent*>& GetFppWeaponPrimitiveInfo() { return WeaponFppPrimitiveInfo; }

	UAnimSequence* GetAnimIdleTpp() { return AnimIdleTpp; }
	UAnimSequence* GetAnimIdleFpp() { return AnimIdleFpp; }
	UAnimSequence* GetAnimSprintFpp() { return AnimSprintFpp; }
	UAnimSequence* GetAnimSprintTpp() { return AnimSprintTpp; }
	UAnimSequence* GetSubAttackTpp() { return SubAttackAnimTpp; }
	UAnimSequence* GetSubAttackFpp() { return SubAttackAnimFpp; }
	UBlendSpace* GetAnimMovementTpp() { return AnimMovementTpp; }
	UBlendSpace* GetAnimMovementFpp() { return AnimMovementFpp; }

	virtual UAnimMontage* GetAttackMontageTpp() { return AttackMontageTpp; }
	virtual UAnimMontage* GetAttackMontageFpp() { return AttackMontageFpp; }
	virtual UAnimMontage* GetUnholsterMontageTpp() { return UnholsterMontageTpp; }
	virtual UAnimMontage* GetUnholsterMontageFpp() { return UnholsterMontageFpp; }
	virtual UAnimMontage* GetHolsterMontageTpp() { return HolsterMontageTpp; }
	virtual UAnimMontage* GetHolsterMontageFpp() { return HolsterMontageFpp; }

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

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState)
	EWeaponState WeaponState = EWeaponState::NONE; // 무기 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|State", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType = EWeaponType::NONE; // 무기 종류

	// Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontageTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontageFpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* HolsterMontageTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* HolsterMontageFpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* UnholsterMontageTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* UnholsterMontageFpp;
	
	// Anim
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* AnimIdleTpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* AnimIdleFpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* AnimSprintTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* AnimSprintFpp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* SubAttackAnimTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* SubAttackAnimFpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
	UBlendSpace* AnimMovementTpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
	UBlendSpace* AnimMovementFpp;	

	// Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Sound", Meta = (AllowPrivateAccess = true))
	class USoundCue* AttackSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Mesh", meta = (AllowPrivateAccess = "true"))
	FName FppHandSocketName = "SOCKET_Weapon"; // Weapon을 Attach할 스켈레탈 본 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Mesh", meta = (AllowPrivateAccess = "true"))
	FName TppHandSocketName = "SOCKET_Weapon"; // Weapon을 Attach할 스켈레탈 본 이름

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* TppWeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component", meta = (AllowPrivateAccess = "true"))
	UPrimitiveComponent* FppWeaponMesh;

	TMap<FString, UPrimitiveComponent*> WeaponTppPrimitiveInfo;
	TMap<FString, UPrimitiveComponent*> WeaponFppPrimitiveInfo; // 추가적인 WeaponMesh를 가지는 배열

	UPROPERTY(Replicated)
	bool IsAttacking = false; // 공격중인지
	UPROPERTY(Replicated)
	bool IsSubAttacking = false; // 보조 공격중인지

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
	bool bAttackWhileSprint = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
	bool bSubAttackWhileSprint = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
	bool bCanSubAttack = true;

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
	TSubclassOf<UKraverDamageType> AttackDamageType;

	FTimerHandle AutomaticAttackHandle; // 연사공격을 담당하는 TimerHandle

};
