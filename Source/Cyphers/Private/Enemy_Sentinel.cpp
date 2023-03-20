// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Sentinel.h"
#include "Enemy_SentinelAnim.h"
#include <Engine/SkeletalMesh.h>
#include <Components/CapsuleComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <UMG/Public/Components/WidgetComponent.h>
#include "Enemy_SentinelHpUI.h"
#include <Kismet/KismetMathLibrary.h>
#include <Camera/CameraComponent.h>
#include "Cypher_Kaya.h"
#include "PlayerCamera.h"
#include <Components/BoxComponent.h>

// Sets default values
AEnemy_Sentinel::AEnemy_Sentinel()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//Mesh 의 위치를 셋팅
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	//Mesh 의 회전을 셋팅
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	//Mesh 외형 셋팅
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Animations/EnemyRetarget/SM_Sentinel.SM_Sentinel'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}
	//Anim Instance class 셋팅
	ConstructorHelpers::FClassFinder<UEnemy_SentinelAnim> tempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/Animation/ABP_Enemy_Sentinel.ABP_Enemy_Sentinel_C'"));
	if (tempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnim.Class);
	}

	//FSM 컴포넌트 추가
	fsm = CreateDefaultSubobject<UEnemy_SentinelFSM>(TEXT("FSM"));
	
	//LineTrace 에 김자가 되게 셋팅	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Auto possess ai 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	ConstructorHelpers::FClassFinder<UEnemy_SentinelHpUI> tempHpUi(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/Widget/WB_Enemy_SentinelHpUI.WB_Enemy_SentinelHpUI_C'"));
	if (tempHpUi.Succeeded()) {
		sentinelUIfactory = tempHpUi.Class;
	}
	hpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	hpWidget->SetWidgetClass(sentinelUIfactory);
	hpWidget->SetupAttachment(GetMesh());
	hpWidget->SetRelativeLocation(FVector(0,0, 225));
	hpWidget->SetRelativeRotation(FRotator(0,90, 0));
	hpWidget->SetRelativeScale3D(FVector(0.399762, 0.399762, 0.329024));

	attackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollision"));
	attackCollision->SetupAttachment(RootComponent);
	attackCollision->SetCollisionProfileName(TEXT("AttackCollision"));
	attackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	attackCollision->SetRelativeLocation(FVector(116,0,0));
	attackCollision->SetBoxExtent(FVector(76,58,86));
}

// Called when the game starts or when spawned
void AEnemy_Sentinel::BeginPlay()
{
	Super::BeginPlay();

	//움직이는 방향으로 몸을 자동으로 회전하라는 옵션
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//카메라
	kaya = Cast<ACypher_Kaya>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	GetWorldTimerManager().SetTimer(TimerHandle_UpdateWidgetRotation, this, &AEnemy_Sentinel::UpdateWidgetRotation, 0.1f, true);

	sentinelHpUI = Cast<UEnemy_SentinelHpUI>(hpWidget->GetWidget());
	maxHP = health;
	currHP = health;

	attackCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy_Sentinel::OnAttackOverlap);
}

// Called every frame
void AEnemy_Sentinel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (sentinelHpUI != nullptr) {
		sentinelHpUI->UpdateCurrHP(currHP, maxHP);

	}
}

// Called to bind functionality to input
void AEnemy_Sentinel::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy_Sentinel::ReceiveDamage()
{
	currHP--;
	if (currHP > 0)
	{
		fsm->ChangeState(EEnemy_SentinelState::Damaged);
	}
	//그렇지 않으면 Die 상태로 전환
	else
	{

		fsm->ChangeState(EEnemy_SentinelState::Die);
	}
}

void AEnemy_Sentinel::SetActive(bool bActive)
{
	// 활성화
	if (bActive)
	{
		//충돌 활성
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//생성 위치 재설정
		fsm->originPos = GetActorLocation();
	}
	// 비활성화
	else
	{
		//충돌 비활성
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//탄창에 날 다시 넣어줘
		dieDelegate.ExecuteIfBound(this);
	}

	//메쉬를 활성 / 비활성
	GetMesh()->SetActive(bActive);
	//메쉬를 보이고 / 안보이고
	GetMesh()->SetVisibility(bActive);
	//캐릭터 무브먼트 활성 / 비활성
	GetCharacterMovement()->SetActive(bActive);
	//fsm 활성 / 비활성
	fsm->SetActive(bActive);
}

void AEnemy_Sentinel::OnAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (kaya != nullptr) {
		kaya->ReceiveDamage(2);
	}
}

void AEnemy_Sentinel::UpdateWidgetRotation()
{
	if (hpWidget)
	{
		CameraLocation = GetPlayerCameraLocation();
		CameraRotation = GetPlayerCameraRotation();

		WidgetRotation = UKismetMathLibrary::FindLookAtRotation(hpWidget->GetComponentLocation(), CameraLocation);
		hpWidget->SetWorldRotation(WidgetRotation);
	}
}

FVector AEnemy_Sentinel::GetPlayerCameraLocation()
{
	if (kaya)
	{ 
		return kaya->Camera->GetActorLocation();
	}

	return FVector::ZeroVector;
}

FRotator AEnemy_Sentinel::GetPlayerCameraRotation()
{
	if (kaya)
	{
		return kaya->Camera->GetActorRotation();
	}

	return FRotator::ZeroRotator;
}