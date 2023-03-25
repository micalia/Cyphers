// Fill out your copyright notice in the Description page of Project Settings.

#include "Golem.h"
#include <Engine/SkeletalMesh.h>
#include "StoneObj.h"
#include "GolemAnim.h"
#include <Components/CapsuleComponent.h>
#include <Components/SceneComponent.h>
#include "Cypher_Kaya.h"
#include "GolemFSM.h"
#include <Kismet/GameplayStatics.h>
#include <Materials/MaterialParameterCollection.h>
#include "../CyphersGameModeBase.h"
#include "PlayerWidget.h"
#include <Particles/ParticleSystem.h>
#include "GolemGroundAttackCollision.h"

AGolem::AGolem()
{
	PrimaryActorTick.bCanEverTick = true;
	
	//뼈대 수정한 골렘을 넣어야 함
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Models/Rock_Giant/ModifyModel/FinalModifyGolem.FinalModifyGolem'"));
	if (tempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}
	GetMesh()->SetRelativeLocationAndRotation(FVector(-28, 0, -196), FRotator(0, -90, 0));

	static ConstructorHelpers::FClassFinder<AStoneObj> tempStoneObj(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/BP_StoneObj.BP_StoneObj_C'"));
	if (tempStoneObj.Succeeded()) {
		stoneFactory = tempStoneObj.Class;
	}

	static ConstructorHelpers::FClassFinder<UGolemAnim> tempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/Animation/ABP_Golem.ABP_Golem_C'"));
	if (tempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnim.Class);
	}
	
	GetCapsuleComponent()->SetCapsuleHalfHeight(196);
	GetCapsuleComponent()->SetCapsuleRadius(125);

	fsm = CreateDefaultSubobject<UGolemFSM>(TEXT("UGolemFSM"));

	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> tempMPC(TEXT("/Script/Engine.MaterialParameterCollection'/Game/Materials/MPC/MPC_StoneOpacity.MPC_StoneOpacity'"));
	if (tempMPC.Succeeded()) {
		stoneOpacity = tempMPC.Object;
	}

	groundAttackPoint = CreateDefaultSubobject<USceneComponent>(TEXT("groundAttackPoint"));
	groundAttackPoint->SetupAttachment(GetCapsuleComponent());
	groundAttackPoint->SetRelativeLocation(FVector(164, -34, -190));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> tempGroundAttackEffect(TEXT("/Script/Engine.ParticleSystem'/Game/Resources/Effect/GroundAttacks/Fx/Earth/P_EarthHexGround5.P_EarthHexGround5'"));
	if (tempGroundAttackEffect.Succeeded()) {
		groundAttackEffect = tempGroundAttackEffect.Object;
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem> tempJumpAttackEffect(TEXT("/Script/Engine.ParticleSystem'/Game/Resources/Effect/GroundAttacks/Fx/Earth/P_EarthGroundAttack.P_EarthGroundAttack'"));
	if (tempJumpAttackEffect.Succeeded()) {
		jumpAttackEffect = tempJumpAttackEffect.Object;
	}

	static ConstructorHelpers::FClassFinder<AGolemGroundAttackCollision> tempGACObj(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/BP_GolemGroundAttackCollision.BP_GolemGroundAttackCollision_C'"));
	if (tempGACObj.Succeeded()) {
		GA_CollisionFactory = tempGACObj.Class;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> tempFootSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/GiantFootSound.GiantFootSound'"));
	if (tempFootSound.Succeeded()) {
		footSound = tempFootSound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempJumpAttackSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/JumpAttackSound2.JumpAttackSound2'"));
	if (tempJumpAttackSound.Succeeded()) {
		jumpAttackSound = tempJumpAttackSound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempGroundAttackSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/GroundAttack.GroundAttack'"));
	if (tempGroundAttackSound.Succeeded()) {
		groundAttackSound = tempGroundAttackSound.Object;
	}
	
	JA_EffectPoint = CreateDefaultSubobject<USceneComponent>(TEXT("JA_EffectPoint"));
	JA_EffectPoint->SetupAttachment(GetCapsuleComponent());
	JA_EffectPoint->SetRelativeLocation(FVector(148, 0, -86));


}
 
void AGolem::BeginPlay()
{
	Super::BeginPlay();
	mainPlayer = Cast<ACypher_Kaya>(UGameplayStatics::GetActorOfClass(GetWorld(), ACypher_Kaya::StaticClass()));
	CyphersGameMode = Cast<ACyphersGameModeBase>(GetWorld()->GetAuthGameMode());
	maxHP = health;
	currHP = maxHP;

}

void AGolem::Tick(float DeltaTime)
{ 
	CyphersGameMode->playerWidget->UpdateBossCurrHP(currHP, maxHP);
}

void AGolem::MoveJumpAttack() {

	startPos = GetActorLocation();
	originEndPos = mainPlayer->GetActorLocation();
	
	FVector betweenPos = FMath::Lerp(startPos, originEndPos, 0.5);
	betweenPos.Z += 1700;

	JumpAttackPath(startPos, betweenPos, originEndPos);
}

void AGolem::PlayJumpAttackSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), jumpAttackSound);
}

void AGolem::SpawnGroundAttackCollision(FRotator dir)
{
	GetWorld()->SpawnActor<AGolemGroundAttackCollision>(GA_CollisionFactory, groundAttackPoint->GetComponentLocation(), dir);
}

void AGolem::PlayGroundAttackSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), groundAttackSound);
}

void AGolem::JumpAttackPath(FVector start, FVector between, FVector end) {
	lineLoc.Empty();

	float ratio = 1 / curvePointCount;
	for (int32 i = 0; i <= (int32)curvePointCount; i++)
	{
		FVector p = CalculateBezier(ratio * i, start, between, end);
		lineLoc.Add(p);

		DrawDebugSphere(GetWorld(), p, 20.0f, 32, FColor::Red, false, 5.0f);
	}
}

FVector AGolem::CalculateBezier(float ratio, FVector start, FVector between, FVector end)
{
	FVector p01 = FMath::Lerp<FVector, float>(start, between, ratio);
	FVector p12 = FMath::Lerp<FVector, float>(between, end, ratio);
	FVector p = FMath::Lerp<FVector, float>(p01, p12, ratio);

	return p;
}

void AGolem::PlayFootSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), footSound);
}

void AGolem::ReceiveDamage()
{
	currHP--;
	if (currHP > 0)
	{
		//fsm->ChangeState(EEnemy_SentinelState::Damaged);
	}
	//그렇지 않으면 Die 상태로 전환
	else
	{
		FVector orginPos = GetMesh()->GetRelativeLocation();
		GetMesh()->SetRelativeLocation(FVector(orginPos.X, orginPos.Y, orginPos.Z+30));
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		fsm->anim->PlayDieAnim();
		fsm->bDie = true;
		fsm->mState = EGolemState::Die;
	}
}