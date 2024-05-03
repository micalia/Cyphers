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
// ���콺 Ŭ�� ��ǲ
	bool bLeftMouseButtonPressed;
	bool bRightMouseButtonPressed;
	void InitInput(); //���콺 Ŭ�� ��ǲ �ʱ�ȭ
	void InputMouseLeft();	//��Ÿ	
	void InputMouseLeftAndRight();	//�뽬 ����
	void InputMouseRight(); //�����ȵ�
	void InputKeyShiftAndMouseLeft();  //�����ȵ�
	UPROPERTY(EditAnywhere)
	float MouseLRCheckTime = 0.07;
	float MouseLRCheckCurrentTime = 0;
	
public:
//Ű���� ��ǲ
	void InputKeyF();
	void InputKeyE_Pressed();
	void InputKeyE_Released();
	void InputKeySpaceBar();
public:
//���� ����
	bool bNotDamageMotion;
	//���� ��ų �����
	bool bDefaultUsingSkill = false;
	bool* bUsingSkill = &bDefaultUsingSkill;
public:
// ���콺 ��Ŭ�� : ��Ÿ
	void BasicAttack();
	void AttackCheck();
	void AttackStartComboState();
	void AttackEndComboState();
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
	bool IsComboInputOn;
	bool bBasicAttackOn = false;
	// ��Ÿ ���� ���� üũ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BasicAttack)
	float BasicAttackStartPos = 300;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BasicAttack)
	float BasicAttackstartToEndDistance = 800;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BasicAttack)
	FVector BasicAttackRange = FVector(800, 600, 500);
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo = 3;
public:
//��Ŭ�� : �뽬 ����
	bool bDashAttackOn = false;
	bool startCoolBothMouse;
	UPROPERTY(EditAnywhere)
	float bothMouseAttackCool = 3;
	float currbothMouseAttackCool;
	// ���콺�� Ŭ���ϰ� ��Ŭ�� ������ ���� ª�� �ð����� ���� �ΰ�, ���� ��ų ����
	bool bMouseClickChk = false;
	void DashAttackCheck();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DashAttack)
	float DashAttackStartPos = 700;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DashAttack)
	float DashAttackstartToEndDistance = 1400;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DashAttack)
	FVector DashAttackRange = FVector(1400, 800, 500);
	void DashAttack();
public:
//Ű����E : �ñر� ��ų
UPROPERTY()
	class APowerAttackDecal* decal;
	bool startCoolKeyE;
	UPROPERTY(EditAnywhere)
	float keyECool = 5;
	float currkeyECool;
	//�ñر� ������� ī�޶� ���� ��ġ�� Lerp �̵�
	bool bBackCameraOringinPos;
	float currCameraBackTime = 0;
	UPROPERTY(EditAnywhere)
	float CameraBackTime = 0.2;
	//�ñر� ����(��¡)
	bool bAttackCharge;
	FTimerHandle TimerHandle_PowerAttackStart;
	void StartPowerAttack();
	UPROPERTY(EditAnywhere)
	float PowerAttackStartTime = 1;
	float currPowerAttackCheck;
	bool powerAttackStartCheck;
	bool bPowerAttackOn = false;
public:
	//�뽬 : �����̽��� ��ų
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
	bool bDash;
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Dash, Meta = (AllowPrivateAccess = true))
	int32 CurrentDashCombo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Dash, Meta = (AllowPrivateAccess = true))
	int32 MaxDashCombo = 2;
public:
	//Ű����F : ��� ��ų
	bool bGripAttack;
	bool startCoolKeyF;
	UPROPERTY(EditAnywhere)
	float keyFCool = 2;
	float currkeyFCool;

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
	float startToEndDistance = 720;
	UPROPERTY(EditAnywhere)
	FVector gripAttackRange = FVector(80, 330, 500);
	bool ga1Check;
	void GripAttackCheck();
	void GripAttackCheck2();
	UPROPERTY(EditAnywhere)
	float moveHitEnemyPos = 500;
};
