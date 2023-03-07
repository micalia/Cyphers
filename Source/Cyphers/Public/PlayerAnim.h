// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

UENUM(BlueprintType)
enum class EPlayerState : uint8 {
	Move,
	BasicAttack,
};
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
		bool bAir = false;

public:
	UPROPERTY(EditAnywhere)
		class UAnimMontage* basicAttackAnimMontage;

	void BasicAttackPlayAnim();

	UFUNCTION()
		void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
