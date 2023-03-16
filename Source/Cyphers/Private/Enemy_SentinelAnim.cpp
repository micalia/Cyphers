// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_SentinelAnim.h"
#include "Enemy_Sentinel.h"
#include "Cypher_Kaya.h"
#include <Components/BoxComponent.h>

void UEnemy_SentinelAnim::NativeBeginPlay()
{
	//GetOwningActor(); <<-------- Actor 를 가져온다.
	/*AActor* actor = GetOwningActor();
	APawn* p =Cast<APawn>(actor);*/

	//1. 내가 붙어 있는 Pawn 을 가져온다.
	APawn* pawn = TryGetPawnOwner();

	//2. AEnemy 로 형변환한다.
	enemy = Cast<AEnemy_Sentinel>(pawn);
}

void UEnemy_SentinelAnim::AnimNotify_DieEnd()
{
	//3. enemy 가 가지고 있는 fsm 을 이용해서 bDieMove 를 true
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
