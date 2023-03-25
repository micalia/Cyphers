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

	UFUNCTION()
		void AnimNotify_FootSound();
//������
	void PlayDieAnim();
//��������
	UPROPERTY(EditAnywhere)
	float jaeScale = 0.9;
	UFUNCTION()
		void AnimNotify_JumpAttackStart();

	UFUNCTION()
		void AnimNotify_JumpAttackImpact();

	UFUNCTION(BlueprintCallable, Category = FSMEvent)
		void PlayJumpAttackAnim(FName sectionName);

//�������� ����
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
// �׶��� ����
	UFUNCTION()
		void AnimNotify_GroundAttackEffect();
	void PlayGroundAttackAnim();
// �ٰŸ� ���� - ���������
	void PlayCloseKnockDownAttack();
//��Ÿ��
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
