// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enemy_SentinelFSM.h"
#include "Enemy_SentinelAnim.generated.h"

/**
 * 
 */
UCLASS()
class CYPHERS_API UEnemy_SentinelAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeBeginPlay() override;

public:
	UPROPERTY()
		class AEnemy_Sentinel* enemy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EEnemy_SentinelState state;

	UFUNCTION()
		void AnimNotify_DieEnd();

	UFUNCTION()
		void AnimNotify_EnemyAttack();
	UFUNCTION()
	void AnimNotify_EnemyAttackEnd();
	UFUNCTION()
		void AnimNotify_CheckRiseDelayTime();
	UFUNCTION()
		void AnimNotify_ChangeToIdle();
	UFUNCTION()
		void AnimNotify_GA2Sound();
	UFUNCTION()
		void AnimNotify_NoHitState();
	UFUNCTION()
		void AnimNotify_HitState();

	void PlayGripAttackDamageAnim();
	void PlayGripAttackDamage2Anim();
};
