// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GolemFSM.generated.h"

UENUM(BlueprintType)
enum class EGolemState :uint8
{
	AppearBoss,
	Idle,
	Move,
	Damage,
	Die,
	JumpAttack,
	ThrowStoneAttack,
	GroundAttack,
	CloseKnockDownAttack,
	DashAttack,
};
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CYPHERS_API UGolemFSM : public UActorComponent
{
	GENERATED_BODY()

public:
	UGolemFSM();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//보스 등장
	bool bBossStart = false;
	//카메라 연출
	//UFUNCTION(BlueprintCallable)
	//	void ResetPlayerLocation(ACypher_kaya* player);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class ACypher_Kaya* target;
	UPROPERTY()
		class AGolem* me;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FSM)
		class UGolemAnim* anim;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FSM)
		EGolemState mState = EGolemState::Idle;
	bool bDie = false;

	//현재 상태
public:
	void IdleState();
	void MoveState(float DeltaTime);
	void JumpAttackState();
	void DamageState();
	void DieState();

	void CalculateCurrentTime(float DeltaTime);
	//데미지 처리
	//UFUNCTION(BlueprintCallable)
	//	void OnDamageProcess(float damage);

	void CheckAttackRangeAndCoolTime();
public:
	//custom
	UPROPERTY(EditAnywhere, Category = BossStat)
		float bossSpeed = 300;

	//플레이어와 붙어있을 경우 Move가 아닌 Idle상태로 변경
	UPROPERTY(EditAnywhere)
		float closestToTargetRange = 800;
	//fix
	FVector targetDistance;
	float targetDistanceLength;

public:
	//타겟 있는 곳으로 회전
	bool LookAtPlayerAfterAttack();
	FVector GoalPosition;
	float TurnAngle;
	FVector GoalDirection;
	// Called every frame
	float PrevTurnAngle;
	float NextTurnAngle;
	float TurnSpeedForDeltaTime = 550;
	float CurrentRotationTime;
	bool bTurnComplete = false;
	void SetNewGoalDirection();

public:
	UPROPERTY(EditAnywhere, Category = JumpAttack)
		float JA_startAtkPos = 350;
	UPROPERTY(EditAnywhere, Category = JumpAttack)
		float JA_startToEndDistance = 600;

	UPROPERTY(EditAnywhere, Category = JumpAttack)
		FVector JumpAttackRange = FVector(1100, 1650, 400);
	//custom
	UPROPERTY(EditAnywhere, Category = JumpAttack)
		float jumpAttackRangeStart = 3900;
	//float jumpAttackRangeStart = 1000; //world 스케일이 1일 경우
	UPROPERTY(EditAnywhere, Category = JumpAttack)
		float jumpAttackRangeEnd = 12600;
	//float jumpAttackRangeEnd = 3200;
	UPROPERTY(EditAnywhere, Category = JumpAttack)
		float jumpAttackDamage = 40;
	UPROPERTY(EditAnywhere, Category = JumpAttack)
		float JM_Point_BetweentMoveTime = 0.129f;
	UPROPERTY(EditAnywhere, Category = JumpAttack)
		float jumpMovingTime = 0.015f;
	UPROPERTY(EditAnywhere, Category = JumpAttack)
		float jumpAttackCoolTime = 3.8;
	//fix
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = JumpAttack)
		float jumpAttackCurrentTime = 0;
	float jumpAttackDeltaTime = 0;
	int32 jumpAttackIdx = 0;
	bool jumpAttackOn = false;
public:
	//custom
	UPROPERTY(EditAnywhere, Category = ThrowAttack)
		float throwStoneAttackRangeStart = 4000;
	UPROPERTY(EditAnywhere, Category = ThrowAttack)
		float throwStoneAttackRangeEnd = 13000;
	UPROPERTY(EditAnywhere, Category = ThrowAttack)
		float throwStoneAttackDamage = 40;
	UPROPERTY(EditAnywhere, Category = ThrowAttack)
		float throwStoneAttackCoolTime = 4.2;
	UPROPERTY(EditAnywhere, Category = ThrowAttack)
		float throwPower = 600000.0f;
	//fix
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = ThrowAttack)
		float throwStoneAttackCurrentTime = 0;
public:
	//custom
	UPROPERTY(EditAnywhere, Category = GroundAttack)
		float groundAttackRangeStart = 1500;
	UPROPERTY(EditAnywhere, Category = GroundAttack)
		float groundAttackRangeEnd = 3000;
	UPROPERTY(EditAnywhere, Category = GroundAttack)
		float groundAttackCoolTime = 3;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = GroundAttack)
		float groundAttackCurrentTime = 0;
public:
//근거리 땅 내려찍기
	UPROPERTY(EditAnywhere, Category = CloseKnockDownAttack)
		float KD_startAtkPos = 350;
	UPROPERTY(EditAnywhere, Category = CloseKnockDownAttack)
		float KD_startToEndDistance = 600;

	UPROPERTY(EditAnywhere, Category = CloseKnockDownAttack)
		FVector KnockDownAttackRange = FVector(430,1000,800);
	UPROPERTY(EditAnywhere, Category = CloseKnockDownAttack)
		float closeKnockDownAttackRangeStart = 0;
	UPROPERTY(EditAnywhere, Category = CloseKnockDownAttack)
		float closeKnockDownAttackRangeEnd = 1200;
	UPROPERTY(EditAnywhere, Category = CloseKnockDownAttack)
		float closeKnockDownAttackCoolTime = 3;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = CloseKnockDownAttack)
		float closeKnockDownAttackCurrentTime = 0;
	void KnockDownAttackCheck();

public:
//대쉬어택
	UPROPERTY(EditAnywhere, Category = DashAttack)
		float DashAttackCoolTime = 4;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = DashAttack)
		float DashAttackCurrentTime = 0;
	UPROPERTY(EditAnywhere, Category = DashAttack)
		float DashAttackRangeStart = 1500;
	UPROPERTY(EditAnywhere, Category = DashAttack)
		float DashAttackRangeEnd = 4300;
};
