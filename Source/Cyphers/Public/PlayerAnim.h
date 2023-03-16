// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "Cyphers/Cyphers.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

UENUM(BlueprintType)
enum class EPlayerState : uint8 {
	Move,
	BasicAttack,
};

DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnDashAttackHitCheckDelegate);

UCLASS()
class CYPHERS_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPlayerAnim();

	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY()
	class ACypher_Kaya* me;

	EPlayerState playerState = EPlayerState::Move;

	FVector velocity;
	FVector forward;
	FVector right;

	// 앞, 뒤를 판별하는 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float dirV = 0;

	// 오른쪽, 왼쪽을 판별하는 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float dirH = 0;

	//공중에 있는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bAir = true;

private:
//공격 몽타주
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		class UAnimMontage* basicAttackAnimMontage;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = DashAttack, Meta = (AllowPrivateAccess = true))
		class UAnimMontage* dashAttackAnimMontage;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = GripAttack, Meta = (AllowPrivateAccess = true))
		class UAnimMontage* gripAttackAnimMontage;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = GripAttack, Meta = (AllowPrivateAccess = true))
		class UAnimMontage* powerAttackAnimMontage;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = GripAttack, Meta = (AllowPrivateAccess = true))
		class UAnimMontage* damageMontage;
public:
//평타 공격 콤보(좌클릭)

	void BasicAttackMontageSection(int32 NewSection);

	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;
	FOnDashAttackHitCheckDelegate OnDashAttackHitCheck;
	UFUNCTION()
		void AnimNotify_AttackHitCheck();
	UFUNCTION()
		void AnimNotify_NextAttackCheck();
	UFUNCTION()
		void AnimNotify_PowerAttackCombo1();
	UFUNCTION()
		void AnimNotify_PowerAttackCombo2();
	UFUNCTION()
		void AnimNotify_PowerAttackCombo3();
	UFUNCTION()
		void AnimNotify_PowerAttackCombo4();
	UFUNCTION()
		void AnimNotify_PowerAttackCombo5();
	UFUNCTION()
		void AnimNotify_PowerAttackCombo6();
	UFUNCTION()
		void AnimNotify_PowerAttackEnd();
	UFUNCTION()
		void AnimNotify_DashAttackCheck();
		FName GetAttackMontageSectionName(int32 Section);

	void BasicAttackPlayAnim();
//대쉬 공격 (양클릭)
	void DashAttackPlayAnim();
	void GripAttackPlayAnim();
	void PowerAttackReadyAnim();
	void PowerAttackPlayAnim();
	void DamagePlayAnim();
//파워어택
	/*UPROPERTY(EditAnywhere, Category = PowerAttack)
	float powerAttackRange= 450;
	UPROPERTY(EditAnywhere, Category = PowerAttack)
	float powerAttackRadius= 70;*/
	/*UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
		float DashAttackRange = 450;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
		float DashAttackRadius = 70;*/
};
