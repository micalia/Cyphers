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
#include "UMG/Public/Components/Overlay.h"

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
			ABLOG(Warning, TEXT("OnNextAttackCheck"));
			if (IsComboInputOn) {
				AttackStartComboState();
				kayaAnim->BasicAttackMontageSection(CurrentCombo);
			}
		}
		});
	//UCypher_Kaya_Attack::BeginPlay()
	kayaAnim->OnNextDashCheck.AddLambda([this]()->void {
		if (bDashComboOn) { // 현재 1단 스텝 중이라면 2단 스텝 실행
			if (CurrentDashCombo < 1) {
				DashStartComboState();
				kayaAnim->PlayDashAnim();
			}
		}
	});
}

void UCypher_Kaya_Attack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
#pragma region GripAttack
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
	//잡기 쿨타임 UI
	if (startCoolKeyF) {
		currkeyFCool -= DeltaTime;
		kaya->CyphersGameMode->playerWidget->UpdateKeyFCoolTime(currkeyFCool, keyFCool);
		if (currkeyFCool < 0) {
			kaya->CyphersGameMode->playerWidget->KeyFCoolTimeBar->SetVisibility(ESlateVisibility::Hidden);
			startCoolKeyF = false;
		}
	}
#pragma endregion

#pragma region Dash
	// 스페이스바 쿨타임 UI
	if (startCoolSpaceBar) {
		currSpaceBarCool -= DeltaTime;
		if (kaya->CyphersGameMode) {
			kaya->CyphersGameMode->playerWidget->UpdateSpaceBarCoolTime(currSpaceBarCool, spaceBarCool);
		}
		if (currSpaceBarCool < 0) {
			if (kaya->CyphersGameMode) {
				kaya->CyphersGameMode->playerWidget->SpaceBarCoolTimerBar->SetVisibility(ESlateVisibility::Hidden);
			}
			startCoolSpaceBar = false;
		}
	}
#pragma endregion

#pragma region Keyboard(E) Super Skill 
	// 궁극기 쿨타임 UI
	if (kaya->CyphersGameMode) {
		if (startCoolKeyE) {
			currkeyECool -= DeltaTime;
			kaya->CyphersGameMode->playerWidget->UpdateKeyECoolTime(currkeyECool, keyECool);
			if (currkeyECool < 0) {
				kaya->CyphersGameMode->playerWidget->KeyECoolTimeBar->SetVisibility(ESlateVisibility::Hidden);
				startCoolKeyE = false;
			}
		}
	}
	if (bAttackCharge) { // 차지 중이면 시간을 잰다.
		if (powerAttackStartCheck) {
			currPowerAttackCheck = 0;
		}
		else {
			currPowerAttackCheck += DeltaTime;
		}
		// 궁극기 차징 최소시간 1초가 지나면 궁극기 바로 시작할 수 있는 bool을 true
		if (PowerAttackStartTime < currPowerAttackCheck) {
			powerAttackStartCheck = true;
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
#pragma endregion

#pragma region MouseClickSkill_Left or Right or Both
	// 경직 상태에서는 공격할 수 없음
	if (kaya->bDamageState == false) {
		if (bMouseClickChk == true) {
			MouseLRCheckCurrentTime += DeltaTime;
			// 짧은 시간동안 딜레이 하여 마우스 왼쪽, 오른쪽 클릭 여부에 따라 로직 실행
			if (MouseLRCheckCurrentTime > MouseLRCheckTime) {
				if (bLeftMouseButtonPressed == true && bRightMouseButtonPressed == true) {
					InitInput();
					if (!startCoolBothMouse) {
						AttackEndComboState(); // 만약 평타 1회 또는 2회 후에 양클릭을 한경우에는 기본 콤보를 0으로 만듦
						//마우스 양클릭 공격실행	
						UE_LOG(LogTemp, Warning, TEXT("Both Click!!"))
						DashAttack();
					}
				}
				else if (bLeftMouseButtonPressed) {
					InitInput();
					//마우스 왼쪽공격
					UE_LOG(LogTemp, Warning, TEXT("mouseLeft"))
						BasicAttack();
				}
				else if (bRightMouseButtonPressed) {
					InitInput();
					//마우스 오른쪽 공격 : 구현 안됨
					UE_LOG(LogTemp, Warning, TEXT("RightClick"))
				}
			}
		}
	}

	// 양클릭 쿨타임 UI
	if (kaya->CyphersGameMode) {
		if (startCoolBothMouse) {
			currbothMouseAttackCool -= DeltaTime;
			kaya->CyphersGameMode->playerWidget->UpdateBothMouseCoolTime(currbothMouseAttackCool, bothMouseAttackCool);
			if (currbothMouseAttackCool < 0) {
				kaya->CyphersGameMode->playerWidget->BothMouseCoolTimeBar->SetVisibility(ESlateVisibility::Hidden);
				startCoolBothMouse = false;
			}
		}
	}
#pragma endregion
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
	bMouseClickChk = false;
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
	if (bDash == true) {
		// 대쉬가 입력된경우 2번째 대쉬 실행
 		bDashComboOn = true;
	}
	else { // 대쉬 시작
		bDash = true;
		bUsingSkill = &bDash;
//왼쪽 화살표는 -1 / 오른쪽 화살표는 1 / 앞 화살표만 누를경우 0 반환
//플레이어 대쉬 방향을 정하기 위한 함수
		kayaAnim->PlayDashAnim();
	}
}

FVector UCypher_Kaya_Attack::GA_MoveNextPoint(FVector startPos, FVector endPos)
{
	FHitResult hitInfo;
	FCollisionQueryParams param;
	param.AddIgnoredActor(kaya);

	bool isHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, param);

	if (isHit) {
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

void UCypher_Kaya_Attack::StartPowerAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("start powerAttack"))
	kaya->Camera->bSkillReady = false;
	kaya->bCameraPosFix = true;
	kaya->DetachCameraActor();
	kayaAnim->PowerAttackPlayAnim();
	if (kaya->CyphersGameMode) {
		kaya->CyphersGameMode->playerWidget->OverlayKeyEPressing->SetRenderOpacity(0);
	}
}

void UCypher_Kaya_Attack::BasicAttack()
{
	if (!*bUsingSkill || bUsingSkill == &bBasicAttackOn) {
		if (bBasicAttackOn) { //현재 이미 좌클릭 공격이 실행중인데 입력값이 들어오면
			IsComboInputOn = true;
		}
		else { //첫번째 공격
			AttackStartComboState();
			kayaAnim->BasicAttackPlayAnim();
			kayaAnim->BasicAttackMontageSection(CurrentCombo);
			bBasicAttackOn = true;
			bUsingSkill = &bBasicAttackOn;
		}
	}
}

void UCypher_Kaya_Attack::AttackStartComboState()
{
	IsComboInputOn = false;
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void UCypher_Kaya_Attack::AttackEndComboState()
{
	IsComboInputOn = false;
	CurrentCombo = 0;
}

void UCypher_Kaya_Attack::InputMouseLeft()
{
	bMouseClickChk = true;
	bLeftMouseButtonPressed = true;
}

void UCypher_Kaya_Attack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted == true) return;
	//대쉬 상태였다면
  	if (bDash) {
		bDash = false;
		DashEndComboState();
		startCoolSpaceBar = true;
		if (kaya->CyphersGameMode) {
			kaya->CyphersGameMode->playerWidget->SpaceBarCoolTimerBar->SetVisibility(ESlateVisibility::Visible);
		}
	}
	kaya->bDamageState = false;
	bNotDamageMotion = false;
	AttackEndComboState();
	// 사용중인 스킬 
	*bUsingSkill = false;
	bUsingSkill = &bDefaultUsingSkill;
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
	if (startCoolKeyF || *bUsingSkill)return;
	bGripAttack = true;
	*bUsingSkill = bGripAttack;
	UE_LOG(LogTemp, Warning, TEXT("InputF()!!!"))

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
	if (startCoolKeyE || bAttackCharge)return;
	if (*bUsingSkill)return;
	if (kaya->CyphersGameMode) {
		kaya->CyphersGameMode->playerWidget->OverlayKeyEPressing->SetRenderOpacity(1);
	}
	bAttackCharge = true;
	bUsingSkill = &bAttackCharge;
	UE_LOG(LogTemp, Warning, TEXT("E press"))
		kaya->PlayPowerAttackSwordReadySound();
	UGameplayStatics::PlaySound2D(GetWorld(), kaya->powerAttackStart);
	kayaAnim->PowerAttackReadyAnim();
	decal = GetWorld()->SpawnActor<APowerAttackDecal>(kaya->decalFactory, kaya->footPos->GetComponentLocation(), kaya->footPos->GetComponentRotation());

	kaya->Camera->bSkillCamMove = true;
	kaya->Camera->bSkillReady = true;
}

void UCypher_Kaya_Attack::InputKeyE_Released()
{
	// 궁극기 외 입력은 무시
	if (bAttackCharge == false) return;
	if (bUsingSkill != &bAttackCharge)return;
	
	bAttackCharge = false;
	bPowerAttackOn = true;
	bUsingSkill = &bPowerAttackOn;
	bNotDamageMotion = true;
	if (powerAttackStartCheck) {
		StartPowerAttack();
	}
	else {
		float delayTime = PowerAttackStartTime - currPowerAttackCheck;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_PowerAttackStart, this, &UCypher_Kaya_Attack::StartPowerAttack, delayTime, false);
	}
	powerAttackStartCheck = false;
	currPowerAttackCheck = 0;
}

void UCypher_Kaya_Attack::InputKeySpaceBar()
{ // 스페이스바
 	if (startCoolSpaceBar)return;
	if (CurrentDashCombo > MaxDashCombo) return;
//현재 스킬 사용중이 아니거나 또는
//현재 사용중인 스킬의 주소가 스페이스바 스킬(대쉬) 주소와 같다면
//if 문 안에 로직을 실행함
	if (!*bUsingSkill || bUsingSkill == &bDash) {
		currSpaceBarCool = spaceBarCool;
		dashHorizontal = kaya->compPlayerMove->GetH();
 		Dash();
	}
}

void UCypher_Kaya_Attack::InputMouseRight()
{
	bMouseClickChk = true;
	bRightMouseButtonPressed = true;
}

void UCypher_Kaya_Attack::DashAttack()
{
	if (*bUsingSkill) return;
	bDashAttackOn = true;
	bUsingSkill = &bDashAttackOn;
	startCoolBothMouse = true;
	currbothMouseAttackCool = bothMouseAttackCool;
	if (kaya->CyphersGameMode != nullptr) {
		kaya->CyphersGameMode->playerWidget->BothMouseCoolTimeBar->SetVisibility(ESlateVisibility::Visible);
	}
	kayaAnim->DashAttackPlayAnim();
}
