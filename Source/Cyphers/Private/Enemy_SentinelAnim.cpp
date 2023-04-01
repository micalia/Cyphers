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
	//AGolem* golem = Cast<AGolem>(UGameplayStatics::GetActorOfClass(GetWorld(), AGolem::StaticClass()));
	//if (golem != nullptr) {
	//	golem->bossAppear = true;  
	//	ACyphersGameModeBase* CyphersGameMode = Cast<ACyphersGameModeBase>(GetWorld()->GetAuthGameMode());
	//	CyphersGameMode->playerWidget->BossUI->SetRenderOpacity(1);
	//	/*ANextStageWall* wall = Cast<ANextStageWall>(UGameplayStatics::GetActorOfClass(GetWorld(), ANextStageWall::StaticClass()));
	//	wall->Destroy();*/
	//	//Cast<AEnemy>(TryGetPawnOwner())->fsm->bDieMove = true;
	//}
}

void UEnemy_SentinelAnim::AnimNotify_EnemyAttack()
{
	enemy->attackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UEnemy_SentinelAnim::AnimNotify_EnemyAttackEnd()
{
	enemy->attackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UEnemy_SentinelAnim::AnimNotify_CheckRiseDelayTime()
{
	enemy->fsm->ChangeState(EEnemy_SentinelState::LieFloor);
}

void UEnemy_SentinelAnim::AnimNotify_ChangeToIdle()
{
	enemy->fsm->ChangeState(EEnemy_SentinelState::Idle);
}

void UEnemy_SentinelAnim::AnimNotify_GA2Sound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), enemy->GA2_Sound);
}

void UEnemy_SentinelAnim::PlayGripAttackDamageAnim()
{
	enemy->PlayAnimMontage(enemy->fsm->damageMontage, 1.0f, FName(TEXT("GripAttackDamage")));
}

void UEnemy_SentinelAnim::PlayGripAttackDamage2Anim()
{
	enemy->PlayAnimMontage(enemy->fsm->damageMontage, 1.0f, FName(TEXT("GripDamage2")));
}
