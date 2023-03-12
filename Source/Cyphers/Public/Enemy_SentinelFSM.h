// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enemy_SentinelFSM.generated.h"

//상태 정의
UENUM(BlueprintType)
enum class EEnemy_SentinelState : uint8
{
	Idle,
	Move,
	Attack,
	AttackDelay,
	Damaged,
	Die,
	ReturnPos
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CYPHERS_API UEnemy_SentinelFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemy_SentinelFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	//현재상태
	UPROPERTY(EditAnywhere)
		EEnemy_SentinelState currState = EEnemy_SentinelState::Idle;

	//타겟
	UPROPERTY(EditAnywhere)
		class ACypher_Kaya* target;
	//나
	UPROPERTY(EditAnywhere)
		class AEnemy_Sentinel* me;

	//anim instance
	UPROPERTY(EditAnywhere)
		class UEnemy_SentinelAnim* anim;

	//anim montage
	UPROPERTY(EditAnywhere)
		class UAnimMontage* damageMontage;

	//ai controller 
	UPROPERTY(EditAnywhere)
		class AAIController* ai;

	//현재시간
	float currTime = 0;
	//대기 상태 기다리는 시간
	float idleDelayTime = 2;

	//쫓아 갈 수 있는 범위
	float traceRange = 1000;

	//공격범위
	float attackRange = 200;
	//공격 대기 시간
	float attackDelayTime = 4;

	//피격 대기 시간
	float damageDelayTime = 2;

	//현재 체력
	float currHP;
	//최대 체력
	float maxHP = 1;

	//죽었을 때 내려가는 속력
	float dieSpeed = 100;
	//죽었을 때 내려가게 하는 변수
	bool bDieMove = false;

	//이동할 수 있는 반경
	float moveRange = 2000;
	//처음 위치를 담아놓을 변수
	FVector originPos;

	//랜덤한 위치
	FVector randPos;

public:
	//대기
	void UpdateIdle();
	//이동
	void UpdateMove();
	//공격
	void UpdateAttack();
	//공격 대기
	void UpdaetAttackDelay();
	//피격
	void UpdateDamaged();
	//죽음
	void UpdateDie();
	//리턴
	void UpdateReturnPos();


	//상태 변경시 한번만!!! 호출 되는 함수
	void ChangeState(EEnemy_SentinelState state);

	//공격 받았을 때 처리하는 함수
	void ReceiveDamage();

	//Delay 함수
	bool IsWaitComplete(float delayTime);

	//타겟을 쫓아 갈 수 있니?
	bool IsTargetTrace();

	//해당 위치까지 도착하면 상태를 Idle 로 전환하는 함수
	void MoveToPos(FVector pos);
};
