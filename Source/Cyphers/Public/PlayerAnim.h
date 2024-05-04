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
DECLARE_MULTICAST_DELEGATE(FOnNextDashCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnGripAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnGripAttackCheck2Delegate);

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

	void AttachCamera();

	void DamagePlayAnim();
	void DiePlayAnim();

	//골렘한테 대쉬공격 받았을때
	void KnockBackFlyAnim();
	void RiseAnim();
private:
	//몽타주
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* basicAttackAnimMontage;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = DashAttack, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* dashAttackAnimMontage;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = GripAttack, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* gripAttackAnimMontage;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = PowerAttack, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* powerAttackAnimMontage;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Dash, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* dashMontage;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Damage, Meta = (AllowPrivateAccess = true))
	class UAnimMontage* damageMontage;
public:
	//걷기 사운드
	UFUNCTION()
	void AnimNotify_LeftFoot();
	UFUNCTION()
	void AnimNotify_RightFoot();
	//평타 공격 콤보(좌클릭)

	void BasicAttackMontageSection(int32 NewSection);

	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;
	FOnDashAttackHitCheckDelegate OnDashAttackHitCheck;
	FOnNextDashCheckDelegate OnNextDashCheck;
	FOnGripAttackCheckDelegate OnGripAttackCheck;
	FOnGripAttackCheck2Delegate OnGripAttackCheck2;

	UFUNCTION()
	void AnimNotify_AttackHitCheck();
	UFUNCTION()
	void AnimNotify_NextAttackCheck();
	UFUNCTION()
	void AnimNotify_GripAttackCheck();
	UFUNCTION()
	void AnimNotify_GripAttackCheck2();
	UFUNCTION()
	void AnimNotify_PowerAttackCombo1();
	UFUNCTION()
	void AnimNotify_PowerAttackCombo2();
	UFUNCTION()
	void AnimNotify_PowerAttackCombo7();
	UFUNCTION()
	void AnimNotify_PowerAttackEnd();
	UFUNCTION()
	void AnimNotify_PAEndEffect();
	UFUNCTION()
	void AnimNotify_DashAttackCheck();
	FName GetAttackMontageSectionName(int32 Section);

	//파워 어택 END 이펙트 크기
	UPROPERTY(EditAnywhere)
	float paeScale = 0.6;
	void BasicAttackPlayAnim();
	//대쉬 공격 (양클릭)
	void DashAttackPlayAnim();
	void GripAttackPlayAnim();
	void PowerAttackReadyAnim();
	void PowerAttackPlayAnim();
	//대쉬
	void PlayDashAnim();
	UFUNCTION()
	void AnimNotify_NextDashCheck();
	////사운드 재생
	UFUNCTION()
	void AnimNotify_BasicAttack1Sound();
	UFUNCTION()
	void AnimNotify_BasicAttack2Sound();
	UFUNCTION()
	void AnimNotify_BasicAttack3Sound();

	// 잔상 이펙트
	UFUNCTION()
	void AnimNotify_AfterImage();

	//Input F공격
	UFUNCTION()
	void AnimNotify_GaUpAtkSound();
	UFUNCTION()
	void AnimNotify_GaDownAtkSound();
};
