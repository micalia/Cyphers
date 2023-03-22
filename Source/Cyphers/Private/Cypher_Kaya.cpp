// Fill out your copyright notice in the Description page of Project Settings.


#include "Cypher_Kaya.h"
#include <Engine/StaticMesh.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "PlayerMoveInput.h"
#include "Cypher_Kaya_Attack.h"
#include "PlayerAnim.h"
#include <Components/StaticMeshComponent.h>
#include "PlayerCamera.h"
#include <Components/ChildActorComponent.h>
#include <Components/SceneComponent.h>
#include "Enemy_Sentinel.h"
#include <Components/CapsuleComponent.h>
#include "../CyphersGameModeBase.h"
#include "PlayerWidget.h"
#include "Golem.h"
#include "Components/ProgressBar.h"
#include "PowerAttackDecal.h"
#include <Particles/ParticleSystem.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>

ACypher_Kaya::ACypher_Kaya() {
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Models/Unity_Kaya/reCharacter.reCharacter'"));
	if (tempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}
	
	GetMesh()->SetRelativeLocation(FVector(0, 0, -86));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	compSword = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("compSword"));
	compSword->SetupAttachment(GetMesh(), TEXT("Sword"));
	compSword->SetRelativeScale3D(FVector(1.183, 1.133, 1));
	compSword->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempSwordMesh(TEXT("/Script/Engine.StaticMesh'/Game/Resources/Models/NightSkySword/maya2sketchfab.maya2sketchfab'"));
	if (tempSwordMesh.Succeeded()) {
		compSword->SetStaticMesh(tempSwordMesh.Object);
	}

	/*compArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("compArm"));
	compArm->SetupAttachment(RootComponent);
	compArm->SetRelativeLocation(FVector(0,0, 50));*/

	/*compCam = CreateDefaultSubobject<UCameraComponent>(TEXT("compCam"));
	compCam->SetupAttachment(compArm);
	compCam->SetRelativeLocation(FVector(-110,0, 78));*/

	ConstructorHelpers::FClassFinder<UPlayerAnim> tempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/Animation/ABP_Cypher_Kaya.ABP_Cypher_Kaya_C'"));
	if (tempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnim.Class);
	}
//Component Attach
	compPlayerMove = CreateDefaultSubobject<UPlayerMoveInput>(TEXT("compPlayerMove"));
	compKayaAttack = CreateDefaultSubobject<UCypher_Kaya_Attack>(TEXT("compKayaAttack"));
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));

	camTarget = CreateDefaultSubobject<USceneComponent>(TEXT("CameraTarget"));
	camTarget->SetupAttachment(RootComponent);

	ConstructorHelpers::FClassFinder<APlayerCamera> tempPlayerCamera(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/BP_PlayerCamera.BP_PlayerCamera_C'"));
	if (tempPlayerCamera.Succeeded())
	{
		cameraFactory = tempPlayerCamera.Class;
	}
	
	CameraActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("CameraActorComponent"));
	CameraActorComponent->SetupAttachment(camTarget);

	// Set the child actor class to the camera actor class we defined earlier
	CameraActorComponent->SetChildActorClass(cameraFactory);

	// Move the camera actor to the character's head
	
	CameraActorComponent->SetRelativeLocation(CameraOffset);

	powerAttackColl = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PowerAttackCollision"));
	powerAttackColl->SetupAttachment(GetMesh());
	powerAttackColl->SetCollisionProfileName(TEXT("AttackCollision"));
	powerAttackColl->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	powerAttackColl->SetRelativeLocation(FVector(0,72,100));
	powerAttackColl->SetRelativeRotation(FRotator(90,0,0));
	powerAttackColl->SetCapsuleHalfHeight(154);
	powerAttackColl->SetCapsuleRadius(154);
	//powerAttackColl->SetSphereRadius(120);

	static ConstructorHelpers::FObjectFinder<USoundBase> tempPowerAttackStartVoice(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/PowerAttackStartVoice.PowerAttackStartVoice'"));
	if (tempPowerAttackStartVoice.Succeeded()) {
		powerAttackStart = tempPowerAttackStartVoice.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempPowerAttackEndVoice(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/PowerAttackEndVoice.PowerAttackEndVoice'"));
	if (tempPowerAttackEndVoice.Succeeded()) {
		powerAttackEnd = tempPowerAttackEndVoice.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempSwing1(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/swing1.swing1'"));
	if (tempSwing1.Succeeded()) {
		swing1 = tempSwing1.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempSwing2(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/swing2.swing2'"));
	if (tempSwing2.Succeeded()) {
		swing2 = tempSwing2.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempSwing3(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/swing3.swing3'"));
	if (tempSwing3.Succeeded()) {
		swing3 = tempSwing3.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempDamageSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/kayaDamage.kayaDamage'"));
	if (tempDamageSound.Succeeded()) {
		damageSound = tempDamageSound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempDashAttackSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/dashAttack.dashAttack'"));
	if (tempDashAttackSound.Succeeded()) {
		dashAttackSound = tempDashAttackSound.Object;
	}

	static ConstructorHelpers::FClassFinder<APowerAttackDecal> tempDecalObj(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/BP_PowerAttackDecal.BP_PowerAttackDecal_C'"));
	if (tempDecalObj.Succeeded()) {
		decalFactory = tempDecalObj.Class;
	}
	
	footPos = CreateDefaultSubobject<USceneComponent>(TEXT("footPos"));
	footPos->SetupAttachment(RootComponent);
	footPos->SetRelativeLocation(FVector(0,0, -86));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> tempPowerAttackEndEffect(TEXT("/Script/Engine.ParticleSystem'/Game/Resources/Effect/GroundAttacks/Fx/Fire/P_FireGroundAttack6.P_FireGroundAttack6'"));
	if (tempPowerAttackEndEffect.Succeeded()) {
		powerAttackEndEffect = tempPowerAttackEndEffect.Object;
	}
	
	//블루프린트 에디터 창에서 [Mesh]클릭 후 디테일 창에서 tick을 검색한 다음 [Optimization]->[Advanced]->[Visibility Based Anim Tick Option]값을 'Always Tick Pose and Refresh Bones'로 바꿀것

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> tempNiagraSystem(TEXT("/Script/Niagara.NiagaraSystem'/Game/Resources/Effect/NS_AfterImageSystem.NS_AfterImageSystem'"));
	if (tempNiagraSystem.Succeeded()) {
		NiagaraSystemAsset = tempNiagraSystem.Object;
	}

	compNiagra = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AfterImageEffect"));
	compNiagra->SetupAttachment(RootComponent);
	compNiagra->SetAsset(NiagaraSystemAsset);
}

void ACypher_Kaya::BeginPlay()
{
	Super::BeginPlay();

	CyphersGameMode = Cast<ACyphersGameModeBase>(GetWorld()->GetAuthGameMode());
	Camera = Cast<APlayerCamera>(CameraActorComponent->GetChildActor());
	if (Camera)
	{
		Camera->SetAsMainCamera();
	}
	powerAttackColl->OnComponentBeginOverlap.AddDynamic(this, &ACypher_Kaya::OnPowerAttackOverlap);

	maxHP = health;
	currHP = health;

}

void ACypher_Kaya::Tick(float DeltaTime)
{ 
	Super::Tick(DeltaTime);
	if (CyphersGameMode != nullptr) { 
		CyphersGameMode->playerWidget->UpdateCurrHP(currHP, maxHP);
	}
}

void ACypher_Kaya::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	compPlayerMove->SetupInputBinding(PlayerInputComponent);
	compKayaAttack->SetupInputBinding(PlayerInputComponent);
}

void ACypher_Kaya::ReceiveDamage(int32 damage)
{
	if (compKayaAttack->bNotDamageMotion == false) {
		PlayDamageSound();
	}
	//만약 궁극기 차징중이였다면 궁캔슬 되면서 쿨타임
	if (compKayaAttack->bAttackCharge) {
		compKayaAttack->decal->bPowerAttackEnd = true;
		compKayaAttack->bAttackCharge = false;
		compKayaAttack->startCoolKeyE = true;
		compKayaAttack->currkeyECool = compKayaAttack->keyECool;
		CyphersGameMode->playerWidget->KeyECoolTimeBar->SetVisibility(ESlateVisibility::Visible);
	}

	currHP= currHP- damage;
	if(currHP>0){
		if (compKayaAttack->bNotDamageMotion == false) {
			compKayaAttack->kayaAnim->DamagePlayAnim();
		}
		UE_LOG(LogTemp, Warning, TEXT("Damage, currHP : %f"), currHP)
		
	}
	else {
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		compKayaAttack->kayaAnim->DiePlayAnim();
		currHP = 0;
		UE_LOG(LogTemp, Warning, TEXT("Player DIe"))
	}
}

void ACypher_Kaya::DetachCameraActor()
{
	//FVector CameraWorldPos = Camera->GetActorLocation();
	Camera->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	//CameraActorComponent->DetachFromParent(true);
	//Camera->SetActorLocation(CameraWorldPos);
}

void ACypher_Kaya::AttachCameraActor()
{
	//CameraActorComponent->AttachToComponent(CameraActorComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	Camera->AttachToComponent(CameraActorComponent, FAttachmentTransformRules::KeepWorldTransform);
	//Camera->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform, TEXT("CameraTarget"));
	//Camera->SetActorRelativeLocation(CameraOffset);
}

void ACypher_Kaya::OnPowerAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DrawDebugSphere(GetWorld(), GetActorLocation(), 20.0f, 32, FColor::Red, false, 5.0f);

	//공격 대상마다 체크하는 로직 수정 필요
	UE_LOG(LogTemp, Warning, TEXT("enemy HIt!!!"))
	AEnemy_Sentinel* sentinel = Cast<AEnemy_Sentinel>(OtherActor);
	if (sentinel != nullptr) {
		sentinel->ReceiveDamage();
	}

	AGolem* golem = Cast<AGolem>(OtherActor);
	if (golem != nullptr)
	{
		golem->ReceiveDamage();
	}
	
}

void ACypher_Kaya::PlayDashAttackSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), dashAttackSound);
}

void ACypher_Kaya::PlayDamageSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), damageSound);
}
