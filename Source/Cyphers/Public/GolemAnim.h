// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GolemFSM.h"
#include "GolemAnim.generated.h"
/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FOnKnockDownAttackCheckDelegate);

UCLASS()
class CYPHERS_API UGolemAnim : public UAnimInstance
{
	GENERATED_BODY()
public:
	UGolemAnim();

	UPROPERTY()
		class AGolem* enemy;
	UPROPERTY(VisibleAnywhere)
		class ACypher_Kaya* target;

	UPROPERTY(EditAnywhere, Category=Effect)
	float groundAttackEffectSize = 3.4;

	virtual void NativeBeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
		EGolemState animState;

	FOnKnockDownAttackCheckDelegate OnKnockDownAttackCheck;

	UPROPERTY(VisibleAnywhere, Category = CheckBool)
		bool bAttackPlay = false;

	UFUNCTION(BlueprintCallable, Category = FSMEvent)
		void OnEndAttackAnimation();
	UFUNCTION()
		void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
		void AnimNotify_TurnToTarget();

		UFUNCTION()
			void AnimNotify_HideBossHP();
		UFUNCTION()
			void AnimNotify_BossRoarShake();
	UFUNCTION()
		void AnimNotify_FootSound();
	UFUNCTION()
		void AnimNotify_RoarSound();

		UFUNCTION()
		void AnimNotify_DieSound1();
		UFUNCTION()
		void AnimNotify_DieSound2();
//데미지
	void PlayDieAnim();
//점프공격
	UPROPERTY(EditAnywhere)
	float jaeScale = 0.9;
	UFUNCTION()
		void AnimNotify_JumpAttackStart();

	UFUNCTION()
		void AnimNotify_JumpAttackImpact();

	UFUNCTION(BlueprintCallable, Category = FSMEvent)
		void PlayJumpAttackAnim(FName sectionName);

//돌던지기 공격
	UPROPERTY()
		class AStoneObj* spawnStone;
	UPROPERTY(EditDefaultsOnly, Category = Sound)
		class USoundBase* pickStoneSound;
	UFUNCTION(BlueprintCallable, Category = FSMEvent)
		void PlayThrowStoneAttackAnim(FName sectionName);
	UFUNCTION()
		void AnimNotify_SpawnSound();
	UFUNCTION()
		void AnimNotify_StoneSpawn();
	UFUNCTION()
		void AnimNotify_ThrowStone();
// 그라운드 어택
	UFUNCTION()
		void AnimNotify_GroundAttackEffect();
	void PlayGroundAttackAnim();
// 근거리 공격 - 땅내려찍기
	UFUNCTION()
		void AnimNotify_KnockDownAttackCheck();
	void PlayCloseKnockDownAttack();
//몽타주
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		UAnimMontage* jumpAttackMontage;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		UAnimMontage* throwStoneAttackMontage;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		UAnimMontage* groundAttackMontage;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		UAnimMontage* closeKnockDownAttackMontage;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		UAnimMontage* damageMontage;
};
