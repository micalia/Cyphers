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
#include "EndPoint.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include <UMG/Public/Components/WidgetComponent.h>
#include "DamageIndicator.h"
#include <Kismet/KismetMathLibrary.h>
#include <Particles/ParticleSystemComponent.h>
#include "PlayerAnim.h"
#include "Cypher_Kaya_Attack.h"

AGolem::AGolem()
{
	PrimaryActorTick.bCanEverTick = true;
	
	//»À´ë ¼öÁ¤ÇÑ °ñ·½À» ³Ö¾î¾ß ÇÔ
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
	static ConstructorHelpers::FObjectFinder<UParticleSystem> tempKnockDownAttackEffect(TEXT("/Script/Engine.ParticleSystem'/Game/Resources/Effect/GroundAttacks/Fx/Fire/P_FireGroundAttack4.P_FireGroundAttack4'"));
	if (tempKnockDownAttackEffect.Succeeded()) {
		knockDownAttackEffect = tempKnockDownAttackEffect.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> tempDashAttackEffect(TEXT("/Script/Engine.ParticleSystem'/Game/Resources/Effect/Realistic_Starter_VFX_Pack_Vol2/Particles/Destruction/P_Destruction_Concrete.P_Destruction_Concrete'"));
	if (tempDashAttackEffect.Succeeded()) {
		dashAttackEffect = tempDashAttackEffect.Object;
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
	static ConstructorHelpers::FObjectFinder<USoundBase> tempKD_Sound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/KD_AttackSound.KD_AttackSound'"));
	if (tempKD_Sound.Succeeded()) {
		KD_Sound = tempKD_Sound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempBossRoar(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/GolemRoar.GolemRoar'"));
	if (tempBossRoar.Succeeded()) {
		bossRoar = tempBossRoar.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempDieSound1(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/bossDie1.bossDie1'"));
	if (tempDieSound1.Succeeded()) {
		dieSound1 = tempDieSound1.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempDieSound2(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/bossDie2.bossDie2'"));
	if (tempDieSound2.Succeeded()) {
		dieSound2 = tempDieSound2.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempDashAtkSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/GolemDashAtkPunchSound.GolemDashAtkPunchSound'"));
	if (tempDashAtkSound.Succeeded()) {
		dashAtkSound = tempDashAtkSound.Object;
	}
	
	JA_EffectPoint = CreateDefaultSubobject<USceneComponent>(TEXT("JA_EffectPoint"));
	JA_EffectPoint->SetupAttachment(GetCapsuleComponent());
	JA_EffectPoint->SetRelativeLocation(FVector(148, 0, -86));

	damageIndicatorWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("damageIndicatorWidget"));
	damageIndicatorWidget->SetupAttachment(GetMesh());

	static ConstructorHelpers::FClassFinder<UDamageIndicator> tempDamageIndicatorFactory(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/Widget/WB_DamageIndicator.WB_DamageIndicator_C'"));
	if (tempDamageIndicatorFactory.Succeeded()) {
		DamageIndicatorFactory = tempDamageIndicatorFactory.Class;
	}

	damageIndicatorWidget->SetWidgetClass(DamageIndicatorFactory);

	leftPunchCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("leftPunchCollision"));
	leftPunchCollision->SetupAttachment(GetMesh(), TEXT("Bip001-L-Hand"));
	leftPunchCollision->SetCapsuleRadius(80);
	leftPunchCollision->SetCapsuleHalfHeight(113);
	leftPunchCollision->SetCollisionProfileName(TEXT("EnemyAtkCollision"));
	leftPunchCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
 
void AGolem::BeginPlay()
{
	Super::BeginPlay();
	mainPlayer = Cast<ACypher_Kaya>(UGameplayStatics::GetActorOfClass(GetWorld(), ACypher_Kaya::StaticClass()));
	CyphersGameMode = Cast<ACyphersGameModeBase>(GetWorld()->GetAuthGameMode());
	maxHP = health;
	currHP = maxHP;

	endPoint = Cast<AEndPoint>(UGameplayStatics::GetActorOfClass(GetWorld(), AEndPoint::StaticClass()));
	damageIndicator = Cast<UDamageIndicator>(damageIndicatorWidget->GetWidget());

	leftPunchCollision->OnComponentBeginOverlap.AddDynamic(this, &AGolem::OnAttackOverlap);
}

void AGolem::Tick(float DeltaTime)
{ 
	CyphersGameMode->playerWidget->UpdateBossCurrHP(currHP, maxHP);
}

void AGolem::OnAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACypher_Kaya* kaya = Cast<ACypher_Kaya>(OtherActor);
	
	if (kaya != nullptr)
	{ 
		UGameplayStatics::PlaySound2D(GetWorld(), dashAtkSound);
		kaya->ReceiveDamage(2);
		kaya->bCameraShake = true;
		UParticleSystemComponent* PAC = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), dashAttackEffect, kaya->GetMesh()->GetBoneLocation(FName(TEXT("Bip001-Head"))), GetActorRotation());
		PAC->SetWorldScale3D(dashAtkEffectSize);
		leftPunchCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		kaya->compKayaAttack->kayaAnim->KnockBackFlyAnim();
		kaya->bRise = true;
	} 
	
}

void AGolem::MoveJumpAttack() {

	startPos = GetActorLocation();
	originEndPos = mainPlayer->GetActorLocation();
	
	FVector betweenPos = FMath::Lerp(startPos, originEndPos, 0.5);
	betweenPos.Z += betweenHeight;

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

void AGolem::ReceiveDamage(FVector hitLocation)
{
	mainPlayer->PlayGolemDamageSound();

	float basicDamage = 1000;
	float criticalDamage = 1500;

	int32 ranVal = UKismetMathLibrary::RandomIntegerInRange(1, 100);
	if (ranVal < 60) {
		currHP -= basicDamage;
		DamageNumber(basicDamage, hitLocation, false);
	}
	else {
		if (damageIndicator)
		{
			damageIndicator->bCritical = true;
		}

		currHP -= criticalDamage;
		DamageNumber(criticalDamage, hitLocation, true);
	}

	if (currHP < 0)
	{
		FVector orginPos = GetMesh()->GetRelativeLocation();
		FRotator orginRot = GetMesh()->GetRelativeRotation();
		GetMesh()->SetRelativeLocation(FVector(orginPos.X, orginPos.Y, orginPos.Z+90));
		GetMesh()->SetRelativeRotation(FRotator(orginRot.Pitch-5, orginRot.Yaw, orginRot.Roll));
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		leftPunchCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		fsm->anim->PlayDieAnim();
		fsm->bDie = true;
		fsm->mState = EGolemState::Die;
		CyphersGameMode->bFadingOut = true;

		if (endPoint != nullptr) { 
			endPoint->compCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			endPoint->compNiagra->SetVisibility(true);
		}
	}
}