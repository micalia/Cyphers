// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enemy_SentinelFSM.generated.h"

//���� ����
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
	//�������
	UPROPERTY(EditAnywhere)
		EEnemy_SentinelState currState = EEnemy_SentinelState::Idle;

	//Ÿ��
	UPROPERTY(EditAnywhere)
		class ACypher_Kaya* target;
	//��
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
	//����ð�
	float currTime = 0;
	//��� ���� ��ٸ��� �ð�
	UPROPERTY(EditAnywhere)
	float idleDelayTime = 0.5;

	//�Ѿ� �� �� �ִ� ����
	UPROPERTY(EditAnywhere)
	float traceRange = 1000;

	//���ݹ���
	UPROPERTY(EditAnywhere)
	float attackRange = 680;
	//���� ��� �ð�
	UPROPERTY(EditAnywhere)
	float attackDelayTime = 0.8;

	//�ǰ� ��� �ð�
	UPROPERTY(EditAnywhere)
	float damageDelayTime = 0.4;


	//�׾��� �� �������� �ӷ�
	float dieSpeed = 100;
	//�׾��� �� �������� �ϴ� ����
	bool bDieMove = false;

	//�̵��� �� �ִ� �ݰ�
	UPROPERTY(EditAnywhere)
	float moveRange = 2000;
	//ó�� ��ġ�� ��Ƴ��� ����
	FVector originPos;

	//�������� �����ִ½ð�
	float LieFloorDelayTime=2;
public:
	//���
	void UpdateIdle();
	//�̵�
	void UpdateMove();
	//����
	void UpdateAttack();
	//���� ���
	void UpdaetAttackDelay();
	//�ǰ�
	void UpdateDamaged();
	//����
	void UpdateDie();
	//��������
	void UpdateLieFloor();

	//���� ����� �ѹ���!!! ȣ�� �Ǵ� �Լ�
	void ChangeState(EEnemy_SentinelState state);

	//Delay �Լ�
	bool IsWaitComplete(float delayTime);

	//Ÿ���� �Ѿ� �� �� �ִ�?
	bool IsTargetTrace();

};
