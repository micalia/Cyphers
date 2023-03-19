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
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CYPHERS_API UGolemFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGolemFSM();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
//���� ����
	bool bBossStart = false;
	//ī�޶� ����
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
	
//���� ����
public:
	void IdleState();
	void MoveState(float DeltaTime);
	void JumpAttackState();
	void DamageState();
	void DieState();

	//������ ó��
	//UFUNCTION(BlueprintCallable)
	//	void OnDamageProcess(float damage);

	void CheckAttackRangeAndCoolTime();
public:
	//custom
	UPROPERTY(EditAnywhere, Category = BossStat)
		float bossSpeed = 300;

//�÷��̾�� �پ����� ��� Move�� �ƴ� Idle���·� ����
	UPROPERTY(EditAnywhere)
		float closestToTargetRange = 200;
	//fix
	FVector targetDistance;
	float targetDistanceLength;

public:
//Ÿ�� �ִ� ������ ȸ��
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
	//custom
	UPROPERTY(EditAnywhere, Category = JumpAttack)
		float jumpAttackRangeStart = 1000;
	UPROPERTY(EditAnywhere, Category = JumpAttack)
		float jumpAttackRangeEnd = 3200;
	UPROPERTY(EditAnywhere, Category = JumpAttack)
		float jumpAttackDamage = 40;
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
		float throwStoneAttackRangeStart = 1100;
	UPROPERTY(EditAnywhere, Category = ThrowAttack)
		float throwStoneAttackRangeEnd = 3210;
	UPROPERTY(EditAnywhere, Category = ThrowAttack)
		float throwStoneAttackDamage = 40;
	UPROPERTY(EditAnywhere, Category = ThrowAttack)
		float throwStoneAttackCoolTime = 4.2;
	UPROPERTY(EditAnywhere, Category = ThrowAttack)
		float throwPower = 275000;
	//fix
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = ThrowAttack)
		float throwStoneAttackCurrentTime = 0;

};
