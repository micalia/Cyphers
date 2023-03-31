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
	LieFloor,
	Rise,
	Flying,
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

	bool bDetectPlayer;
	FVector distWithPlayer;
	UPROPERTY(EditAnywhere)
	float detectRange = 3500;
	//현재시간
	float currTime = 0;
	//대기 상태 기다리는 시간
	UPROPERTY(EditAnywhere)
	float idleDelayTime = 0.5;

	//쫓아 갈 수 있는 범위
	UPROPERTY(EditAnywhere)
	float traceRange = 1000;

	//공격범위
	UPROPERTY(EditAnywhere)
	float attackRange = 680;
	//공격 대기 시간
	UPROPERTY(EditAnywhere)
	float attackDelayTime = 0.8;

	//피격 대기 시간
	UPROPERTY(EditAnywhere)
	float damageDelayTime = 0.4;


	//죽었을 때 내려가는 속력
	float dieSpeed = 100;
	//죽었을 때 내려가게 하는 변수
	bool bDieMove = false;

	//이동할 수 있는 반경
	UPROPERTY(EditAnywhere)
	float moveRange = 2000;
	//처음 위치를 담아놓을 변수
	FVector originPos;

	//데미지로 누워있는시간
	float LieFloorDelayTime=2;
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
	//누워있음
	void UpdateLieFloor();

	//상태 변경시 한번만!!! 호출 되는 함수
	void ChangeState(EEnemy_SentinelState state);

	//Delay 함수
	bool IsWaitComplete(float delayTime);

	//타겟을 쫓아 갈 수 있니?
	bool IsTargetTrace();

};
