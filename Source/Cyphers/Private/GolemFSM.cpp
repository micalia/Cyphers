// Fill out your copyright notice in the Description page of Project Settings.


#include "GolemFSM.h"
#include "Golem.h"
#include "Cypher_Kaya.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include "GolemAnim.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <Particles/ParticleSystemComponent.h>

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
	anim->OnKnockDownAttackCheck.AddUObject(this, &UGolemFSM::KnockDownAttackCheck);
}


// Called every frame
void UGolemFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bDie == true) return;

	CalculateCurrentTime(DeltaTime);

	if (target != nullptr) {
		targetDistance = target->GetActorLocation() - me->GetActorLocation();
	}

	targetDistanceLength = targetDistance.Length();

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
		if (jumpAttackOn == true)JumpAttackState();
		break;
	case EGolemState::ThrowStoneAttack:
		me->currAttackDamage = throwStoneAttackDamage;
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

	if (bTurnComplete == false) return;
	LookAtPlayerToAttack();
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
	PtoPCurrTime += GetWorld()->DeltaTimeSeconds;
//0.1214..(Bn���� ����Ʈ���� Bn+1���� ����Ʈ ���� �ɸ��� �ð�) = 0.85(Player���� �����ϴ� �ð�) / 7(������ � ����Ʈ ����)
	float PointToPointMoveTime = jumpMovingTime / me->curvePointCount;
	
	if (PtoPCurrTime < PointToPointMoveTime) {
		float alpha = PtoPCurrTime / PointToPointMoveTime;
		//���� ����Ʈ �������� ĳ���Ͱ� �ε巴�� �̵��ϱ� ���� Lerp�� Ȱ���Ͽ���
		me->SetActorLocation(FMath::Lerp(me->lineLoc[jumpAttackIdx], me->lineLoc[jumpAttackIdx + 1], alpha));
	}
	else {
		PtoPCurrTime = 0;
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
}
void UGolemFSM::DieState() {}

void UGolemFSM::CalculateCurrentTime(float DeltaTime)
{
	jumpAttackCurrentTime += DeltaTime;
	throwStoneAttackCurrentTime += DeltaTime;
	groundAttackCurrentTime += DeltaTime;
	closeKnockDownAttackCurrentTime += DeltaTime;
	DashAttackCurrentTime += DeltaTime;
}

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

	//�뽬����
	if (DashAttackCurrentTime > DashAttackCoolTime) {
		if (targetDistanceLength > DashAttackRangeStart &&
			targetDistanceLength < DashAttackRangeEnd) {
			anim->bAttackPlay = true;
			mState = EGolemState::DashAttack;
			anim->PlayDashAttack();
			DashAttackCurrentTime = 0;
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
	if (targetDistanceLength > jumpAttackRangeStart &&
		targetDistanceLength < jumpAttackRangeEnd)
	{//�������� üũ
		if (jumpAttackCurrentTime > jumpAttackCoolTime) {
			anim->bAttackPlay = true;
			me->MakeJumpAttackTrajectory();
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

	//�������� üũ
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
					me->MakeJumpAttackTrajectory();
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
}


//Ÿ���� ���� ȸ�� ����
void UGolemFSM::LookAtPlayerToAttack()
{
	CurrentRotationTime += GetWorld()->GetDeltaSeconds();
	float alpha = CurrentRotationTime / RotationTime;
	//alpha�� 1���� ���� ��� ���� ȸ����
	if (alpha < 1)
	{
		// ȸ���ϱ� �� Angle ���� ȸ���� �� Angle���� ���� ������
		float angle = PrevTurnAngle;
		angle += FMath::Lerp<float, float>(0, NextTurnAngle, alpha);
		me->SetActorRotation(FRotator(0.0f, angle, 0.0f).Quaternion());
	}
	else
	{		
		me->SetActorRotation(FRotator(0.0f, PrevTurnAngle + NextTurnAngle, 0.0f).Quaternion());
		CurrentRotationTime = 0;
		bTurnComplete = false;
	}
}

//Ÿ���� ���� ȸ�� ���
void UGolemFSM::SetNewGoalDirection()
{
 	bTurnComplete = true;
	PlayerPosition = target->GetActorLocation();
	PrevTurnAngle = me->GetActorRotation().Yaw;

	// �� �������� �÷��̾ �ִ� ������ ����
	FVector Dir = PlayerPosition - me->GetActorLocation();
	// ���⸸ �ʿ��ϱ� ������ ����ȭ�� ����
	ToPlayerDir = Dir.GetSafeNormal();
	float Dot = FVector::DotProduct(me->GetActorForwardVector(), ToPlayerDir);
	//cos ��Ÿ�� ��Ÿ ���� ���ϱ� ���� arccos(0 ~ ����(3.14))���� ���� ������ ��ȯ ����
	float Radian = FMath::Acos(Dot); 
	// ���� ���� ���� ������ ����Ͽ� ������ ��ȯ�� ����
	float AngleToPlayer = Radian * 180 / 3.141592f; // �𸮾� �����Լ��ε� ��� ����
	// �÷��̾ �ִ� �������� ���� ���� ������ ���ϱ� ���� ���� ������ ����Ͽ�
	// �� �������� �÷��̾� ��ġ�� �ð� ���⿡ �ִ��� �ݽð� ���⿡ �ִ��� üũ
	FVector Cross = FVector::CrossProduct(me->GetActorForwardVector(), ToPlayerDir);
	if (Cross.Z >= 0)
	{ // ������(�ð� ����)���� ȸ��
		NextTurnAngle = AngleToPlayer;
	}
	else if (Cross.Z < 0)
	{ // ����(�ݽð� ����)���� ȸ��
		NextTurnAngle = -AngleToPlayer;
	}
}

void UGolemFSM::KnockDownAttackCheck()
{
	target->bCameraShake = true;

	FVector StartLocation = me->GetActorLocation() + me->GetActorForwardVector() * KD_startAtkPos;
	FVector AddDistance = me->GetActorForwardVector() * KD_startToEndDistance;
	FVector EndLocation = StartLocation + AddDistance;
	FVector halfSize = KnockDownAttackRange / 2;     //�ݴ�� ���ϸ� halfSize * 2 �� BoxExtent�� ��
	FRotator collisionRot = me->GetActorRotation();
	TArray<AActor*> EmptyActorsToIgnore;
	FHitResult HitResult;

	FVector CenterLoc = (StartLocation + EndLocation) / 2;
	FVector endLineTracePos = CenterLoc + -me->GetActorUpVector() * 10000;
	FHitResult hitInfo;
	FCollisionQueryParams param;
	param.AddIgnoredActor(GetOwner());
	bool isHit = GetWorld()->LineTraceSingleByChannel(hitInfo, CenterLoc, endLineTracePos, ECC_Visibility, param);
	
	if (isHit) {
		UParticleSystemComponent* PAC = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), // ����Ʈ�� ������ ����
			me->knockDownAttackEffect, // ������ ��ƼŬ �ý���
			hitInfo.ImpactPoint, // ����Ʈ�� ������ ��ġ
			me->GetActorRotation(), // ����Ʈ�� ȸ����
			true, // ����Ʈ �ڵ� �ı� ����
			EPSCPoolMethod::AutoRelease // ����Ʈ Ǯ�� ���
		);
		PAC->SetWorldScale3D(me->KD_atk_effect_size);
	}

	// �ٴ������� ����Ʈ���̽� ���� ����Ʈ �����
	//DrawDebugSphere(GetWorld(), CenterLoc, 20.0f, 32, FColor::Red, false, 5.0f);

	bool bResult = UKismetSystemLibrary::BoxTraceSingle(
		GetWorld(),
		StartLocation,
		EndLocation,
		halfSize,
		collisionRot,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel7),
		false,
		EmptyActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		3     //����� ������ Ÿ���̴ϱ� Tick���� Ȯ���Ҷ��� 0.1�� �����ؼ� �ϴ°� ��õ
	);

	AActor* hitActor = HitResult.GetActor();
	if (hitActor != nullptr) {
	UE_LOG(LogTemp, Warning, TEXT("HitAcotor name: %s"), *hitActor->GetName())

	}
	ACypher_Kaya* kaya = Cast<ACypher_Kaya>(hitActor);
	if (bResult)
	{
		if (kaya != nullptr)
		{
			kaya->ReceiveDamage(2);
		}
	}
	
	UGameplayStatics::PlaySound2D(GetWorld(), me->KD_Sound);
}
