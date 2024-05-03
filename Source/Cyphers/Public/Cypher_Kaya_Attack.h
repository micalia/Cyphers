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
// 마우스 클릭 인풋
	bool bLeftMouseButtonPressed;
	bool bRightMouseButtonPressed;
	void InitInput(); //마우스 클릭 인풋 초기화
	void InputMouseLeft();	//평타	
	void InputMouseLeftAndRight();	//대쉬 공격
	void InputMouseRight(); //구현안됨
	void InputKeyShiftAndMouseLeft();  //구현안됨
	UPROPERTY(EditAnywhere)
	float MouseLRCheckTime = 0.07;
	float MouseLRCheckCurrentTime = 0;
	
public:
//키보드 인풋
	void InputKeyF();
	void InputKeyE_Pressed();
	void InputKeyE_Released();
	void InputKeySpaceBar();
public:
//현재 상태
	bool bNotDamageMotion;
	//현재 스킬 사용중
	bool bDefaultUsingSkill = false;
	bool* bUsingSkill = &bDefaultUsingSkill;
public:
// 마우스 좌클릭 : 평타
	void BasicAttack();
	void AttackCheck();
	void AttackStartComboState();
	void AttackEndComboState();
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
	bool IsComboInputOn;
	bool bBasicAttackOn = false;
	// 평타 범위 감지 체크
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
//양클릭 : 대쉬 공격
	bool bDashAttackOn = false;
	bool startCoolBothMouse;
	UPROPERTY(EditAnywhere)
	float bothMouseAttackCool = 3;
	float currbothMouseAttackCool;
	// 마우스를 클릭하고 양클릭 감지를 위해 짧은 시간동안 텀을 두고, 다음 스킬 실행
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
//키보드E : 궁극기 스킬
UPROPERTY()
	class APowerAttackDecal* decal;
	bool startCoolKeyE;
	UPROPERTY(EditAnywhere)
	float keyECool = 5;
	float currkeyECool;
	//궁극기 사용이후 카메라 원래 위치로 Lerp 이동
	bool bBackCameraOringinPos;
	float currCameraBackTime = 0;
	UPROPERTY(EditAnywhere)
	float CameraBackTime = 0.2;
	//궁극기 충전(차징)
	bool bAttackCharge;
	FTimerHandle TimerHandle_PowerAttackStart;
	void StartPowerAttack();
	UPROPERTY(EditAnywhere)
	float PowerAttackStartTime = 1;
	float currPowerAttackCheck;
	bool powerAttackStartCheck;
	bool bPowerAttackOn = false;
public:
	//대쉬 : 스페이스바 스킬
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
	//키보드F : 잡기 스킬
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
