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
	void InputMouseLeft();	//평타	
	void InputMouseRight();	
	void InputMouseLeftAndRight();	//대쉬 공격
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
	//궁극기 사용이후 카메라 원래 위치로 Lerp 이동
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
//쿨타임
	//양클릭
	bool startCoolBothMouse;
	UPROPERTY(EditAnywhere)
	float bothMouseAttackCool = 3;
	float currbothMouseAttackCool;
	//키보드E
	bool startCoolKeyE;
	UPROPERTY(EditAnywhere)
	float keyECool = 5;
	float currkeyECool;

	//키보드F
	bool startCoolKeyF;
	UPROPERTY(EditAnywhere)
		float keyFCool = 2;
	float currkeyFCool;
//대쉬
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
//잡기공격
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
//기본공격
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
	float AttackRange = 200;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
		float AttackRadius = 50;
//대쉬공격
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
		float DashAttackRange = 450;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
		float DashAttackRadius = 70;

		bool CheckCurrState();
//파워어택
		FTimerHandle TimerHandle_PowerAttackStart;
		void StartPowerAttack();
		UPROPERTY(EditAnywhere)
		float PowerAttackStartTime=1;
		float currPowerAttackCheck;
		bool powerAttackStartCheck;
public:
//평타
// 이득우 콤보구현
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
	bool CanNextCombo; => 이득우 선생님 책에는 이 변수를 사용하지만 코드에 흐름을 분석한 결과 굳이 필요없는 변수라 삭제하려고 했으나 혹시 내가 보르는 이득우 선생님의 의도를 나중에 깨달을 수도 있으므로 주석처리 함*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo = 3;
public:
//대쉬 공격
	void DashAttack();
};
