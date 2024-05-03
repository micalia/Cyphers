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
#include <Kismet/KismetMathLibrary.h>
#include "WhiteScreen.h"
#include <UMG/Public/Components/WidgetComponent.h>
#include "UMG/Public/Components/Overlay.h"

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
	compSword->SetRelativeScale3D(FVector(1, 1.34052, 1.3));
	compSword->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempSwordMesh(TEXT("/Script/Engine.StaticMesh'/Game/Resources/Models/NightSkySword/maya2sketchfab.maya2sketchfab'"));
	if (tempSwordMesh.Succeeded()) {
		compSword->SetStaticMesh(tempSwordMesh.Object);
	}

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
	cameraOriginPos = CameraOffset;

	powerAttackColl = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PowerAttackCollision"));
	powerAttackColl->SetupAttachment(GetMesh());
	powerAttackColl->SetCollisionProfileName(TEXT("AttackCollision"));
	powerAttackColl->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	powerAttackColl->SetRelativeLocation(FVector(0,72,100));
	powerAttackColl->SetRelativeRotation(FRotator(90,0,0));
	powerAttackColl->SetCapsuleHalfHeight(154);
	powerAttackColl->SetCapsuleRadius(154);

	static ConstructorHelpers::FObjectFinder<USoundBase> tempPowerAttackStartVoice(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/PowerAttackStartVoice.PowerAttackStartVoice'"));
	if (tempPowerAttackStartVoice.Succeeded()) {
		powerAttackStart = tempPowerAttackStartVoice.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempPowerAttackEndVoice(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/PowerAttackEndVoice.PowerAttackEndVoice'"));
	if (tempPowerAttackEndVoice.Succeeded()) {
		powerAttackEnd = tempPowerAttackEndVoice.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempPlayerAppearSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/PlayerAppearSound.PlayerAppearSound'"));
	if (tempPlayerAppearSound.Succeeded()) {
		playerAppearSound = tempPlayerAppearSound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempRightFootSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/rightFootStep.rightFootStep'"));
	if (tempRightFootSound.Succeeded()) {
		rightFootSound = tempRightFootSound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempLeftFootSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/leftFootStep.leftFootStep'"));
	if (tempLeftFootSound.Succeeded()) {
		leftFootSound = tempLeftFootSound.Object;
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
	static ConstructorHelpers::FObjectFinder<USoundBase> tempGaUpAtkSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/GA_UpAtk.GA_UpAtk'"));
	if (tempGaUpAtkSound.Succeeded()) {
		gaUpAtkSound = tempGaUpAtkSound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempGaDownAtkSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/GA_DownAtk.GA_DownAtk'"));
	if (tempGaDownAtkSound.Succeeded()) {
		gaDownAtkSound = tempGaDownAtkSound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempDamageSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/kayaDamage.kayaDamage'"));
	if (tempDamageSound.Succeeded()) {
		damageSound = tempDamageSound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempDashAttackSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/dashAttack.dashAttack'"));
	if (tempDashAttackSound.Succeeded()) {
		dashAttackSound = tempDashAttackSound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempDashSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/DashSound.DashSound'"));
	if (tempDashSound.Succeeded()) {
		dashSound = tempDashSound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempGA1_Sound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/GA_1_Sound.GA_1_Sound'"));
	if (tempGA1_Sound.Succeeded()) {
		GA1_Sound = tempGA1_Sound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempGA2_Sound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/GA_2_Sound.GA_2_Sound'"));
	if (tempGA2_Sound.Succeeded()) {
		GA2_Sound = tempGA2_Sound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempGolemDamage(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/GolemDamage.GolemDamage'"));
	if (tempGolemDamage.Succeeded()) {
		golemDamage = tempGolemDamage.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempPowerAttackComboSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/PowerAttackComboSound.PowerAttackComboSound'"));
	if (tempPowerAttackComboSound.Succeeded()) {
		powerAttackComboSound = tempPowerAttackComboSound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempPowerAttackEndGroundSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/powerAttackSound.powerAttackSound'"));
	if (tempPowerAttackEndGroundSound.Succeeded()) {
		powerAttackEndGroundSound = tempPowerAttackEndGroundSound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> tempPowerAttackSwordReday(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/PowerAttackSwordReady.PowerAttackSwordReady'"));
	if (tempPowerAttackSwordReday.Succeeded()) {
		powerAttackSwordReday = tempPowerAttackSwordReday.Object;
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

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> tempNiagraSystem(TEXT("/Script/Niagara.NiagaraSystem'/Game/Resources/Effect/NS_AfterImageSystem.NS_AfterImageSystem'"));
	if (tempNiagraSystem.Succeeded()) {
		NiagaraSystemAsset = tempNiagraSystem.Object;
	}

	compNiagra = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AfterImageEffect"));
	compNiagra->SetupAttachment(RootComponent);
	compNiagra->SetAsset(NiagaraSystemAsset);

	//등장 연출

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> tempDisolveNiagaraAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/Resources/Effect/DissolveSpawn/VFX/NS_Morph2.NS_Morph2'"));
	if (tempDisolveNiagaraAsset.Succeeded()) {
		disolveNiagaraAsset = tempDisolveNiagaraAsset.Object;
	}

	disolveNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DisolveNiagara"));
	disolveNiagara->SetupAttachment(GetMesh());
	disolveNiagara->SetAsset(disolveNiagaraAsset);

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
	
	if(bRise)RiseCheck();
	if(bPlayerAppear)PlayerAppear();
	if (bCameraShake == true)CameraShakeRandom();
	
	currtimer+=DeltaTime;
	if (currtimer>timer) {
		float a=0;
	}
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

void ACypher_Kaya::PlayerAppear()
{
	currAppearTime += GetWorld()->GetDeltaSeconds();
	if (currAppearTime < AppearTime) {
	float alpha = currAppearTime / AppearTime;
	GetMesh()->SetScalarParameterValueOnMaterials(TEXT("Amount"), alpha);
	}
	else {
		currAppearTime = 0;
		bPlayerAppear = false;
		disolveNiagara->SetActive(false);
	}
}

void ACypher_Kaya::ReceiveDamage(int32 damage)
{
	if (compKayaAttack->bNotDamageMotion == false) {
		PlayDamageSound();
	}
#pragma region If Using SuperSkill Keyboard(E)
	if (compKayaAttack->bAttackCharge) {
		Camera->bSkillReady = false;
		compKayaAttack->decal->bPowerAttackEnd = true;
		compKayaAttack->bAttackCharge = false;
		compKayaAttack->startCoolKeyE = true;
		compKayaAttack->currkeyECool = compKayaAttack->keyECool;
		compKayaAttack->currPowerAttackCheck = 0;
	}
	if (CyphersGameMode) { 
		CyphersGameMode->playerWidget->KeyECoolTimeBar->SetVisibility(ESlateVisibility::Visible);
		CyphersGameMode->playerWidget->OverlayKeyEPressing->SetRenderOpacity(0);
	}
#pragma endregion
#pragma region If Using Dash Skill
	if (compKayaAttack->bDashOn) {
		compKayaAttack->bDashOn = false;
		compKayaAttack->DashEndComboState();
		compKayaAttack->startCoolSpaceBar = true;
		if (CyphersGameMode) {
			CyphersGameMode->playerWidget->SpaceBarCoolTimerBar->SetVisibility(ESlateVisibility::Visible);
		}
	}
#pragma endregion
#pragma region If Using BasicAttack
	compKayaAttack->IsComboInputOn = false;
	compKayaAttack->AttackEndComboState();
	compKayaAttack->InitInput();
#pragma endregion
#pragma region If Using GripAttack	
	compKayaAttack->gripMoveCurrTime = 0;
	compKayaAttack->gripIndex = 0;
	compKayaAttack->bIsGripAttacking = false;
	compKayaAttack->GAMovePoints.Empty();
#pragma endregion

	currHP= currHP- damage;
	if(currHP>0){
		if (compKayaAttack->bNotDamageMotion == false) {
			compKayaAttack->kayaAnim->DamagePlayAnim();
		}
	}
	else {
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		compKayaAttack->kayaAnim->AttachCamera();
		compKayaAttack->kayaAnim->DiePlayAnim();
		currHP = 0;
		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		DisableInput(controller); 
		ACyphersGameModeBase* gamemode = Cast<ACyphersGameModeBase>(GetWorld()->GetAuthGameMode());
		gamemode->whiteScreen->SetVisibility(ESlateVisibility::Visible);
		gamemode->OpacityOnCheck = true;
		gamemode->ActiveWhiteScreenOpacity = true;
	}

	*compKayaAttack->bUsingSkill = false;
	bDamageState = true;
	compKayaAttack->bUsingSkill = &bDamageState;
}

void ACypher_Kaya::DetachCameraActor()
{
	Camera->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void ACypher_Kaya::AttachCameraActor()
{
	Camera->AttachToComponent(CameraActorComponent, FAttachmentTransformRules::KeepWorldTransform);
}

void ACypher_Kaya::OnPowerAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//공격 대상마다 체크하는 로직 수정 필요
	AEnemy_Sentinel* sentinel = Cast<AEnemy_Sentinel>(OtherActor);
	if (sentinel != nullptr) {
		sentinel->ReceiveDamage();
	}

	AGolem* golem = Cast<AGolem>(OtherActor);
	if (golem != nullptr)
	{
		golem->ReceiveDamage(GetActorLocation());
	}
}

void ACypher_Kaya::RiseCheck()
{
	riseCurrTime += GetWorld()->GetDeltaSeconds();
	if (riseCurrTime > riseTime) {
		bRise = false;
		compKayaAttack->kayaAnim->RiseAnim();
		riseCurrTime = 0;
	}
}

void ACypher_Kaya::PlayPowerAttackSwordReadySound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), powerAttackSwordReday);
}

void ACypher_Kaya::PlayPowerAttackGroundSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), powerAttackEndGroundSound);
}

void ACypher_Kaya::PlayDashSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), dashSound);
}

void ACypher_Kaya::PlayGaUpAtkSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), gaUpAtkSound);
}

void ACypher_Kaya::PlayGaDownAtkSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), gaDownAtkSound);
}

void ACypher_Kaya::PlayGolemDamageSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), golemDamage);
}

void ACypher_Kaya::PlayDashAttackSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), dashAttackSound);
}

void ACypher_Kaya::PlayDamageSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), damageSound);
}

void ACypher_Kaya::CameraShakeRandom()
{
	if (camCurrTime < cameraShakeTime) {
		camCurrTime += GetWorld()->GetDeltaSeconds();
		float y = UKismetMathLibrary::RandomFloatInRange(-6, 6);
		float z = UKismetMathLibrary::RandomFloatInRange(-6, 6);
		CameraActorComponent->SetRelativeLocation(cameraOriginPos + FVector(0, y, z));
	}
	else {
		bCameraShake = false;
		CameraActorComponent->SetRelativeLocation(cameraOriginPos);
		camCurrTime = 0;
	}
}