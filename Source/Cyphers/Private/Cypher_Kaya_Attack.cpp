// Fill out your copyright notice in the Description page of Project Settings.


#include "Cypher_Kaya_Attack.h"
#include "PlayerAnim.h"
#include "Cypher_Kaya.h"
#include "DrawDebugHelpers.h"
#include "Enemy_Sentinel.h"
#include "PlayerCamera.h"
#include <Kismet/GameplayStatics.h>
#include "../CyphersGameModeBase.h"
#include "PlayerWidget.h"
#include <UMG/Public/Components/ProgressBar.h>
#include "Golem.h"
#include "PowerAttackDecal.h"
#include "PlayerMoveInput.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Enemy_SentinelFSM.h"
#include "Enemy_SentinelAnim.h"
#include <Kismet/KismetMathLibrary.h>

UCypher_Kaya_Attack::UCypher_Kaya_Attack()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCypher_Kaya_Attack::BeginPlay()
{
	Super::BeginPlay();
	kaya = Cast<ACypher_Kaya>(me);
	AttackEndComboState();
	kayaAnim = Cast<UPlayerAnim>(me->GetMesh()->GetAnimInstance());

	kayaAnim->OnAttackHitCheck.AddUObject(this, &UCypher_Kaya_Attack::AttackCheck);
	kayaAnim->OnDashAttackHitCheck.AddUObject(this, &UCypher_Kaya_Attack::DashAttackCheck);
	kayaAnim->OnGripAttackCheck.AddUObject(this, &UCypher_Kaya_Attack::GripAttackCheck);
	kayaAnim->OnGripAttackCheck2.AddUObject(this, &UCypher_Kaya_Attack::GripAttackCheck2);
	kayaAnim->OnMontageEnded.AddDynamic(this, &UCypher_Kaya_Attack::OnAttackMontageEnded);
	kayaAnim->OnNextAttackCheck.AddLambda([this]()->void {
		if (kaya->bDamageState == false) {
			//UE_LOG(LogTemp, Warning, TEXT("M"))
			ABLOG(Warning, TEXT("OnNextAttackCheck"));
			//CanNextCombo=false;
			if (IsComboInputOn) {
				AttackStartComboState();
				kayaAnim->BasicAttackMontageSection(CurrentCombo);
			}
		}
		});

	kayaAnim->OnNextDashCheck.AddLambda([this]()->void {
		if (kaya->bDamageState == false) {
			if (bDashComboOn) {
				UE_LOG(LogTemp, Warning, TEXT("NextDash"))
					UE_LOG(LogTemp, Warning, TEXT("CurrentDashCombo : %d"), CurrentDashCombo)
					if (CurrentDashCombo < 1) {
						bNextDirValCheck = false;
						DashStartComboState();
						kayaAnim->PlayDashAnim();

					}
			}
		}
		});
}

void UCypher_Kaya_Attack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//잡기공격
	if (bIsGripAttacking) {
		gripMoveCurrTime += DeltaTime;

		switch (gripIndex)
		{
		case 1:
			GA_alpha = gripMoveCurrTime / grip1MoveTime;
			if (GA_alpha > 1) {
				gripMoveCurrTime = 0;
				gripIndex++;
			}
			kaya->SetActorLocation(FMath::Lerp(GAMovePoints[0], GAMovePoints[1], GA_alpha));
			break;
		case 2:
			GA_alpha = gripMoveCurrTime / grip2MoveTime;
			if (GA_alpha > 1) {
				gripMoveCurrTime = 0;
				gripIndex++;
			}
			kaya->SetActorLocation(FMath::Lerp(GAMovePoints[1], GAMovePoints[2], GA_alpha));
			break;
		case 3:
			GA_alpha = gripMoveCurrTime / grip3MoveTime;
			if (GA_alpha > 1) {
				gripMoveCurrTime = 0;
				gripIndex = 0;
				bIsGripAttacking = false;
				GAMovePoints.Empty();
				break;
			}
			kaya->SetActorLocation(FMath::Lerp(GAMovePoints[2], GAMovePoints[3], GA_alpha));
			break;
		default:
			gripMoveCurrTime = 0;
			gripIndex = 0;
			break;
		}

	}

	//쿨타임 처리
	if (startCoolBothMouse) {
		currbothMouseAttackCool -= DeltaTime;
		kaya->CyphersGameMode->playerWidget->UpdateBothMouseCoolTime(currbothMouseAttackCool, bothMouseAttackCool);
		if (currbothMouseAttackCool < 0) {
			kaya->CyphersGameMode->playerWidget->BothMouseCoolTimeBar->SetVisibility(ESlateVisibility::Hidden);
			startCoolBothMouse = false;
		}
	}

	if (startCoolKeyE) {
		currkeyECool -= DeltaTime;
		kaya->CyphersGameMode->playerWidget->UpdateKeyECoolTime(currkeyECool, keyECool);
		if (currkeyECool < 0) {
			kaya->CyphersGameMode->playerWidget->KeyECoolTimeBar->SetVisibility(ESlateVisibility::Hidden);
			startCoolKeyE = false;
		}
	}

	if (startCoolKeyF) {
		currkeyFCool -= DeltaTime;
		kaya->CyphersGameMode->playerWidget->UpdateKeyFCoolTime(currkeyFCool, keyFCool);
		if (currkeyFCool < 0) {
			kaya->CyphersGameMode->playerWidget->KeyFCoolTimeBar->SetVisibility(ESlateVisibility::Hidden);
			startCoolKeyF = false;
		}
	}

	if (startCoolSpaceBar) {
		currSpaceBarCool -= DeltaTime;
		kaya->CyphersGameMode->playerWidget->UpdateSpaceBarCoolTime(currSpaceBarCool, spaceBarCool);
		if (currSpaceBarCool < 0) {
			kaya->CyphersGameMode->playerWidget->SpaceBarCoolTimerBar->SetVisibility(ESlateVisibility::Hidden);
			startCoolSpaceBar = false;
		}
	}

	if (IsNoComboAttacking == true) {
		currPowerAttackCheck += DeltaTime;
		if (powerAttackStartCheck)return;
		if (PowerAttackStartTime < currPowerAttackCheck) {
			powerAttackStartCheck = true;
		}
	}
	else {
		currPowerAttackCheck = 0;
	}

	if (kaya->bDamageState == false) {
		//마우스 양클릭 체크 후 공격 실행
		if (bAttackInput == true) {
			if (IsNoComboAttacking) {
				InitInput();
				return;
			}
			MouseLRCheckCurrentTime += DeltaTime;
			if (MouseLRCheckCurrentTime > MouseLRCheckTime) {
				if(bGripAttack == true)return;
				if (bLeftMouseButtonPressed == true && bRightMouseButtonPressed == true) {
					if (bGripAttack == true)return;
					InitInput();
					if (startCoolBothMouse) return;
					if (IsAttacking == true) return;
					if(bDash == true)return;
					AttackEndComboState(); // 만약 평타 1회 또는 2회 후에 양클릭을 한경우에는 기본 콤보를 0으로 만듦
					//마우스 양클릭 공격실행	
					UE_LOG(LogTemp, Warning, TEXT("Both Click!!"))
						IsNoComboAttacking = true;
					DashAttack();
				}
				else if (bLeftMouseButtonPressed) {
					if (bDash == true)return;
					if (bGripAttack == true)return;
					InitInput();
					//마우스 왼쪽공격
					UE_LOG(LogTemp, Warning, TEXT("mouseLeft"))
						BasicAttack();
				}
				else if (bRightMouseButtonPressed) {
					if (bDash == true)return;
					if (bGripAttack == true)return;
					InitInput();
					//마우스 오른쪽 공격
					UE_LOG(LogTemp, Warning, TEXT("RightClick"))
				}

			}
		}
	}

	//궁극기 사용 후 카메라 월드좌표에서 캐릭터 기존 카메라 위치로 이동
	if (bBackCameraOringinPos) {
		if (CameraBackTime > currCameraBackTime) {
			currCameraBackTime += DeltaTime;
			kaya->Camera->SetActorLocation(FMath::Lerp(kaya->beforeActCameraPos, kaya->afterActCameraPos, currCameraBackTime / CameraBackTime));
			kaya->Camera->SetActorRotation(FMath::Lerp(kaya->beforeActCameraRot, kaya->afterActCameraRot, currCameraBackTime / CameraBackTime));
		}
		else {
			kaya->bCameraPosFix = false;
			bBackCameraOringinPos = false;
			currCameraBackTime = 0;
		}
	}
}

void UCypher_Kaya_Attack::SetupInputBinding(class UInputComponent* PlayerInputComponent)
{
	Super::SetupInputBinding(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("MouseLeft"), IE_Pressed, this, &UCypher_Kaya_Attack::InputMouseLeft);
	PlayerInputComponent->BindAction(TEXT("MouseRight"), IE_Pressed, this, &UCypher_Kaya_Attack::InputMouseRight);
	PlayerInputComponent->BindAction(TEXT("MouseLeftAndShift"), IE_Pressed, this, &UCypher_Kaya_Attack::InputKeyShiftAndMouseLeft);
	PlayerInputComponent->BindAction(TEXT("KeyF"), IE_Pressed, this, &UCypher_Kaya_Attack::InputKeyF);
	PlayerInputComponent->BindAction(TEXT("KeyE"), IE_Pressed, this, &UCypher_Kaya_Attack::InputKeyE_Pressed);
	PlayerInputComponent->BindAction(TEXT("KeyE"), IE_Released, this, &UCypher_Kaya_Attack::InputKeyE_Released);
	PlayerInputComponent->BindAction(TEXT("SpaceBar"), IE_Pressed, this, &UCypher_Kaya_Attack::InputKeySpaceBar);
}

void UCypher_Kaya_Attack::InitInput()
{
	bAttackInput = false;
	MouseLRCheckCurrentTime = 0;
	bLeftMouseButtonPressed = false;
	bRightMouseButtonPressed = false;
}


void UCypher_Kaya_Attack::AttackCheck()
{
	FVector StartLocation = me->GetActorLocation() + me->GetActorForwardVector() * BasicAttackStartPos;
	FVector AddDistance = me->GetActorForwardVector() * BasicAttackstartToEndDistance;
	FVector EndLocation = StartLocation + AddDistance;
	FVector halfSize = BasicAttackRange / 2;
	FRotator collisionRot = me->GetActorRotation();
	TArray<AActor*> EmptyActorsToIgnore;
	EmptyActorsToIgnore.Add(GetOwner());
	TArray<FHitResult> OutHits;

	bool bResult = UKismetSystemLibrary::BoxTraceMulti(
		GetWorld(),
		StartLocation,
		EndLocation,
		halfSize,
		collisionRot,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5),
		true,
		EmptyActorsToIgnore,
		EDrawDebugTrace::None,
		OutHits,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		3
	);

	if (bResult)
	{
		UE_LOG(LogTemp, Warning, TEXT("OutHitNum : %d"), OutHits.Num())
			for (int32 i = 0; i < OutHits.Num(); i++)
			{
				AActor* hitActor = OutHits[i].GetActor();
				AEnemy_Sentinel* sentinel = Cast<AEnemy_Sentinel>(hitActor);
				if (sentinel != nullptr)
				{
					sentinel->ReceiveDamage();
				}
				AGolem* golem = Cast<AGolem>(hitActor);

				if (golem != nullptr)
				{
					golem->ReceiveDamage(OutHits[i].ImpactPoint);
				}

			}
	}
	//bool bResult1 = UKismetSystemLibrary::BoxTraceSingle(
	//	GetWorld(),
	//	StartLocation,
	//	EndLocation,
	//	halfSize,
	//	collisionRot,
	//	UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5),
	//	false,
	//	EmptyActorsToIgnore,
	//	EDrawDebugTrace::ForDuration,
	//	HitResult,
	//	true,
	//	FLinearColor::Red,
	//	FLinearColor::Green,
	//	3
	//);
		
	/*FHitResult HitResult2;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(me);
	bool bResult2 = me->GetWorld()->SweepSingleByChannel(
		HitResult2,
		me->GetActorLocation(),
		me->GetActorLocation() + me->GetActorForwardVector() * 500 * 3,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel5,
		FCollisionShape::MakeSphere(500 * 3 * 3),
		Params
	);*/
//
//#if ENABLE_DRAW_DEBUG
//
//	//FVector TraceVec = me->GetActorForwardVector() * AttackRange * 3;
//	//FVector Center = me->GetActorLocation() + TraceVec * 0.5f;
//	//float HalfHeight = AttackRange * 3 * 0.5f + AttackRadius * 3;
//	//FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
//	//FColor DrawColor = bResult ? FColor::Green : FColor::Red;
//	//float DebugLifeTime = 5.0f;
//
//	//DrawDebugCapsule(
//	//	GetWorld(),
//	//	Center,
//	//	HalfHeight,
//	//	AttackRadius * 3,
//	//	CapsuleRot,
//	//	DrawColor,
//	//	false,
//	//	DebugLifeTime
//	//);
//#endif
	//AActor* hitActor = HitResult2.GetActor();
//	if (hitActor != nullptr) {
//
//		UE_LOG(LogTemp, Warning, TEXT("hitActor: %s"), *hitActor->GetName())
//	}
//
//	AEnemy_Sentinel* sentinel = Cast<AEnemy_Sentinel>(hitActor);
//	if (bResult)
//	{
//		if (sentinel != nullptr)
//		{
//			sentinel->ReceiveDamage();
//		}
//	}
//
	/*AGolem* golem = Cast<AGolem>(hitActor);
	if (bResult2)
	{
		if (golem != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("HIT2: %s"), *HitResult2.ImpactPoint.ToString())
			golem->ReceiveDamage(HitResult2.ImpactPoint);
		}
	}*/
}

void UCypher_Kaya_Attack::DashAttackCheck()
{
	FVector StartLocation = me->GetActorLocation() + me->GetActorForwardVector() * DashAttackStartPos;
	FVector AddDistance = me->GetActorForwardVector() * DashAttackstartToEndDistance;
	FVector EndLocation = StartLocation + AddDistance;
	FVector halfSize = DashAttackRange / 2;
	FRotator collisionRot = me->GetActorRotation();
	TArray<AActor*> EmptyActorsToIgnore;
	EmptyActorsToIgnore.Add(GetOwner());
	TArray<FHitResult> OutHits;

	bool bResult = UKismetSystemLibrary::BoxTraceMulti(
		GetWorld(),
		StartLocation,
		EndLocation,
		halfSize,
		collisionRot,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5),
		false,
		EmptyActorsToIgnore,
		EDrawDebugTrace::None,
		OutHits,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		3
	);
	/*bool bResult1 = UKismetSystemLibrary::BoxTraceSingle(
		GetWorld(),
		StartLocation,
		EndLocation,
		halfSize,
		collisionRot,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5),
		false,
		EmptyActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		3
	);*/

		if (bResult)
		{
			for (int32 i = 0; i < OutHits.Num(); i++)
			{
				AActor* hitActor = OutHits[i].GetActor();
				AEnemy_Sentinel* sentinel = Cast<AEnemy_Sentinel>(hitActor);
				if (sentinel != nullptr)
				{
					sentinel->ReceiveDamage();
				}
				AGolem* golem = Cast<AGolem>(hitActor);

				if (golem != nullptr)
				{
					golem->ReceiveDamage(OutHits[i].ImpactPoint);
				}

			}
		}

	/*	AActor* hitActor = HitResult.GetActor();
	if (hitActor != nullptr) {

		UE_LOG(LogTemp, Warning, TEXT("hitActor: %s"), *hitActor->GetName())
	}

	AEnemy_Sentinel* sentinel = Cast<AEnemy_Sentinel>(hitActor);
	if (bResult)
	{
		if (sentinel != nullptr)
		{
			sentinel->ReceiveDamage();
		}
	}*/

	//	FHitResult HitResult;
	//	FCollisionQueryParams Params;
	//	Params.AddIgnoredActor(me);
	//	bool bResult = me->GetWorld()->SweepSingleByChannel(
	//		HitResult,
	//		me->GetActorLocation(),
	//		me->GetActorLocation() + me->GetActorForwardVector() * DashAttackRange * 3,
	//		FQuat::Identity,
	//		ECollisionChannel::ECC_GameTraceChannel5,
	//		FCollisionShape::MakeSphere(DashAttackRadius * 3 * 3),
	//		Params
	//	);
	//
	//	//DrawDebugSphere(GetWorld(), me->GetActorLocation(), 20.0f, 32, FColor::Purple, false, 5.0f);
	//	//DrawDebugSphere(GetWorld(), me->GetActorLocation() + me->GetActorForwardVector() * DashAttackRange * 3, 20.0f, 32, FColor::Green, false, 5.0f);
	//	//DrawDebugSphere(GetWorld(), me->GetActorLocation() + me->GetActorForwardVector() * DashAttackRange * 3, DashAttackRadius * 3 *3, 32, FColor::White, false, 5);
	//#if ENABLE_DRAW_DEBUG
	//
	//	/*FVector TraceVec = me->GetActorForwardVector() * DashAttackRange * 3;
	//	FVector Center = me->GetActorLocation() + TraceVec * 0.5f;
	//	float HalfHeight = DashAttackRange * 3 * 0.5f + DashAttackRadius * 3;
	//	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	//	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	//	float DebugLifeTime = 5.0f;
	//
	//	DrawDebugCapsule(
	//		GetWorld(),
	//		Center,
	//		HalfHeight,
	//		DashAttackRadius * 3,
	//		CapsuleRot,
	//		DrawColor,
	//		false,
	//		DebugLifeTime
	//	);*/
	//#endif
		/*AActor* hitActor = HitResult.GetActor();
		AEnemy_Sentinel* sentinel = Cast<AEnemy_Sentinel>(hitActor);
		if (bResult)
		{

			if (sentinel != nullptr)
			{
				sentinel->ReceiveDamage();
			}
		}
		AGolem* golem = Cast<AGolem>(hitActor);
		if (bResult)
		{
			if (golem != nullptr)
			{
				golem->ReceiveDamage(HitResult.ImpactPoint);
			}
		}*/

	kaya->PlayDashAttackSound();
}

void UCypher_Kaya_Attack::DashStartComboState()
{
	bDashComboOn = false;
	CurrentDashCombo = FMath::Clamp<int32>(CurrentDashCombo + 1, 1, MaxDashCombo);
}

void UCypher_Kaya_Attack::DashEndComboState()
{
	bDashComboOn = false;
	CurrentDashCombo = 0;
}

void UCypher_Kaya_Attack::Dash()
{
	if (bDashOn) {
		bDashComboOn = true;
		if (bNextDirValCheck == false) {
			bNextDirValCheck = true;
		}
	}
	else {
		bDashOn = true;
		kayaAnim->PlayDashAnim();
	}
}

FVector UCypher_Kaya_Attack::GA_MoveNextPoint(FVector startPos, FVector endPos)
{
	FHitResult hitInfo;
	FCollisionQueryParams param;
	param.AddIgnoredActor(kaya);

	bool isHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, param);
	//DrawDebugLine(GetWorld(), startPos, endPos, FColor::Blue, false, 3, 0, 8);

	if (isHit) {
		//UE_LOG(LogTemp, Warning, TEXT("GA_HitName : %s"), *hitInfo.GetComponent()->GetName())
		return hitInfo.ImpactPoint;
	}

	return endPos;
}

void UCypher_Kaya_Attack::GripAttackCheck()
{
	FVector StartLocation = me->GetActorLocation() + me->GetActorForwardVector() * startGripAtkPos;
	FVector AddDistance = me->GetActorForwardVector() * startToEndDistance;
	FVector EndLocation = StartLocation + AddDistance;
	FVector halfSize = gripAttackRange / 2;
	FRotator collisionRot = me->GetActorRotation();
	TArray<AActor*> EmptyActorsToIgnore;
	EmptyActorsToIgnore.Add(GetOwner());
	FHitResult HitResult;

	bool bResult = UKismetSystemLibrary::BoxTraceSingle(
		GetWorld(),
		StartLocation,
		EndLocation,
		halfSize,
		collisionRot,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5),
		false,
		EmptyActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		3
	);
	UE_LOG(LogTemp, Warning, TEXT("TraceSingCall!!"))
		AActor* hitActor = HitResult.GetActor();
	if (hitActor != nullptr) {

		UE_LOG(LogTemp, Warning, TEXT("hitActor: %s"), *hitActor->GetName())
	}

	AEnemy_Sentinel* sentinel = Cast<AEnemy_Sentinel>(hitActor);
	if (bResult)
	{
		if (sentinel != nullptr)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), kaya->GA1_Sound);

			ga1Check = true;
			FVector destination = sentinel->GetActorLocation();
			FVector myPos = me->GetActorLocation();
			FVector moveEnemyPos = me->GetActorLocation() + me->GetActorForwardVector() * moveHitEnemyPos;
			sentinel->SetActorLocation(moveEnemyPos);
			FRotator rotDir = UKismetMathLibrary::MakeRotator(0, 0, UKismetMathLibrary::FindLookAtRotation(destination, myPos).Yaw);
			sentinel->SetActorRotation(rotDir);
			sentinel->fsm->ChangeState(EEnemy_SentinelState::Flying);
			sentinel->fsm->anim->PlayGripAttackDamageAnim();
			sentinel->ReceiveGripAttackDamage();
		}
	}


	AGolem* golem = Cast<AGolem>(hitActor);
	if (bResult)
	{
		if (golem != nullptr)
		{
			golem->ReceiveDamage(HitResult.ImpactPoint);
		}
	}
}

void UCypher_Kaya_Attack::GripAttackCheck2()
{
	FVector StartLocation = me->GetActorLocation() + me->GetActorForwardVector() * startGripAtkPos;
	FVector AddDistance = me->GetActorForwardVector() * startToEndDistance;
	FVector EndLocation = StartLocation + AddDistance;
	FVector halfSize = gripAttackRange / 2;
	FRotator collisionRot = me->GetActorRotation();
	TArray<AActor*> EmptyActorsToIgnore;
	EmptyActorsToIgnore.Add(GetOwner());
	FHitResult HitResult;

	bool bResult = UKismetSystemLibrary::BoxTraceSingle(
		GetWorld(),
		StartLocation,
		EndLocation,
		halfSize,
		collisionRot,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5),
		false,
		EmptyActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		3
	);
	UE_LOG(LogTemp, Warning, TEXT("TraceSingCall!!"))
		AActor* hitActor = HitResult.GetActor();
	if (hitActor != nullptr) {

		UE_LOG(LogTemp, Warning, TEXT("hitActor: %s"), *hitActor->GetName())
	}

	AEnemy_Sentinel* sentinel = Cast<AEnemy_Sentinel>(hitActor);
	if (bResult)
	{
		if (sentinel != nullptr)
		{
			if (ga1Check == false) {
				UGameplayStatics::PlaySound2D(GetWorld(), kaya->GA2_Sound);
				FVector destination = sentinel->GetActorLocation();
				FVector myPos = me->GetActorLocation();
				/* 보스 걷기 루트모션 이동으로 이동 로직이 필요없어짐
				FVector P0 = me->GetActorLocation();
				FVector vt = targetDistance.GetSafeNormal() * bossSpeed * DeltaTime;
				me->SetActorLocation(P0 + vt);*/
				FVector moveEnemyPos = me->GetActorLocation() + me->GetActorForwardVector() * moveHitEnemyPos;
				sentinel->SetActorLocation(moveEnemyPos);
				FRotator rotDir = UKismetMathLibrary::MakeRotator(0, 0, UKismetMathLibrary::FindLookAtRotation(destination, myPos).Yaw);
				sentinel->SetActorRotation(rotDir);
				sentinel->fsm->ChangeState(EEnemy_SentinelState::Flying);
				sentinel->fsm->anim->PlayGripAttackDamage2Anim();
			}
			ga1Check = false;
			sentinel->ReceiveGripAttackDamage();
		}
	}

	AGolem* golem = Cast<AGolem>(hitActor);
	if (bResult)
	{
		if (golem != nullptr)
		{
			golem->ReceiveDamage(HitResult.ImpactPoint);
		}
	}
}

bool UCypher_Kaya_Attack::CheckCurrState()
{
	if (IsNoComboAttacking == true || IsAttacking == true) {
		return true;
	}
	return false;
}

void UCypher_Kaya_Attack::StartPowerAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("start powerAttack"))
		kaya->bCameraPosFix = true;
	kaya->DetachCameraActor();
	bAttackCharge = false;
	kayaAnim->PowerAttackPlayAnim();
}

void UCypher_Kaya_Attack::BasicAttack()
{
	if (IsAttacking) { //현재 이미 좌클릭 공격이 실행중인데 입력값이 들어오면
		//ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		//if (CanNextCombo) {
		IsComboInputOn = true;
		//}
	}
	else { //첫번째 공격
		//ABCHECK(CurrentCombo == 0);
		AttackStartComboState();
		kayaAnim->BasicAttackPlayAnim();
		kayaAnim->BasicAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void UCypher_Kaya_Attack::AttackStartComboState()
{
	//CanNextCombo = true;
	IsComboInputOn = false;
	//ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo-1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void UCypher_Kaya_Attack::AttackEndComboState()
{
	IsComboInputOn = false;
	//CanNextCombo = false;
	CurrentCombo = 0;
}

void UCypher_Kaya_Attack::InputMouseLeft()
{
	//ClickTimestamp = FPlatformTime::Seconds();
	bAttackInput = true;
	bLeftMouseButtonPressed = true;
}

void UCypher_Kaya_Attack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted == true) return;
	//ABCHECK(CurrentCombo > 0);
	bGripAttack = false;
	bDash = false;
//대쉬 상태였다면
	if (bDashOn) {
		bDashOn = false;
		DashEndComboState();
		startCoolSpaceBar = true;
		kaya->CyphersGameMode->playerWidget->SpaceBarCoolTimerBar->SetVisibility(ESlateVisibility::Visible);
	}
	kaya->bDamageState = false;
	IsAttacking = false;
	powerAttackStartCheck = false;
	IsNoComboAttacking = false;
	bNotDamageMotion = false;
	AttackEndComboState();
}

void UCypher_Kaya_Attack::InputMouseLeftAndRight()
{
	UE_LOG(LogTemp, Warning, TEXT("InputMouseLeftAndRight()"))
}

void UCypher_Kaya_Attack::InputKeyShiftAndMouseLeft()
{
	UE_LOG(LogTemp, Warning, TEXT("Shift mouse left!"))
}

void UCypher_Kaya_Attack::InputKeyF()
{
	if (startCoolKeyF)return;
	if (IsAttacking == true) return;
	if (IsNoComboAttacking == true) return;
	if (bDash == true)return;
	UE_LOG(LogTemp, Warning, TEXT("InputF()!!!"))
		bGripAttack = true;
		startCoolKeyF = true;
	currkeyFCool = keyFCool;
	kaya->CyphersGameMode->playerWidget->KeyFCoolTimeBar->SetVisibility(ESlateVisibility::Visible);
	//잡기 공격 거리 계산
	//전방에 벽이 있는지 체크

	FVector P0 = kaya->GetActorLocation();
	FVector dir = kaya->GetActorForwardVector();
	dir.Z = P0.Z;
	FVector distance;

	GAMovePoints.Insert(P0, 0);

	distance = P0 + FVector(dir.X * grip1MoveDistance, dir.Y * grip1MoveDistance, 0);
	GAMovePoints.Insert(GA_MoveNextPoint(GAMovePoints[0], distance), 1);

	distance += FVector(dir.X * grip2MoveDistance, dir.Y * grip2MoveDistance, 0);
	GAMovePoints.Insert(GA_MoveNextPoint(GAMovePoints[1], distance), 2);

	distance += FVector(dir.X * grip3MoveDistance, dir.Y * grip3MoveDistance, 0);
	GAMovePoints.Insert(GA_MoveNextPoint(GAMovePoints[2], distance), 3);

	bIsGripAttacking = true;
	gripIndex = 1;

	kayaAnim->GripAttackPlayAnim();
}

void UCypher_Kaya_Attack::InputKeyE_Pressed()
{
	if (startCoolKeyE)return;
	if (IsAttacking == true) return;
	if (IsNoComboAttacking == true) return;
	if (bDash == true)return;
	if (bGripAttack == true)return;
	UE_LOG(LogTemp, Warning, TEXT("E press"))
		kaya->PlayPowerAttackSwordReadySound();
	UGameplayStatics::PlaySound2D(GetWorld(), kaya->powerAttackStart);
	IsNoComboAttacking = true;
	bAttackCharge = true;
	kayaAnim->PowerAttackReadyAnim();
	decal = GetWorld()->SpawnActor<APowerAttackDecal>(kaya->decalFactory, kaya->footPos->GetComponentLocation(), kaya->footPos->GetComponentRotation());

	kaya->Camera->bSkillCamMove = true;
	kaya->Camera->bSkillReady = true;
}

void UCypher_Kaya_Attack::InputKeyE_Released()
{
	if (bAttackCharge == false) return;
	UE_LOG(LogTemp, Warning, TEXT("E Release"))
		kaya->Camera->bSkillReady = false;
		IsAttacking = true;
	bNotDamageMotion = true;
	if (powerAttackStartCheck) {
		powerAttackStartCheck = false;
		StartPowerAttack();
	}
	else {
		float delayTime = PowerAttackStartTime - currPowerAttackCheck;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_PowerAttackStart, this, &UCypher_Kaya_Attack::StartPowerAttack, delayTime, false);
		powerAttackStartCheck = false;
		//GetWorld()->GetTimerManager().ClearTimer(TimerHandle_PowerAttackStart);
	}
}

void UCypher_Kaya_Attack::InputKeySpaceBar()
{
	UE_LOG(LogTemp, Warning, TEXT("Spacebar!"))
		if (startCoolSpaceBar)return;
	if (IsAttacking == true) return;
	if (IsNoComboAttacking == true) return;
	if (bGripAttack == true)return;
	if (CurrentDashCombo > MaxDashCombo) return;
	bDash = true;
	currSpaceBarCool = spaceBarCool;
	dashHorizontal = kaya->compPlayerMove->GetH();
	Dash();
}

void UCypher_Kaya_Attack::InputMouseRight()
{
	//float TimeInterval = FPlatformTime::Seconds() - ClickTimestamp;
	//UE_LOG(LogTemp, Warning, TEXT("Time interval: %f"), TimeInterval)
	bAttackInput = true;
	bRightMouseButtonPressed = true;
}

void UCypher_Kaya_Attack::DashAttack()
{
	startCoolBothMouse = true;
	currbothMouseAttackCool = bothMouseAttackCool;
	if (kaya->CyphersGameMode != nullptr) {
		kaya->CyphersGameMode->playerWidget->BothMouseCoolTimeBar->SetVisibility(ESlateVisibility::Visible);
	}
	IsAttacking = true;
	kayaAnim->DashAttackPlayAnim();
}
