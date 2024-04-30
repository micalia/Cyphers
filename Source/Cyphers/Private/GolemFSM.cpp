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

	if (bTurnComplete != true) return;
	if (LookAtPlayerAfterAttack()) {
		//UE_LOG(LogTemp, Warning, TEXT("turning End!"))
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
//0.1214..(Bn궤적 포인트부터 Bn+1궤적 포인트 까지 걸리는 시간) = 0.85(Player까지 도달하는 시간) / 7(베지어 곡선 포인트 갯수)
	float PointToPointMoveTime = jumpMovingTime / me->curvePointCount;
	
	if (PtoPCurrTime < PointToPointMoveTime) {
		float alpha = PtoPCurrTime / PointToPointMoveTime;
		//궤적 포인트 구간별로 캐릭터가 부드럽게 이동하기 위해 Lerp를 활용하였음
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

	//대쉬어택
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
	if (targetDistanceLength > jumpAttackRangeStart &&
		targetDistanceLength < jumpAttackRangeEnd)
	{//점프공격 체크
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

	//돌던지기 체크
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
	PlayerPosition = target->GetActorLocation();
	PrevTurnAngle = me->GetActorRotation().Yaw;

	// 골렘 기준으로 플레이어가 있는 방향을 구함
	FVector Dir = PlayerPosition - me->GetActorLocation();
	// 방향만 필요하기 때문에 정규화를 해줌
	ToPlayerDir = Dir.GetSafeNormal();
	float Dot = FVector::DotProduct(me->GetActorForwardVector(), ToPlayerDir);
	//cos 세타의 세타 값을 구하기 위해 arccos(0 ~ 파이(3.14))사이 라디안 값으로 반환 해줌
	float Radian = FMath::Acos(Dot); 
	// 라디안 값을 다음 공식을 사용하여 각도로 변환을 해줌
	float AngleToPlayer = Radian * 180 / 3.141592f; // 언리얼 내장함수로도 사용 가능
	// 플레이어가 있는 방향으로 적게 도는 방향을 구하기 위해 외적 공식을 사용하여
	// 골렘 기준으로 플레이어 위치가 시계 방향에 있는지 반시계 방향에 있는지 체크
	FVector cross = FVector::CrossProduct(me->GetActorForwardVector(), ToPlayerDir);
	if (cross.Z >= 0)
	{ // 오른쪽(시계 방향)으로 회전
	//GEngine->AddOnScreenDebugMessage(-1, 999, FColor::Purple, FString::Printf(TEXT("Cross : %f / Dot : %f / Radian : %f / angle : %f / Right Rotate"), cross.Z, Dot, Radian, AngleToPlayer), true, FVector2D(1.5f, 1.5f));
		NextTurnAngle = AngleToPlayer;
	}
	else if (cross.Z < 0)
	{ // 왼쪽(반시계 방향)으로 회전
	//GEngine->AddOnScreenDebugMessage(-1, 999, FColor::Purple, FString::Printf(TEXT("Cross : %f / Dot : %f / Radian : %f / angle : %f/ Left Rotate"), cross.Z, Dot, Radian, AngleToPlayer), true, FVector2D(1.5f, 1.5f));
		NextTurnAngle = -AngleToPlayer;
	}
}

void UGolemFSM::KnockDownAttackCheck()
{
	target->bCameraShake = true;

	FVector StartLocation = me->GetActorLocation() + me->GetActorForwardVector() * KD_startAtkPos;
	FVector AddDistance = me->GetActorForwardVector() * KD_startToEndDistance;
	FVector EndLocation = StartLocation + AddDistance;
	FVector halfSize = KnockDownAttackRange / 2;     //반대로 말하면 halfSize * 2 는 BoxExtent가 됨
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
			GetWorld(), // 이펙트를 생성할 월드
			me->knockDownAttackEffect, // 생성할 파티클 시스템
			hitInfo.ImpactPoint, // 이펙트를 생성할 위치
			me->GetActorRotation(), // 이펙트의 회전값
			true, // 이펙트 자동 파괴 여부
			EPSCPoolMethod::AutoRelease // 이펙트 풀링 방법
		);
		PAC->SetWorldScale3D(me->KD_atk_effect_size);
	}

	// 바닥쪽으로 라인트레이스 쏴서 이펙트 만들것
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
		3     //디버깅 라이프 타임이니까 Tick에서 확인할때는 0.1로 설정해서 하는걸 추천
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
