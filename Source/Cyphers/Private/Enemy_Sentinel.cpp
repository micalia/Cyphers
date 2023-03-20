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


	//Mesh �� ��ġ�� ����
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	//Mesh �� ȸ���� ����
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	//Mesh ���� ����
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Animations/EnemyRetarget/SM_Sentinel.SM_Sentinel'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}
	//Anim Instance class ����
	ConstructorHelpers::FClassFinder<UEnemy_SentinelAnim> tempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/Animation/ABP_Enemy_Sentinel.ABP_Enemy_Sentinel_C'"));
	if (tempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnim.Class);
	}

	//FSM ������Ʈ �߰�
	fsm = CreateDefaultSubobject<UEnemy_SentinelFSM>(TEXT("FSM"));
	
	//LineTrace �� ���ڰ� �ǰ� ����	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Auto possess ai ����
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

	//�����̴� �������� ���� �ڵ����� ȸ���϶�� �ɼ�
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//ī�޶�
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
	//�׷��� ������ Die ���·� ��ȯ
	else
	{

		fsm->ChangeState(EEnemy_SentinelState::Die);
	}
}

void AEnemy_Sentinel::SetActive(bool bActive)
{
	// Ȱ��ȭ
	if (bActive)
	{
		//�浹 Ȱ��
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//���� ��ġ �缳��
		fsm->originPos = GetActorLocation();
	}
	// ��Ȱ��ȭ
	else
	{
		//�浹 ��Ȱ��
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//źâ�� �� �ٽ� �־���
		dieDelegate.ExecuteIfBound(this);
	}

	//�޽��� Ȱ�� / ��Ȱ��
	GetMesh()->SetActive(bActive);
	//�޽��� ���̰� / �Ⱥ��̰�
	GetMesh()->SetVisibility(bActive);
	//ĳ���� �����Ʈ Ȱ�� / ��Ȱ��
	GetCharacterMovement()->SetActive(bActive);
	//fsm Ȱ�� / ��Ȱ��
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