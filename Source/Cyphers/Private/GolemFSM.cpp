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
	/* ���� �ȱ� ��Ʈ��� �̵����� �̵� ������ �ʿ������
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
	/* �ʿ������ ����
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
	/// �÷��̾�� ���� ������ �پ�������� Move�� �ƴ� Idle���·� �����ϴ� �Ÿ� : 200
	/// ���������� ü���� 1000�̴�
	/// �������Ϳ��� 4���� ��ų�� �����Ѵ�
	/// normal Attack1 (������:3 / ����: 0~350 / ��Ÿ��: 2��) 
	/// normal Attack2 (������:7 / ����: 500~750 / ��Ÿ��: 2��)
	/// Jump Attack (������:10 / ����: 1000~3000 / ��Ÿ��: 7��)
	/// Throw Stone (������:10 / ����: 1100~3010 / ��Ÿ��: 7��)
	/// 
	/// normal Attack�� ��Ÿ���� ���
	/// normal Attack1,2 �� ��Ÿ���� �����ϰ�
	/// JA, TS������ ������ ��Ÿ���� ���ư���
	/// 
	/// moveState���¿��� ��� distance�� üũ�ϰ�
	/// ���� �������� ��Ÿ�Ӱ� �������� ���ݹ����� ������ �����ϸ�
	/// 
	/// �������⸦ �ϰ� �׷��� ������ ���� ������ �Ѵ�
	/// 
	/// </summary>
	if (anim->bAttackPlay == true) return;
	//UE_LOG(LogTemp, Warning, TEXT("targetDistanceLength"))

//�ٰŸ� ����
	//�ٰŸ� �� �������
	if (closeKnockDownAttackCurrentTime > closeKnockDownAttackCoolTime) {
		if (targetDistanceLength > closeKnockDownAttackRangeStart &&
			targetDistanceLength < closeKnockDownAttackRangeEnd) {
			anim->bAttackPlay = true;
			mState = EGolemState::CloseKnockDownAttack;
			anim->PlayCloseKnockDownAttack();
			return;
		}
	}

	//�׶��� ����
	if (groundAttackCurrentTime > groundAttackCoolTime) {
		if (targetDistanceLength > groundAttackRangeStart &&
			targetDistanceLength < groundAttackRangeEnd) {
				anim->bAttackPlay = true;
				mState = EGolemState::GroundAttack;
				anim->PlayGroundAttackAnim();
				return;
		}
	}


//���Ÿ� ����
	//�������� ������ ���� üũ
	if (targetDistanceLength > throwStoneAttackRangeStart &&
		targetDistanceLength < throwStoneAttackRangeEnd)
	{
		//���� ���� �����ϰ� ��ġ���� üũ
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
	{//�������� üũ
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
	else {//��� ��ų�� ��Ÿ���� ���
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


//Ÿ���� ���� ȸ�� ����
bool UGolemFSM::LookAtPlayerAfterAttack()
{
	//���� �޾ƿ� NextTurnAngle�� ���밪���� �ٲ۴�.
	float absNextTurnAngle = FMath::Abs(NextTurnAngle);
	// �� ȸ���� �ð� =  ����ȸ���� / �ʴ�ȸ�� ���ǵ�
	float RotationTime = absNextTurnAngle / TurnSpeedForDeltaTime;
	// ���� ȸ�� �ð��� ��� �����ش�.
	CurrentRotationTime += GetWorld()->GetDeltaSeconds();
	// ���� lerp�� �� ���İ� = ���� �ð� / �� ȸ���� �ð�
	float t = CurrentRotationTime / RotationTime;

	//1���� ���� ��� ���� ȸ�����̶�� �ǹ��̴�.
	if (t < 1)
	{
		// ������ �� ȸ���ϱ��� ������ �ִ� Angle�� �⺻���� �ΰ� �װ��� ������ ���� ���Ѵ�.
		float angle = PrevTurnAngle;
		//�޾ƿ� angle���� ������ ��� �������� ȸ�� ����ϰ�� ���������� ȸ���ϱ����� �����Ͽ� �ش�.
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
		//�ð��� 1�� �̻��� ��� ����(������ ������ �ִ��� + ȸ�� �ؾ��� ��)�� �׳� �־��ش�. 
		me->SetActorRotation(FRotator(0.0f, PrevTurnAngle + NextTurnAngle, 0.0f).Quaternion());
		CurrentRotationTime = 0;
		bTurnComplete = false;
		return true;
	}
}

//Ÿ���� ���� ȸ�� ���
void UGolemFSM::SetNewGoalDirection()
{
	bTurnComplete = true;
	GoalPosition = target->GetActorLocation();
	PrevTurnAngle = me->GetTransform().GetRotation().Euler().Z;

	FVector Dest = FVector(GoalPosition.X, GoalPosition.Y, 0.0f);
	FVector Start = FVector(me->GetTransform().GetLocation().X, me->GetTransform().GetLocation().Y, 0.0f);
	FVector dir = Dest - Start;

	GoalDirection = dir.GetSafeNormal();
	//�븻����¡�� �ΰ��� ���͸� dot�Ѵ�. //���⼭ ���� Z������ �ϱ� ���� �ι����� Z���� 0.0f�� �־� �־���.
	float dot = FVector::DotProduct(me->GetActorForwardVector(), GoalDirection);
	float AcosAngle = FMath::Acos(dot);    // dot�� ���� ��ũ�ڻ��� ����� �ָ� 0 ~ 180�� ������ �� (0 ~ 1)�� ��� ���� ���´�.
	float angle = FMath::RadiansToDegrees(AcosAngle); //�װ��� degrees ���ε� �̰Ϳ� 1������ �����ָ� 60�й��� ���� ���´�.

	//���⼭ �� ���͸� ũ�ν� �Ͽ� ȸ���� ���� ��� �ȴ�.
	//�� ũ�ν� ���ʹ� Axisȸ���� ȸ������ �Ǹ� , �� ��� ������ ȸ�� ���� ����(����), ������(���)�� �˼� �ִ�.
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
		//TurnAngle = 360 - angle; //360���� ���ԵǸ� ����� ���� �����ϰ� �ȴ�.
		NextTurnAngle = -angle;
	}
	FString str = FString::Printf(TEXT("AcosAngle : %f, angle : %f // "), AcosAngle, angle);

	//����غ���
	/*GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Green, str + lr + GoalDirection.ToString());
	UE_LOG(LogTemp, Warning, TEXT("AcosAngle : %f, angle : %f // "), AcosAngle, angle)*/
}
