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

//��Ÿ
// �̵�� �޺�����
	void AttackStartComboState();
	void AttackEndComboState();
	UFUNCTION()
		void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;
	/*UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo; => �̵�� ������ å���� �� ������ ��������� �ڵ忡 �帧�� �м��� ��� ���� �ʿ���� ������ �����Ϸ��� ������ Ȥ�� ���� ������ �̵�� �������� �ǵ��� ���߿� ������ ���� �����Ƿ� �ּ�ó�� ��*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo = 3;
public:
//SB ����
	UPROPERTY(BlueprintReadWrite)
	bool bAttackChk = true;
	UPROPERTY(BlueprintReadWrite)
	int32 basicAttackCombo = 1;
	UPROPERTY(BlueprintReadWrite)
	bool basicAttackComboContinueCheckSection = false;
	UPROPERTY(BlueprintReadWrite)
	bool basicAttackComboContinue = false;
public:
	void InputMouseLeft();	//��Ÿ
	/*void InputMouseRight();
	void InputMouseLeftAndRight();
	void InputKeyShiftAndMouseLeft();
	void InputKeySpace();
	void InputKeyE();*/
};
