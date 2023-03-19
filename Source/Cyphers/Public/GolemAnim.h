// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GolemFSM.h"
#include "GolemAnim.generated.h"
/**
 * 
 */
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

	virtual void NativeBeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
		EGolemState animState;

	UPROPERTY(VisibleAnywhere, Category = CheckBool)
		bool bAttackPlay = false;

	UFUNCTION(BlueprintCallable, Category = FSMEvent)
		void OnEndAttackAnimation();
	UFUNCTION()
		void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
		void AnimNotify_TurnToTarget();

//점프공격

	UFUNCTION()
		void AnimNotify_JumpAttackStart();

	UFUNCTION()
		void AnimNotify_JumpAttackEnd();


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
//몽타주
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		UAnimMontage* jumpAttackMontage;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		UAnimMontage* throwStoneAttackMontage;
};
