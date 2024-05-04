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

	// ��, �ڸ� �Ǻ��ϴ� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float dirV = 0;

	// ������, ������ �Ǻ��ϴ� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float dirH = 0;

	//���߿� �ִ��� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAir = true;

	void AttachCamera();

	void DamagePlayAnim();
	void DiePlayAnim();

	//������ �뽬���� �޾�����
	void KnockBackFlyAnim();
	void RiseAnim();
private:
	//��Ÿ��
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
	//�ȱ� ����
	UFUNCTION()
	void AnimNotify_LeftFoot();
	UFUNCTION()
	void AnimNotify_RightFoot();
	//��Ÿ ���� �޺�(��Ŭ��)

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

	//�Ŀ� ���� END ����Ʈ ũ��
	UPROPERTY(EditAnywhere)
	float paeScale = 0.6;
	void BasicAttackPlayAnim();
	//�뽬 ���� (��Ŭ��)
	void DashAttackPlayAnim();
	void GripAttackPlayAnim();
	void PowerAttackReadyAnim();
	void PowerAttackPlayAnim();
	//�뽬
	void PlayDashAnim();
	UFUNCTION()
	void AnimNotify_NextDashCheck();
	////���� ���
	UFUNCTION()
	void AnimNotify_BasicAttack1Sound();
	UFUNCTION()
	void AnimNotify_BasicAttack2Sound();
	UFUNCTION()
	void AnimNotify_BasicAttack3Sound();

	// �ܻ� ����Ʈ
	UFUNCTION()
	void AnimNotify_AfterImage();

	//Input F����
	UFUNCTION()
	void AnimNotify_GaUpAtkSound();
	UFUNCTION()
	void AnimNotify_GaDownAtkSound();
};
