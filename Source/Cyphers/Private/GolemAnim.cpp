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
	UE_LOG(LogTemp, Warning, TEXT("OnAtkEnded!!!"))
}

void UGolemAnim::AnimNotify_TurnToTarget()
{
	enemy->fsm->SetNewGoalDirection();
}

void UGolemAnim::AnimNotify_FootSound()
{
	enemy->PlayFootSound();
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

void UGolemAnim::AnimNotify_JumpAttackEnd()
{
	//파티클 생성
	UE_LOG(LogTemp, Warning, TEXT("attackEnd"))

}

void UGolemAnim::AnimNotify_JumpAttackImpact()
{
	enemy->PlayJumpAttackSound();
	FVector startPos = enemy->JA_EffectPoint->GetComponentLocation();
	FVector endPos = startPos -300;
	FHitResult hitInfo;
	FCollisionQueryParams param;
	param.AddIgnoredActor(enemy->GetOwner());

	bool isHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, param);

	if (isHit == true) {
		
	UParticleSystemComponent* jae = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), enemy->jumpAttackEffect, hitInfo.ImpactPoint, enemy->JA_EffectPoint->GetComponentRotation(), true, EPSCPoolMethod::AutoRelease);
	jae->SetRelativeScale3D(FVector(jaeScale));
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
	FName stoneSpawnSocketName(TEXT("SpawnStoneSocket"));
	if (enemy->stoneOpacity != nullptr) {
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), enemy->stoneOpacity, TEXT("StoneOpacity"), 1);
		UE_LOG(LogTemp, Warning, TEXT("opacity 1 set!!"))
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Fail Set opacity"))
	}

	if (enemy->GetMesh()->DoesSocketExist(stoneSpawnSocketName)) {
		spawnStone = GetWorld()->SpawnActor<AStoneObj>(enemy->stoneFactory,
			enemy->GetMesh()->GetSocketLocation(stoneSpawnSocketName),
			enemy->GetMesh()->GetSocketRotation(stoneSpawnSocketName));
		spawnStone->AttachToComponent(enemy->GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			stoneSpawnSocketName);
	}
	//enemy->bCameraShake = true;
	//GetWorld()->SpawnActor<UStaticMesh>(rockMesh, enemy->GetMesh()->GetSocketLocation(FName(TEXT("RightHand"))));
}

void UGolemAnim::AnimNotify_ThrowStone()
{
	spawnStone->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	spawnStone->compCollision->SetCollisionProfileName(TEXT("StoneObj"));
	spawnStone->compCollision->SetSimulatePhysics(true);

	UE_LOG(LogTemp, Warning, TEXT("player : %s / enemy socket : %s"), *target->GetMesh()->GetBoneLocation(FName(TEXT("Bip001-Head"))).ToString(), *enemy->GetMesh()->GetSocketLocation(FName(TEXT("Bip001-L-Hand"))).ToString())
		//FVector distance = target->GetMesh()->GetBoneLocation(FName(TEXT("Bip001-Head"))) - enemy->GetMesh()->GetSocketLocation(FName(TEXT("Bip001-L-Hand")));
		FVector distance = target->GetMesh()->GetBoneLocation(FName(TEXT("Bip001-Head"))) - spawnStone->GetActorLocation();
	FVector dir = distance.GetSafeNormal();
	FVector force = spawnStone->compCollision->GetMass() * dir * enemy->fsm->throwPower;
	spawnStone->compCollision->AddForceAtLocation(force, spawnStone->compCollision->GetCenterOfMass());
}

void UGolemAnim::AnimNotify_GroundAttackEffect()
{
	enemy->PlayGroundAttackSound();
	FRotator dir = UKismetMathLibrary::FindLookAtRotation(enemy->groundAttackPoint->GetComponentLocation(), target->GetActorLocation());
	dir.Pitch = 0;
	dir.Roll = 0;
	enemy->SpawnGroundAttackCollision(dir);

	dir.Yaw = dir.Yaw - 90;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), enemy->groundAttackEffect, enemy->groundAttackPoint->GetComponentLocation(), dir, true, EPSCPoolMethod::AutoRelease);
}

void UGolemAnim::PlayGroundAttackAnim()
{
	Montage_Play(groundAttackMontage, 1.0f);
}
