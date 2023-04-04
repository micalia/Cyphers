// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_SentinelFSM.h"
#include <Animation/AnimMontage.h>
#include <Kismet/GameplayStatics.h>
#include <AIModule/Classes/AIController.h>
#include <NavigationSystem/Public/NavigationSystem.h>
#include <Kismet/KismetMathLibrary.h>
#include "Cypher_Kaya.h"
#include "Enemy_SentinelAnim.h"
#include "Enemy_Sentinel.h"
#include <Components/CapsuleComponent.h>
#include <UMG/Public/Components/WidgetComponent.h>
#include "BlockStone.h"
#include "../CyphersGameModeBase.h"

// Sets default values for this component's properties
UEnemy_SentinelFSM::UEnemy_SentinelFSM()
{
	//SetActive (true / false) 작동 되게 하자!
	bAutoActivate = true;
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	//몽타주 불러오자
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempMontage(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Animations/Mongtage/AM_Enemy_Sentinel/AM_Enemy_SentinelDamage.AM_Enemy_SentinelDamage'"));
	if (tempMontage.Succeeded())
	{
		damageMontage = tempMontage.Object;
	}
}


// Called when the game starts
void UEnemy_SentinelFSM::BeginPlay()
{
	Super::BeginPlay();

	//타겟 찾자
	target = Cast<ACypher_Kaya>(UGameplayStatics::GetActorOfClass(GetWorld(), ACypher_Kaya::StaticClass()));
	//나를 찾자
	me = Cast<AEnemy_Sentinel>(GetOwner());
	
	//anim instance 찾자
	/*USkeletalMeshComponent* mesh = me->GetMesh();
	UAnimInstance animInstance = mesh->GetAnimInstance();
	anim = Cast<UEnemyAnim>(animInstance);*/
	anim = Cast<UEnemy_SentinelAnim>(me->GetMesh()->GetAnimInstance());

	//ai controller 찾자
	ai = Cast<AAIController>(me->GetController());
	//ai = UAIBlueprintHelperLibrary::GetAIController(me);

	disappearZheight = me->GetActorLocation().Z -200;
}


// Called every frame
void UEnemy_SentinelFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (target != nullptr) {
		distWithPlayer = target->GetActorLocation() - me->GetActorLocation();
	}

	switch (currState)
	{
	case EEnemy_SentinelState::Idle:
		UpdateIdle();
		break;
	case EEnemy_SentinelState::Move:
		UpdateMove();
		break;
	case EEnemy_SentinelState::Attack:
		UpdateAttack();
		break;
	case EEnemy_SentinelState::AttackDelay:
		UpdaetAttackDelay();
		break;
	case EEnemy_SentinelState::Damaged:
		UpdateDamaged();
		break;
	case EEnemy_SentinelState::Die:
		UpdateDie();
		break;
	case EEnemy_SentinelState::LieFloor:
		UpdateLieFloor();
		break;
	}
}

void UEnemy_SentinelFSM::UpdateIdle()
{
	if(distWithPlayer.Length() < detectRange) bDetectPlayer = true;

	//만약에 플레이어를 쫓아 갈 수 있니?
	//if (IsTargetTrace())
	//{
	//	//상태를 Move 로 전환
	//	ChangeState(EEnemy_SentinelState::Move);
	//}
	//else
	//{
		//idleDelayTime 이 지나면	
		if (IsWaitComplete(idleDelayTime))
		{
			if(bDetectPlayer == false) return;
			//현재상태를 Move 로 한다.
			ChangeState(EEnemy_SentinelState::Move);
		}
    //	}
}

void UEnemy_SentinelFSM::UpdateMove()
{
	// 시야에 들어왔는지 여부
	bool bTrace = IsTargetTrace();
	
	//1. 타겟을 향하는 방향을 구하고(target - me)
	FVector dir = target->GetActorLocation() - me->GetActorLocation();

	////처음 위치, 나의 현재 위치의 거리
	//float dist = FVector::Distance(originPos, me->GetActorLocation());

	////만약에 dist 가 moveRange 보다 커지면 (움직일 수 있는 반경을 넘어갔다면)
	//if (dist > moveRange)
	//{
	//	//상태를 ReturnPos 로 변경
	//	ChangeState(EEnemy_SentinelState::ReturnPos);
	//}
	////만약에 시야에 들어왔다면
	//else if (bTrace)
	//{
	
		//만약에 target - me 거리가 공격범위보다 작으면
		if (dir.Length() < attackRange)
		{
			//상태를 Attack 으로 변경
			ChangeState(EEnemy_SentinelState::Attack);
		}
		//그렇지 않으면
		else
		{
			//2. 그 방향으S로 이동하고 싶다.
			//////me->AddMovementInput(dir.GetSafeNormal());
			//ai 를 이용해서 목적지까지 이동하고 싶다.	
			EPathFollowingRequestResult::Type re = ai->MoveToActor(target);
		}
	//}
	////시야에 들어오지 않았다면
	//else
	//{
	//	// 랜덤한 위치까지 도착한 후 Idle 상태로 전환
	//	MoveToPos(randPos);
	//}

}

void UEnemy_SentinelFSM::UpdateAttack()
{
	//2. 상태를 AttackDelay 로 전환
	ChangeState(EEnemy_SentinelState::AttackDelay);
}

void UEnemy_SentinelFSM::UpdaetAttackDelay()
{
	//2. 만약에 현재시간이 attackDelayTime 보다 커지면
	if (IsWaitComplete(attackDelayTime))
	{
		//3. target 과 me 거리를 구하자.
		FVector dir = target->GetActorLocation() - me->GetActorLocation();
		float dist = dir.Length();
		//4. 만약에 그거리가 attackRange보다 작으면
		if (dist < attackRange)
		{
			//5. Attack 상태로 전환
			ChangeState(EEnemy_SentinelState::Attack);
		}
		else
		{
			//6. 그렇지 않으면 Idle 상태로 가자
			ChangeState(EEnemy_SentinelState::Idle);
		}
	}
}

void UEnemy_SentinelFSM::UpdateDamaged()
{
	//damageDelayTime 이 지나면
	if (IsWaitComplete(damageDelayTime))
	{
		//Move 상태
		ChangeState(EEnemy_SentinelState::Idle);
	}
}

void UEnemy_SentinelFSM::UpdateDie()
{
	//만약에 bDieMove 가 false 라면 함수를 나가라
	if (bDieMove == false) return;

	//P = P0 + vt
	//1. 아래로 내려가는 위치를 구한다.
	FVector p0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector p = p0 + vt;


	//2. 만약에 p.Z 가 -200 보다 작으면 파괴한다
	if (p.Z < disappearZheight)
	{
		ABlockStone* blockStone = Cast<ABlockStone>(UGameplayStatics::GetActorOfClass(GetWorld(), ABlockStone::StaticClass()));
		blockStone->bNextStage = true;
		
		ACyphersGameModeBase* CyphersGameMode = Cast<ACyphersGameModeBase>(GetWorld()->GetAuthGameMode());
		CyphersGameMode->bCameraShake = true;
		
		UGameplayStatics::PlaySound2D(GetWorld(), CyphersGameMode->earthquakeSound);

		me->Destroy();

		bDieMove = false;
	}
	//3. 그렇지 않으면 해당 위치로 셋팅한다
	else
	{
		me->SetActorLocation(p);
	}
}



void UEnemy_SentinelFSM::UpdateLieFloor()
{
	if (IsWaitComplete(damageDelayTime))
	{
		if (me->currHP <= 0) {
			me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			me->hpWidget->SetVisibility(false);
			currState = EEnemy_SentinelState::Die;
			bDieMove = true;
		}
		else {
			//Move 상태
			ChangeState(EEnemy_SentinelState::Rise);
		}
	}
}

void UEnemy_SentinelFSM::ChangeState(EEnemy_SentinelState state)
{
	//상태 변경 로그를 출력하자
	UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemy_SentinelState"), true);
	if (enumPtr != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s -----> %s"),
			*enumPtr->GetNameStringByIndex((int32)currState),
			*enumPtr->GetNameStringByIndex((int32)state));
	}

	//현재 상태를 갱신
	currState = state;

	//anim 의 상태 갱신
	anim->state = state;

	//현재 시간 초기화
	currTime = 0;

	//ai 의 움직임 멈추자
	ai->StopMovement();

	//상태에 따른 초기설정
	switch (currState)
	{
	case EEnemy_SentinelState::Attack:
		//currTime = attackDelayTime;
		break;
	case EEnemy_SentinelState::Move:
	
		break;
	case EEnemy_SentinelState::Damaged:
	{
		//1. 랜덤한 값을 뽑는다 (0, 1 중)
		int32 rand = FMath::RandRange(0, 1);
		//2. Damage0, Damage1 이란 문자열을 만든다.
		FString sectionName = FString::Printf(TEXT("Damage%d"), rand);
		//3. 몽타주를 플레이한다.
		me->PlayAnimMontage(damageMontage, 1.0f, FName(*sectionName));
	}
	break; 
	case EEnemy_SentinelState::Die:
		//충돌안되게 설정
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		me->hpWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		me->hpWidget->SetVisibility(false);
		//Die 몽타주 실행
		me->PlayAnimMontage(damageMontage, 1.0f, FName(TEXT("Die")));
		break;
	case EEnemy_SentinelState::LieFloor:
		break;
	case EEnemy_SentinelState::Rise:
		me->PlayAnimMontage(damageMontage, 1.0f, TEXT("Rise"));
		break;
	}
}

bool UEnemy_SentinelFSM::IsWaitComplete(float delayTime)
{
	//1. 시간을 흐르게 한다.
	currTime += GetWorld()->DeltaTimeSeconds;

	//2. 만약에 현재시간이 delayTime보다 커지면 
	if (currTime > delayTime)
	{
		//3. 현재시간을 0으로 초기화
		currTime = 0;
		//4. true 반환
		return true;
	}

	//5. 그렇지 않으면 false 를 반환
	return false;
}


bool UEnemy_SentinelFSM::IsTargetTrace()
{
	//1. 나 ----> 플레이어를 향하는 벡터
	FVector dir = target->GetActorLocation() - me->GetActorLocation();

	//2. 나의 앞방향과 1번에 구한 벡터의 내적
	float dotValue = FVector::DotProduct(me->GetActorForwardVector(), dir.GetSafeNormal());

	//3. 2번에 구한 값을 Acos --> 두 벡터의 사이각
	float angle = UKismetMathLibrary::DegAcos(dotValue);

	//4. 만약에 3번에서 구한 값이 30보다 작고(시야각 60)
	//   나 - 타겟 과의 거리가 traceRange 보다 작으면
	if (angle < 30 && dir.Length() < traceRange)
	{
		//Enemy -----> target LineTrace 쏘자!!
		FHitResult hitInfo;
		FCollisionQueryParams param;
		param.AddIgnoredActor(me);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			hitInfo,
			me->GetActorLocation(),
			target->GetActorLocation(),
			ECollisionChannel::ECC_Visibility,
			param);

		//만약에 부딪힌 곳이 있다면
		if (bHit)
		{
			//만약에 부딪힌 놈의 이름이 Player 를 포함하고 있다면
			if (hitInfo.GetActor()->GetName().Contains(TEXT("Player")))
			{
				//5. true 반환
				return true;
			}
		}
	}

	//6. 그렇지 않으면 false 반환	
	return false;

}
