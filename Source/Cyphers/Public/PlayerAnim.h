// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

/**
 *
 */
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
		bool bAir = false;

public:
	UPROPERTY(EditAnywhere)
		class UAnimMontage* basicAttackAnimMontage;

		int32 basicAttackCombo=0;
	void BasicAttackPlayAnim();
};
