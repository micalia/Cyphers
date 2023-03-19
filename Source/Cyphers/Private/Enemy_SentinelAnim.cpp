// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_SentinelAnim.h"
#include "Enemy_Sentinel.h"
#include "Cypher_Kaya.h"
#include <Components/BoxComponent.h>
#include "Golem.h"
#include <Kismet/GameplayStatics.h>
#include "../CyphersGameModeBase.h"
#include "NextStageWall.h"
#include "PlayerWidget.h"
#include <UMG/Public/Components/CanvasPanel.h>

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
	AGolem* golem = Cast<AGolem>(UGameplayStatics::GetActorOfClass(GetWorld(), AGolem::StaticClass()));
	golem->bossAppear = true;  
	ACyphersGameModeBase* CyphersGameMode = Cast<ACyphersGameModeBase>(GetWorld()->GetAuthGameMode());
	CyphersGameMode->playerWidget->BossUI->SetRenderOpacity(1);
	ANextStageWall* wall = Cast<ANextStageWall>(UGameplayStatics::GetActorOfClass(GetWorld(), ANextStageWall::StaticClass()));
	wall->Destroy();
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
