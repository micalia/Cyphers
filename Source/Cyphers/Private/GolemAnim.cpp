// Fill out your copyright notice in the Description page of Project Settings.


#include "GolemAnim.h"
#include "Golem.h"
#include "Cypher_Kaya.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/KismetMaterialLibrary.h>
#include "StoneObj.h"
#include <Components/SphereComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <Components/BoxComponent.h>
#include <Particles/ParticleSystemComponent.h>
#include "PlayerCamera.h"
#include "../CyphersGameModeBase.h"
#include "PlayerWidget.h"
#include <UMG/Public/Components/CanvasPanel.h>
#include <GeometryCollectionEngine/Public/GeometryCollection/GeometryCollectionComponent.h>
#include <Components/CapsuleComponent.h>

UGolemAnim::UGolemAnim()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempJumpAttackMontage(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Animations/Mongtage/AM_Golem/AM_JumpAttack.AM_JumpAttack'"));
	if (tempJumpAttackMontage.Succeeded()) {
		jumpAttackMontage = tempJumpAttackMontage.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempThrowStoneAttackMontage(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Animations/Mongtage/AM_Golem/AM_ThrowStone.AM_ThrowStone'"));
	if (tempThrowStoneAttackMontage.Succeeded()) {
		throwStoneAttackMontage = tempThrowStoneAttackMontage.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempGroundAttackMontage(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Animations/Mongtage/AM_Golem/AM_GroundAttack.AM_GroundAttack'"));
	if (tempGroundAttackMontage.Succeeded()) {
		groundAttackMontage = tempGroundAttackMontage.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempCloseKnockDownAttackMontage(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Animations/Mongtage/AM_Golem/AM_CloseKnockDownAttackMontage.AM_CloseKnockDownAttackMontage'"));
	if (tempCloseKnockDownAttackMontage.Succeeded()) {
		closeKnockDownAttackMontage = tempCloseKnockDownAttackMontage.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempDashAttackMontage(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Animations/Mongtage/AM_Golem/AM_GolemDashAttack.AM_GolemDashAttack'"));
	if (tempDashAttackMontage.Succeeded()) {
		dashAttackMontage = tempDashAttackMontage.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempDamageMontage(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Animations/Mongtage/AM_Golem/AM_Damage.AM_Damage'"));
	if (tempDamageMontage.Succeeded()) {
		damageMontage = tempDamageMontage.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempPickStoneSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/rock_spawn.rock_spawn'"));
	if (tempPickStoneSound.Succeeded()) {
		pickStoneSound = tempPickStoneSound.Object;
	}
}

void UGolemAnim::NativeBeginPlay()
{
	enemy = Cast<AGolem>(TryGetPawnOwner());
	target = Cast<ACypher_Kaya>(UGameplayStatics::GetActorOfClass(GetWorld(), ACypher_Kaya::StaticClass()));
	OnMontageEnded.AddDynamic(this, &UGolemAnim::OnAttackMontageEnded);
}


void UGolemAnim::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bAttackPlay = false;
	enemy->fsm->mState = EGolemState::Idle;
}

void UGolemAnim::AnimNotify_TurnToTarget()
{
	enemy->fsm->SetNewGoalDirection();
}

void UGolemAnim::AnimNotify_DashAtkActive()
{ 
	enemy->leftPunchCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UGolemAnim::AnimNotify_DashAtkEnd()
{
	enemy->leftPunchCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UGolemAnim::AnimNotify_HideBossHP()
{ 
	ACyphersGameModeBase* gameMode = Cast<ACyphersGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	gameMode->playerWidget->BossUI->SetVisibility(ESlateVisibility::Hidden);
}

void UGolemAnim::AnimNotify_BossRoarShake()
{
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->PlayerCameraManager->StartCameraShake(target->Camera->cameraShake);
}

void UGolemAnim::AnimNotify_FootSound()
{
	enemy->PlayFootSound();
}

void UGolemAnim::AnimNotify_RoarSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), enemy->bossRoar);
}

void UGolemAnim::AnimNotify_DieSound1()
{
	target->bCameraShake = true;
	UGameplayStatics::PlaySound2D(GetWorld(), enemy->dieSound1);
}

void UGolemAnim::AnimNotify_DieSound2()
{
	target->bCameraShake = true;
	UGameplayStatics::PlaySound2D(GetWorld(), enemy->dieSound2);
}

void UGolemAnim::AnimNotify_ShakeCam()
{
	target->bCameraShake = true;
}

void UGolemAnim::PlayDieAnim()
{
	enemy->PlayAnimMontage(damageMontage, 1, TEXT("Die"));
}

void UGolemAnim::OnEndAttackAnimation() {
	bAttackPlay = false;
}

//점프공격
void UGolemAnim::PlayJumpAttackAnim(FName sectionName)
{
	enemy->GetCharacterMovement()->GravityScale = 0;
	enemy->PlayAnimMontage(jumpAttackMontage, 1, sectionName);
}

void UGolemAnim::AnimNotify_JumpAttackStart()
{
	enemy->fsm->mState = EGolemState::JumpAttack;
	enemy->fsm->jumpAttackOn = true;
}

void UGolemAnim::AnimNotify_JumpAttackImpact()
{
	enemy->fsm->jumpAttackCurrentTime = 0;

	target->bCameraShake = true;
	enemy->PlayJumpAttackSound();
	
	FVector StartLocation = enemy->GetActorLocation() + enemy->GetActorForwardVector() * enemy->fsm->JA_startAtkPos;
	FVector AddDistance = enemy->GetActorForwardVector() * enemy->fsm->JA_startToEndDistance;
	FVector EndLocation = StartLocation + AddDistance;
	FVector halfSize = enemy->fsm->JumpAttackRange / 2;     //반대로 말하면 halfSize * 2 는 BoxExtent가 됨
	FRotator collisionRot = enemy->GetActorRotation();
	TArray<AActor*> EmptyActorsToIgnore;
	FHitResult HitResult;

	FVector CenterLoc = (StartLocation + EndLocation) / 2 + FVector::UpVector * 100;
	FVector endLineTracePos = CenterLoc + -enemy->GetActorUpVector() * 10000;
	FHitResult hitInfo;
	FCollisionQueryParams param;
	param.AddIgnoredActor(enemy);
	bool isHit = GetWorld()->LineTraceSingleByChannel(hitInfo, CenterLoc, endLineTracePos, ECC_Visibility, param);

	if (isHit) {
		UParticleSystemComponent* jae = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), enemy->jumpAttackEffect, hitInfo.ImpactPoint, enemy->JA_EffectPoint->GetComponentRotation(), true, EPSCPoolMethod::AutoRelease);
		jae->SetRelativeScale3D(FVector(jaeScale * 3.2)); // 월드 캐릭터 스케일을 3으로 했기때문에 3을 곱해줌

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), CenterLoc.X, CenterLoc.Y, CenterLoc.Z);
		UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), endLineTracePos.X, endLineTracePos.Y, endLineTracePos.Z);
	}

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
	ACypher_Kaya* kaya = Cast<ACypher_Kaya>(hitActor);
	if (bResult)
	{
		if (kaya != nullptr)
		{
			kaya->ReceiveDamage(2);
		}
	}
}

//돌던지기
void UGolemAnim::PlayThrowStoneAttackAnim(FName sectionName)
{
	enemy->PlayAnimMontage(throwStoneAttackMontage, 1, sectionName);
}

void UGolemAnim::AnimNotify_SpawnSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), pickStoneSound);
}

void UGolemAnim::AnimNotify_StoneSpawn()
{
	target->bCameraShake = true; // 돌을 땅에서 들어올릴때 카메라 흔들림 효과 연출
	FName stoneSpawnSocketName(TEXT("SpawnStoneSocket"));
	
	if (enemy->GetMesh()->DoesSocketExist(stoneSpawnSocketName)) {
		spawnStone = GetWorld()->SpawnActor<AStoneObj>(enemy->stoneFactory,
			enemy->GetMesh()->GetSocketLocation(stoneSpawnSocketName),
			enemy->GetMesh()->GetSocketRotation(stoneSpawnSocketName));
		spawnStone->AttachToComponent(enemy->GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			stoneSpawnSocketName);
	}
}

void UGolemAnim::AnimNotify_ThrowStone()
{
	if(spawnStone == nullptr) return;
	enemy->fsm->SetNewGoalDirection();
	enemy->fsm->throwStoneAttackCurrentTime = 0;
	// 돌을 던지는 순간 손에 있는 소켓과 분리시킴
	spawnStone->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	spawnStone->compCollision->SetCollisionProfileName(TEXT("StoneObj"));
	spawnStone->compCollision->SetSimulatePhysics(true);
	//현재 돌의 위치를 기준으로 플레이어의 머리 방향을 구함
	FVector VectorToPlayer = target->GetMesh()->GetBoneLocation(FName(TEXT("Bip001-Head"))) - spawnStone->GetActorLocation();
	FVector dir = VectorToPlayer.GetSafeNormal();
	spawnStone->ThrowDir = dir;
	spawnStone->bThrowOn = true;
}

void UGolemAnim::AnimNotify_GroundAttackEffect()
{
	target->bCameraShake = true;

	enemy->PlayGroundAttackSound();
	FRotator dir = UKismetMathLibrary::FindLookAtRotation(enemy->groundAttackPoint->GetComponentLocation(), target->GetActorLocation());
	dir.Pitch = 0;
	dir.Roll = 0;
	enemy->SpawnGroundAttackCollision(dir);

	dir.Yaw = dir.Yaw - 90;
	UParticleSystemComponent* jae = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), enemy->groundAttackEffect, enemy->groundAttackPoint->GetComponentLocation(), dir, true, EPSCPoolMethod::AutoRelease);
	jae->SetRelativeScale3D(FVector(groundAttackEffectSize));
}

void UGolemAnim::PlayGroundAttackAnim()
{
	Montage_Play(groundAttackMontage, 1.0f);
}

void UGolemAnim::AnimNotify_KnockDownAttackCheck()
{
	OnKnockDownAttackCheck.Broadcast();
}

void UGolemAnim::PlayCloseKnockDownAttack()
{
	Montage_Play(closeKnockDownAttackMontage, 1.0f);
}

void UGolemAnim::PlayDashAttack()
{
	Montage_Play(dashAttackMontage, 1.0f);
}
