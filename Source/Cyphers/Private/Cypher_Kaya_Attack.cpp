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
	kayaAnim->OnMontageEnded.AddDynamic(this, &UCypher_Kaya_Attack::OnAttackMontageEnded);
	kayaAnim->OnNextAttackCheck.AddLambda([this]()->void {
		//UE_LOG(LogTemp, Warning, TEXT("M"))
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		//CanNextCombo=false;
		if (IsComboInputOn) {
			AttackStartComboState();
			kayaAnim->BasicAttackMontageSection(CurrentCombo);
		}
	});

}

void UCypher_Kaya_Attack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//쿨타임 처리
	if (startCoolBothMouse) {
		currbothMouseAttackCool-=DeltaTime;
		kaya->CyphersGameMode->playerWidget->UpdateBothMouseCoolTime(currbothMouseAttackCool, bothMouseAttackCool);
		if (currbothMouseAttackCool < 0) {
			kaya->CyphersGameMode->playerWidget->BothMouseCoolTimeBar->SetVisibility(ESlateVisibility::Hidden);
			startCoolBothMouse = false;
		}
	}

	if (startCoolKeyE) {
		currkeyECool-=DeltaTime;
		kaya->CyphersGameMode->playerWidget->UpdateKeyECoolTime(currkeyECool, keyECool);
		if (currkeyECool < 0) {
			kaya->CyphersGameMode->playerWidget->KeyECoolTimeBar->SetVisibility(ESlateVisibility::Hidden);
			startCoolKeyE = false;
		}
	}

	if (IsNoComboAttacking == true) {
		currPowerAttackCheck+=DeltaTime;
		if(powerAttackStartCheck)return;
		if (PowerAttackStartTime < currPowerAttackCheck) {
			powerAttackStartCheck = true;
		}
	}
	else {
		currPowerAttackCheck = 0;
	}
	//마우스 양클릭 체크 후 공격 실행
	if (bAttackInput == true) {
		if(IsNoComboAttacking){
			InitInput();
			return;
		}
		MouseLRCheckCurrentTime += DeltaTime;
		if (MouseLRCheckCurrentTime > MouseLRCheckTime) {			
			if (bLeftMouseButtonPressed == true && bRightMouseButtonPressed == true) {
				InitInput();
				if(startCoolBothMouse) return;
				if(IsAttacking == true) return;
				AttackEndComboState(); // 만약 평타 1회 또는 2회 후에 양클릭을 한경우에는 기본 콤보를 0으로 만듦
				//마우스 양클릭 공격실행	
				UE_LOG(LogTemp, Warning, TEXT("Both Click!!"))
				IsNoComboAttacking = true;
				DashAttack();
			}
			else if (bLeftMouseButtonPressed) {
				InitInput();
					//마우스 왼쪽공격
					UE_LOG(LogTemp, Warning, TEXT("mouseLeft"))
					BasicAttack();
			}
			else if(bRightMouseButtonPressed){
				InitInput();
				//마우스 오른쪽 공격
				UE_LOG(LogTemp, Warning, TEXT("RightClick"))
			}
			
		}
	}

	//궁극기 사용 후 카메라 월드좌표에서 캐릭터 기존 카메라 위치로 이동
	if (bBackCameraOringinPos) {
		if (CameraBackTime > currCameraBackTime) {
			currCameraBackTime+=DeltaTime;		
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
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(me);
	bool bResult = me->GetWorld()->SweepSingleByChannel(
		HitResult,
		me->GetActorLocation(),
		me->GetActorLocation() + me->GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel3,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

#if ENABLE_DRAW_DEBUG

	FVector TraceVec = me->GetActorForwardVector() * AttackRange;
	FVector Center = me->GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(
		GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime
	);
#endif
AActor* hitActor = HitResult.GetActor();
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
	}

	AGolem* golem = Cast<AGolem>(hitActor);
	if (bResult)
	{
		if (golem != nullptr)
		{
			golem->ReceiveDamage();
		}
	}
}

void UCypher_Kaya_Attack::DashAttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(me);
	bool bResult = me->GetWorld()->SweepSingleByChannel(
		HitResult,
		me->GetActorLocation(),
		me->GetActorLocation() + me->GetActorForwardVector() * DashAttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel3,
		FCollisionShape::MakeSphere(DashAttackRadius),
		Params
	);

#if ENABLE_DRAW_DEBUG

	FVector TraceVec = me->GetActorForwardVector() * DashAttackRange;
	FVector Center = me->GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = DashAttackRange * 0.5f + DashAttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(
		GetWorld(),
		Center,
		HalfHeight,
		DashAttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime
	);
#endif
	AActor* hitActor = HitResult.GetActor();
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
			golem->ReceiveDamage();
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
	CurrentCombo =FMath::Clamp<int32>(CurrentCombo+1, 1, MaxCombo);
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
	UE_LOG(LogTemp, Warning, TEXT("end???"))
	//ABCHECK(CurrentCombo > 0);
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
	if (IsAttacking == true) return;
	if (IsNoComboAttacking == true) return;
	UE_LOG(LogTemp, Warning, TEXT("InputF()!!!"))
		kayaAnim->GripAttackPlayAnim();
}

void UCypher_Kaya_Attack::InputKeyE_Pressed()
{
	if(startCoolKeyE)return;
	if(IsAttacking == true) return;
	if(IsNoComboAttacking == true) return;
	UE_LOG(LogTemp, Warning, TEXT("E press"))
	UGameplayStatics::PlaySound2D(GetWorld(), kaya->powerAttackStart);
	IsNoComboAttacking = true;
	bAttackCharge = true;
	kayaAnim->PowerAttackReadyAnim();
	decal = GetWorld()->SpawnActor<APowerAttackDecal>(kaya->decalFactory, kaya->footPos->GetComponentLocation(), kaya->footPos->GetComponentRotation());
	
}

void UCypher_Kaya_Attack::InputKeyE_Released()
{
	if (bAttackCharge == false) return;
	UE_LOG(LogTemp, Warning, TEXT("E Release"))
	
	IsAttacking= true;
	bNotDamageMotion = true;
	if (powerAttackStartCheck) {
		powerAttackStartCheck = false;
		StartPowerAttack();
	}
	else {
		float delayTime = PowerAttackStartTime - currPowerAttackCheck;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_PowerAttackStart, this, &UCypher_Kaya_Attack::StartPowerAttack, delayTime, false);
		powerAttackStartCheck = false;\
		//GetWorld()->GetTimerManager().ClearTimer(TimerHandle_PowerAttackStart);
	}
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
	startCoolBothMouse=true;
	currbothMouseAttackCool = bothMouseAttackCool;
	kaya->CyphersGameMode->playerWidget->BothMouseCoolTimeBar->SetVisibility(ESlateVisibility::Visible);
	IsAttacking=true;
	kayaAnim->DashAttackPlayAnim();
}
