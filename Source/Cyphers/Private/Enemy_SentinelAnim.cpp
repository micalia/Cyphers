// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_SentinelAnim.h"
#include "Enemy_Sentinel.h"
#include "Cypher_Kaya.h"
#include <Components/BoxComponent.h>

void UEnemy_SentinelAnim::NativeBeginPlay()
{
	//GetOwningActor(); <<-------- Actor �� �����´�.
	/*AActor* actor = GetOwningActor();
	APawn* p =Cast<APawn>(actor);*/

	//1. ���� �پ� �ִ� Pawn �� �����´�.
	APawn* pawn = TryGetPawnOwner();

	//2. AEnemy �� ����ȯ�Ѵ�.
	enemy = Cast<AEnemy_Sentinel>(pawn);
}

void UEnemy_SentinelAnim::AnimNotify_DieEnd()
{
	//3. enemy �� ������ �ִ� fsm �� �̿��ؼ� bDieMove �� true
	enemy->fsm->bDieMove = true;

	//Cast<AEnemy>(TryGetPawnOwner())->fsm->bDieMove = true;
}

void UEnemy_SentinelAnim::AnimNotify_EnemyAttack()
{
	enemy->attackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UEnemy_SentinelAnim::AnimNotify_EnemyAttackEnd()
{
	enemy->attackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
