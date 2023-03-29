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
	class ACypher_Kaya* kaya;
	UPROPERTY()
	class UPlayerAnim* kayaAnim;
public:
	bool bLeftMouseButtonPressed;
	bool bRightMouseButtonPressed;
	
	bool bNotDamageMotion;
	void InitInput();
	void InputMouseLeft();	//��Ÿ	
	void InputMouseRight();	
	void InputMouseLeftAndRight();	//�뽬 ����
	void InputKeyShiftAndMouseLeft();
	void InputKeyF();
	void InputKeyE_Pressed();
	void InputKeyE_Released();
	void InputKeySpaceBar();

	/*void InputMouseLeftAndRight();
	void InputKeySpace();
	*/
	UPROPERTY(EditAnywhere)
	float MouseLRCheckTime = 0.07;
	float MouseLRCheckCurrentTime = 0;
	//float ClickTimestamp = 0.0;
	//�ñر� ������� ī�޶� ���� ��ġ�� Lerp �̵�
	bool bBackCameraOringinPos;
	float currCameraBackTime = 0;
	UPROPERTY(EditAnywhere)
	float CameraBackTime = 0.2;
	bool bAttackInput = false;
	bool bAttackCharge;

	void AttackCheck();
	void DashAttackCheck();

	UPROPERTY()
	class APowerAttackDecal* decal;
//��Ÿ��
	//��Ŭ��
	bool startCoolBothMouse;
	UPROPERTY(EditAnywhere)
	float bothMouseAttackCool = 3;
	float currbothMouseAttackCool;
	//Ű����E
	bool startCoolKeyE;
	UPROPERTY(EditAnywhere)
	float keyECool = 5;
	float currkeyECool;

	//Ű����F
	bool startCoolKeyF;
	UPROPERTY(EditAnywhere)
		float keyFCool = 2;
	float currkeyFCool;
//�뽬
	UPROPERTY(EditAnywhere)
	float spaceBarCool = 4;
	UPROPERTY(VisibleAnywhere)
	float currSpaceBarCool;
	bool startCoolSpaceBar;
	bool bDashOn;
	bool bDashComboOn;
	void DashStartComboState();
	void DashEndComboState();
	float dashHorizontal;
	bool bNextDirValCheck;
	void Dash();

public:
//������
	bool bIsGripAttacking;
	float GA_alpha;
	TArray<FVector> GAMovePoints;
	int32 gripIndex = 0;
	float gripMoveCurrTime;
	FVector GA_MoveNextPoint(FVector startPos, FVector endPos);

	UPROPERTY(EditAnywhere, Category = GripAttack)
	float grip1MoveTime = 0.36;
	UPROPERTY(EditAnywhere, Category = GripAttack)
	float grip2MoveTime = 0.41;
	UPROPERTY(EditAnywhere, Category = GripAttack)
	float grip3MoveTime = 0.39;

	UPROPERTY(EditAnywhere, Category = GripAttack)
	float grip1MoveDistance = 560;
	UPROPERTY(EditAnywhere, Category = GripAttack)
	float grip2MoveDistance = 350;
	UPROPERTY(EditAnywhere, Category = GripAttack)
	float grip3MoveDistance = 480;

	UPROPERTY(EditAnywhere)
	float startGripAtkPos = 70;
	UPROPERTY(EditAnywhere)
	float startToEndDistance = 670;
	UPROPERTY(EditAnywhere)
	FVector gripAttackRange = FVector(20, 250, 200);
	bool ga1Check;
	void GripAttackCheck();
	void GripAttackCheck2();
	UPROPERTY(EditAnywhere)
	float moveHitEnemyPos = 500;
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Dash, Meta = (AllowPrivateAccess = true))
		int32 CurrentDashCombo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Dash, Meta = (AllowPrivateAccess = true))
		int32 MaxDashCombo = 2;
public:
//�⺻����
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
	float AttackRange = 200;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
		float AttackRadius = 50;
//�뽬����
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
		float DashAttackRange = 450;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
		float DashAttackRadius = 70;

		bool CheckCurrState();
//�Ŀ�����
		FTimerHandle TimerHandle_PowerAttackStart;
		void StartPowerAttack();
		UPROPERTY(EditAnywhere)
		float PowerAttackStartTime=1;
		float currPowerAttackCheck;
		bool powerAttackStartCheck;
public:
//��Ÿ
// �̵�� �޺�����
	void BasicAttack();
	void AttackStartComboState();
	void AttackEndComboState();
	UFUNCTION()
		void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
	bool IsComboInputOn;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
	bool IsAttacking;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
		bool IsNoComboAttacking;
private:
	/*UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo; => �̵�� ������ å���� �� ������ ��������� �ڵ忡 �帧�� �м��� ��� ���� �ʿ���� ������ �����Ϸ��� ������ Ȥ�� ���� ������ �̵�� �������� �ǵ��� ���߿� ������ ���� �����Ƿ� �ּ�ó�� ��*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo = 3;
public:
//�뽬 ����
	void DashAttack();
};
