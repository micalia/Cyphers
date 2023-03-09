// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Cyphers/Cyphers.h"
#include "PlayerBaseComponent.h"
#include "Cypher_Kaya_Attack.generated.h"

/**
 * 
 */
UCLASS()
class CYPHERS_API UCypher_Kaya_Attack : public UPlayerBaseComponent
{
	GENERATED_BODY()
public:
	UCypher_Kaya_Attack();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SetupInputBinding(class UInputComponent* PlayerInputComponent) override;
public:
	UPROPERTY()
	class UPlayerAnim* kayaAnim;

//평타
// 이득우 콤보구현
	void AttackStartComboState();
	void AttackEndComboState();
	UFUNCTION()
		void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;
	/*UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo; => 이득우 선생님 책에는 이 변수를 사용하지만 코드에 흐름을 분석한 결과 굳이 필요없는 변수라 삭제하려고 했으나 혹시 내가 보르는 이득우 선생님의 의도를 나중에 깨달을 수도 있으므로 주석처리 함*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo = 3;
public:
//SB 구현
	UPROPERTY(BlueprintReadWrite)
	bool bAttackChk = true;
	UPROPERTY(BlueprintReadWrite)
	int32 basicAttackCombo = 1;
	UPROPERTY(BlueprintReadWrite)
	bool basicAttackComboContinueCheckSection = false;
	UPROPERTY(BlueprintReadWrite)
	bool basicAttackComboContinue = false;
public:
	void InputMouseLeft();	//평타
	/*void InputMouseRight();
	void InputMouseLeftAndRight();
	void InputKeyShiftAndMouseLeft();
	void InputKeySpace();
	void InputKeyE();*/
};
