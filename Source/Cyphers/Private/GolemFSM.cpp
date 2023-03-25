// Fill out your copyright notice in the Description page of Project Settings.


#include "GolemFSM.h"
#include "Golem.h"
#include "Cypher_Kaya.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include "GolemAnim.h"
#include <GameFramework/CharacterMovementComponent.h>

// Sets default values for this component's properties
UGolemFSM::UGolemFSM()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UGolemFSM::BeginPlay()
{
	Super::BeginPlay();
	me = Cast<AGolem>(GetOwner());
	target = Cast<ACypher_Kaya>(UGameplayStatics::GetActorOfClass(GetWorld(), ACypher_Kaya::StaticClass()));
	anim = Cast<UGolemAnim>(me->GetMesh()->GetAnimInstance());
}


// Called every frame
void UGolemFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bDie == true) return;

	CalculateCurrentTime(DeltaTime);

	targetDistance = target->GetActorLocation() - me->GetActorLocation();
	targetDistanceLength = targetDistance.Length();

	UE_LOG(LogTemp, Warning, TEXT("targetDistanceLength: %f"), targetDistanceLength)

	switch (mState) {
	case EGolemState::AppearBoss:
		if (me->bossAppear == false) return;
		break;
	case EGolemState::Idle:
		IdleState();
		if (me->bossAppear == false) return;
		break;
	case EGolemState::Move:
		MoveState(DeltaTime);
		break;
	case EGolemState::JumpAttack:
		me->currAttackDamage = jumpAttackDamage;
		//UE_LOG(LogTemp, Warning, TEXT("JumpAttack - currTime : %f"), jumpAttackCurrentTime);
		if (jumpAttackOn == true)JumpAttackState();
		/*if (anim->bAttackPlay == false && bTurnComplete == false) {
			SetNewGoalDirection();
			jumpAttackCurrentTime = 0;
		}*/
		break;
	case EGolemState::ThrowStoneAttack:
		me->currAttackDamage = throwStoneAttackDamage;
		//UE_LOG(LogTemp, Warning, TEXT("ThrowStoneAttack - currTime : %f"), throwStoneAttackCurrentTime);
		/*if (anim->bAttackPlay == false && bTurnComplete == false) {
			SetNewGoalDirection();
			throwStoneAttackCurrentTime = 0;
		}*/
		break;
	case EGolemState::GroundAttack:
		groundAttackCurrentTime = 0;
		break;
	case EGolemState::Damage:
		DamageState();
		break;
	case EGolemState::Die:
		DieState();
		break;
	}

	CheckAttackRangeAndCoolTime();

	if (bTurnComplete != true) return;
	if (LookAtPlayerAfterAttack()) {
		UE_LOG(LogTemp, Warning, TEXT("turning End!"))
	}
}
void UGolemFSM::IdleState() {

	if (me->bossAppear == false) {
		mState = EGolemState::Idle;
		anim->animState = mState;
		return;
	}

	if (closestToTargetRange > targetDistanceLength) {
		mState = EGolemState::Idle;
		anim->animState = mState;
	}
	else {
		mState = EGolemState::Move;
		anim->animState = mState;
	}
}
void UGolemFSM::MoveState(float DeltaTime) {
	FVector destination = target->GetActorLocation();
	FVector mm = me->GetActorLocation();
	targetDistance = destination - me->GetActorLocation();
	/* 보스 걷기 루트모션 이동으로 이동 로직이 필요없어짐
	FVector P0 = me->GetActorLocation();
	FVector vt = targetDistance.GetSafeNormal() * bossSpeed * DeltaTime;
	me->SetActorLocation(P0 + vt);*/

	FRotator rotDir = UKismetMathLibrary::MakeRotator(0, 0, UKismetMathLibrary::FindLookAtRotation(mm, destination).Yaw);

	me->SetActorRotation(UKismetMathLibrary::RInterpTo(
		me->GetActorRotation(),
		rotDir,
		GetWorld()->GetDeltaSeconds(), 2));

	if (closestToTargetRange > targetDistanceLength) {
		mState = EGolemState::Idle;
		anim->animState = mState;
	}
}

void UGolemFSM::JumpAttackState()
{
	jumpAttackDeltaTime += GetWorld()->DeltaTimeSeconds;

	float alpha = jumpAttackDeltaTime/JM_Point_BetweentMoveTime;
	if (alpha<1) {
		
		me->SetActorLocation(FMath::Lerp(me->lineLoc[jumpAttackIdx], me->lineLoc[jumpAttackIdx+1], alpha));

	}
	else {
		jumpAttackDeltaTime = 0;
		jumpAttackIdx++;
		if (jumpAttackIdx + 1 == me->lineLoc.Num())
		{
			jumpAttackIdx = 0;
			jumpAttackOn = false;
			me->GetCharacterMovement()->GravityScale = 1;
		}
	}
}


void UGolemFSM::DamageState() {

	mState = EGolemState::Idle;
	anim->animState = mState;
	/* 필요없을시 삭제
	currentTime += GetWorld()->DeltaRealTimeSeconds;
	if (currentTime > damageDelayTime) {
		currentTime = 0;
	}*/
}
void UGolemFSM::DieState() {}

void UGolemFSM::CalculateCurrentTime(float DeltaTime)
{
	jumpAttackCurrentTime += DeltaTime;
	throwStoneAttackCurrentTime += DeltaTime;
	groundAttackCurrentTime += DeltaTime;
	closeKnockDownAttackCurrentTime += DeltaTime;
}

//void UGolemFSM::OnDamageProcess(float damage) {
//	me->hp -= damage;
//
//	if (me->hp <= 0) {
//		bDie = true;
//		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//		me->headCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//		mState = EGolemState::Die;
//		anim->PlayDamageAnim(TEXT("Die"));
//	}
//	/*hp--;
//
//	if (hp > 0) {
//		UE_LOG(LogTemp, Warning, TEXT("hp : %d"), hp);
//		mState = EGolemState::Damage;
//		FString sectionName = "Damage";
//		anim->PlayDamageAnim(FName(*sectionName));
//	}
//	else {
//		UE_LOG(LogTemp, Warning, TEXT("hp : %d .. You Die"), hp);
//		mState = EGolemState::Die;
//		anim->PlayDamageAnim(TEXT("Die"));
//	}
//	anim->animState = mState;
//	*/
//}

void UGolemFSM::CheckAttackRangeAndCoolTime()
{
	/// <summary>
	/// 플레이어와 가장 가까이 붙어있을경우 Move가 아닌 Idle상태로 유지하는 거리 : 200
	/// 보스몬스터의 체력은 1000이다
	/// 보스몬스터에는 4개의 스킬이 존재한다
	/// normal Attack1 (데미지:3 / 범위: 0~350 / 쿨타임: 2초) 
	/// normal Attack2 (데미지:7 / 범위: 500~750 / 쿨타임: 2초)
	/// Jump Attack (데미지:10 / 범위: 1000~3000 / 쿨타임: 7초)
	/// Throw Stone (데미지:10 / 범위: 1100~3010 / 쿨타임: 7초)
	/// 
	/// normal Attack의 쿨타임의 경우
	/// normal Attack1,2 가 쿨타임을 공유하고
	/// JA, TS어택은 별도로 쿨타임이 돌아간다
	/// 
	/// moveState상태에서 계속 distance를 체크하고
	/// 만약 돌던지기 쿨타임과 돌던지기 공격범위의 조건을 만족하면
	/// 
	/// 돌던지기를 하고 그렇지 않으면 점프 공격을 한다
	/// 
	/// </summary>
	if (anim->bAttackPlay == true) return;
	//UE_LOG(LogTemp, Warning, TEXT("targetDistanceLength"))

//근거리 공격
	//근거리 땅 내려찍기
	if (closeKnockDownAttackCurrentTime > closeKnockDownAttackCoolTime) {
		if (targetDistanceLength > closeKnockDownAttackRangeStart &&
			targetDistanceLength < closeKnockDownAttackRangeEnd) {
			anim->bAttackPlay = true;
			mState = EGolemState::CloseKnockDownAttack;
			anim->PlayCloseKnockDownAttack();
			return;
		}
	}

	//그라운드 어택
	if (groundAttackCurrentTime > groundAttackCoolTime) {
		if (targetDistanceLength > groundAttackRangeStart &&
			targetDistanceLength < groundAttackRangeEnd) {
				anim->bAttackPlay = true;
				mState = EGolemState::GroundAttack;
				anim->PlayGroundAttackAnim();
				return;
		}
	}


//원거리 공격
	//돌던지기 범위를 먼저 체크
	if (targetDistanceLength > throwStoneAttackRangeStart &&
		targetDistanceLength < throwStoneAttackRangeEnd)
	{
		//점프 공격 범위하고도 겹치는지 체크
		if (targetDistanceLength > jumpAttackRangeStart &&
			targetDistanceLength < jumpAttackRangeEnd)
		{
			if (UKismetMathLibrary::RandomIntegerInRange(0, 99) > 49) {
				if (jumpAttackCurrentTime > jumpAttackCoolTime) {
					anim->bAttackPlay = true;
					me->MoveJumpAttack();
					mState = EGolemState::JumpAttack;
					anim->animState = mState;
					FString sectionName = "JumpAttack";
					anim->PlayJumpAttackAnim(FName(*sectionName));
					return;
				}
			}
			else {
				if (throwStoneAttackCurrentTime > throwStoneAttackCoolTime) {
					anim->bAttackPlay = true;
					mState = EGolemState::ThrowStoneAttack;
					anim->animState = mState;
					FString sectionName = "ThrowStone";
					anim->PlayThrowStoneAttackAnim(FName(*sectionName));
					return;
				}
			}
		}
		else {
			if (throwStoneAttackCurrentTime > throwStoneAttackCoolTime) {
				anim->bAttackPlay = true;
				mState = EGolemState::ThrowStoneAttack;
				anim->animState = mState;
				FString sectionName = "ThrowStone";
				anim->PlayThrowStoneAttackAnim(FName(*sectionName));
				return;
			}
		}
	}
	if (targetDistanceLength > jumpAttackRangeStart &&
		targetDistanceLength < jumpAttackRangeEnd)
	{//점프공격 체크
		if (jumpAttackCurrentTime > jumpAttackCoolTime) {
			anim->bAttackPlay = true;
			me->MoveJumpAttack();
			mState = EGolemState::JumpAttack;
			anim->animState = mState;
			FString sectionName = "JumpAttack";
			anim->PlayJumpAttackAnim(FName(*sectionName));
			return;
		}
	}
	else {//모든 스킬이 쿨타임일 경우
		if (closestToTargetRange > targetDistanceLength) {
			mState = EGolemState::Idle;
			anim->animState = mState;
		}
		else {
			mState = EGolemState::Move;
			anim->animState = mState;
		}
		//UE_LOG(LogTemp, Warning, TEXT("all skiil is CoolTime...So I am Move"))
		return;
	}
}


//타겟을 향해 회전 실행
bool UGolemFSM::LookAtPlayerAfterAttack()
{
	//먼저 받아온 NextTurnAngle을 절대값으로 바꾼다.
	float absNextTurnAngle = FMath::Abs(NextTurnAngle);
	// 총 회전할 시간 =  최종회전각 / 초당회전 스피드
	float RotationTime = absNextTurnAngle / TurnSpeedForDeltaTime;
	// 현재 회전 시간을 계속 더해준다.
	CurrentRotationTime += GetWorld()->GetDeltaSeconds();
	// 현재 lerp에 들어갈 알파값 = 현재 시간 / 총 회전할 시간
	float t = CurrentRotationTime / RotationTime;

	//1보다 작을 경우 아직 회전중이라는 의미이다.
	if (t < 1)
	{
		// 이전에 즉 회전하기전 가지고 있는 Angle을 기본으로 두고 그값에 보간된 값을 더한다.
		float angle = PrevTurnAngle;
		//받아온 angle값이 음수일 경우 왼쪽으로 회전 양수일경우 오른쪽으로 회전하기위해 구분하여 준다.
		if (NextTurnAngle < 0)
		{
			angle += -1 * (FMath::Lerp<float, float>(0, absNextTurnAngle, t));
		}
		else if (NextTurnAngle > 0)
		{
			angle += FMath::Lerp<float, float>(0, absNextTurnAngle, t);
		}

		me->SetActorRotation(FRotator(0.0f, angle, 0.0f).Quaternion());
		return false;
	}
	else
	{
		//시간이 1을 이상일 경우 각도(기존에 가지고 있던값 + 회전 해야할 값)를 그냥 넣어준다. 
		me->SetActorRotation(FRotator(0.0f, PrevTurnAngle + NextTurnAngle, 0.0f).Quaternion());
		CurrentRotationTime = 0;
		bTurnComplete = false;
		return true;
	}
}

//타겟을 향해 회전 계산
void UGolemFSM::SetNewGoalDirection()
{
	bTurnComplete = true;
	GoalPosition = target->GetActorLocation();
	PrevTurnAngle = me->GetTransform().GetRotation().Euler().Z;

	FVector Dest = FVector(GoalPosition.X, GoalPosition.Y, 0.0f);
	FVector Start = FVector(me->GetTransform().GetLocation().X, me->GetTransform().GetLocation().Y, 0.0f);
	FVector dir = Dest - Start;

	GoalDirection = dir.GetSafeNormal();
	//노말라이징한 두개의 백터를 dot한다. //여기서 축을 Z축으로 하기 위해 두백터의 Z값을 0.0f로 넣어 주었다.
	float dot = FVector::DotProduct(me->GetActorForwardVector(), GoalDirection);
	float AcosAngle = FMath::Acos(dot);    // dot한 값을 아크코사인 계산해 주면 0 ~ 180도 사이의 값 (0 ~ 1)의 양수 값만 나온다.
	float angle = FMath::RadiansToDegrees(AcosAngle); //그값은 degrees 값인데 이것에 1라디안을 곱해주면 60분법의 도가 나온다.

	//여기서 두 백터를 크로스 하여 회전할 축을 얻게 된다.
	//이 크로스 백터는 Axis회전의 회전축이 되며 , 그 양수 음수로 회전 방향 왼쪽(음수), 오른쪽(양수)를 알수 있다.
	FVector cross = FVector::CrossProduct(me->GetActorForwardVector(), GoalDirection);
	FString lr = "center    //";
	if (cross.Z > 0)
	{
		lr = "Right    //";

		NextTurnAngle = angle;
	}
	else if (cross.Z < 0)
	{
		lr = "Left    //";
		//TurnAngle = 360 - angle; //360에서 뺴게되면 양수로 각을 리턴하게 된다.
		NextTurnAngle = -angle;
	}
	FString str = FString::Printf(TEXT("AcosAngle : %f, angle : %f // "), AcosAngle, angle);

	//출력해보기
	/*GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Green, str + lr + GoalDirection.ToString());
	UE_LOG(LogTemp, Warning, TEXT("AcosAngle : %f, angle : %f // "), AcosAngle, angle)*/
}
