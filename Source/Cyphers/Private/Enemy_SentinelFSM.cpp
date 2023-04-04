// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_SentinelFSM.h"
#include <Animation/AnimMontage.h>
#include <Kismet/GameplayStatics.h>
#include <AIModule/Classes/AIController.h>
#include <NavigationSystem/Public/NavigationSystem.h>
#include <Kismet/KismetMathLibrary.h>
#include "Cypher_Kaya.h"
#include "Enemy_SentinelAnim.h"
#include "Enemy_Sentinel.h"
#include <Components/CapsuleComponent.h>
#include <UMG/Public/Components/WidgetComponent.h>
#include "BlockStone.h"
#include "../CyphersGameModeBase.h"

// Sets default values for this component's properties
UEnemy_SentinelFSM::UEnemy_SentinelFSM()
{
	//SetActive (true / false) �۵� �ǰ� ����!
	bAutoActivate = true;
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	//��Ÿ�� �ҷ�����
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempMontage(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Animations/Mongtage/AM_Enemy_Sentinel/AM_Enemy_SentinelDamage.AM_Enemy_SentinelDamage'"));
	if (tempMontage.Succeeded())
	{
		damageMontage = tempMontage.Object;
	}
}


// Called when the game starts
void UEnemy_SentinelFSM::BeginPlay()
{
	Super::BeginPlay();

	//Ÿ�� ã��
	target = Cast<ACypher_Kaya>(UGameplayStatics::GetActorOfClass(GetWorld(), ACypher_Kaya::StaticClass()));
	//���� ã��
	me = Cast<AEnemy_Sentinel>(GetOwner());
	
	//anim instance ã��
	/*USkeletalMeshComponent* mesh = me->GetMesh();
	UAnimInstance animInstance = mesh->GetAnimInstance();
	anim = Cast<UEnemyAnim>(animInstance);*/
	anim = Cast<UEnemy_SentinelAnim>(me->GetMesh()->GetAnimInstance());

	//ai controller ã��
	ai = Cast<AAIController>(me->GetController());
	//ai = UAIBlueprintHelperLibrary::GetAIController(me);

	disappearZheight = me->GetActorLocation().Z -200;
}


// Called every frame
void UEnemy_SentinelFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (target != nullptr) {
		distWithPlayer = target->GetActorLocation() - me->GetActorLocation();
	}

	switch (currState)
	{
	case EEnemy_SentinelState::Idle:
		UpdateIdle();
		break;
	case EEnemy_SentinelState::Move:
		UpdateMove();
		break;
	case EEnemy_SentinelState::Attack:
		UpdateAttack();
		break;
	case EEnemy_SentinelState::AttackDelay:
		UpdaetAttackDelay();
		break;
	case EEnemy_SentinelState::Damaged:
		UpdateDamaged();
		break;
	case EEnemy_SentinelState::Die:
		UpdateDie();
		break;
	case EEnemy_SentinelState::LieFloor:
		UpdateLieFloor();
		break;
	}
}

void UEnemy_SentinelFSM::UpdateIdle()
{
	if(distWithPlayer.Length() < detectRange) bDetectPlayer = true;

	//���࿡ �÷��̾ �Ѿ� �� �� �ִ�?
	//if (IsTargetTrace())
	//{
	//	//���¸� Move �� ��ȯ
	//	ChangeState(EEnemy_SentinelState::Move);
	//}
	//else
	//{
		//idleDelayTime �� ������	
		if (IsWaitComplete(idleDelayTime))
		{
			if(bDetectPlayer == false) return;
			//������¸� Move �� �Ѵ�.
			ChangeState(EEnemy_SentinelState::Move);
		}
    //	}
}

void UEnemy_SentinelFSM::UpdateMove()
{
	// �þ߿� ���Դ��� ����
	bool bTrace = IsTargetTrace();
	
	//1. Ÿ���� ���ϴ� ������ ���ϰ�(target - me)
	FVector dir = target->GetActorLocation() - me->GetActorLocation();

	////ó�� ��ġ, ���� ���� ��ġ�� �Ÿ�
	//float dist = FVector::Distance(originPos, me->GetActorLocation());

	////���࿡ dist �� moveRange ���� Ŀ���� (������ �� �ִ� �ݰ��� �Ѿ�ٸ�)
	//if (dist > moveRange)
	//{
	//	//���¸� ReturnPos �� ����
	//	ChangeState(EEnemy_SentinelState::ReturnPos);
	//}
	////���࿡ �þ߿� ���Դٸ�
	//else if (bTrace)
	//{
	
		//���࿡ target - me �Ÿ��� ���ݹ������� ������
		if (dir.Length() < attackRange)
		{
			//���¸� Attack ���� ����
			ChangeState(EEnemy_SentinelState::Attack);
		}
		//�׷��� ������
		else
		{
			//2. �� ������S�� �̵��ϰ� �ʹ�.
			//////me->AddMovementInput(dir.GetSafeNormal());
			//ai �� �̿��ؼ� ���������� �̵��ϰ� �ʹ�.	
			EPathFollowingRequestResult::Type re = ai->MoveToActor(target);
		}
	//}
	////�þ߿� ������ �ʾҴٸ�
	//else
	//{
	//	// ������ ��ġ���� ������ �� Idle ���·� ��ȯ
	//	MoveToPos(randPos);
	//}

}

void UEnemy_SentinelFSM::UpdateAttack()
{
	//2. ���¸� AttackDelay �� ��ȯ
	ChangeState(EEnemy_SentinelState::AttackDelay);
}

void UEnemy_SentinelFSM::UpdaetAttackDelay()
{
	//2. ���࿡ ����ð��� attackDelayTime ���� Ŀ����
	if (IsWaitComplete(attackDelayTime))
	{
		//3. target �� me �Ÿ��� ������.
		FVector dir = target->GetActorLocation() - me->GetActorLocation();
		float dist = dir.Length();
		//4. ���࿡ �װŸ��� attackRange���� ������
		if (dist < attackRange)
		{
			//5. Attack ���·� ��ȯ
			ChangeState(EEnemy_SentinelState::Attack);
		}
		else
		{
			//6. �׷��� ������ Idle ���·� ����
			ChangeState(EEnemy_SentinelState::Idle);
		}
	}
}

void UEnemy_SentinelFSM::UpdateDamaged()
{
	//damageDelayTime �� ������
	if (IsWaitComplete(damageDelayTime))
	{
		//Move ����
		ChangeState(EEnemy_SentinelState::Idle);
	}
}

void UEnemy_SentinelFSM::UpdateDie()
{
	//���࿡ bDieMove �� false ��� �Լ��� ������
	if (bDieMove == false) return;

	//P = P0 + vt
	//1. �Ʒ��� �������� ��ġ�� ���Ѵ�.
	FVector p0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector p = p0 + vt;


	//2. ���࿡ p.Z �� -200 ���� ������ �ı��Ѵ�
	if (p.Z < disappearZheight)
	{
		ABlockStone* blockStone = Cast<ABlockStone>(UGameplayStatics::GetActorOfClass(GetWorld(), ABlockStone::StaticClass()));
		blockStone->bNextStage = true;
		
		ACyphersGameModeBase* CyphersGameMode = Cast<ACyphersGameModeBase>(GetWorld()->GetAuthGameMode());
		CyphersGameMode->bCameraShake = true;
		
		UGameplayStatics::PlaySound2D(GetWorld(), CyphersGameMode->earthquakeSound);

		me->Destroy();

		bDieMove = false;
	}
	//3. �׷��� ������ �ش� ��ġ�� �����Ѵ�
	else
	{
		me->SetActorLocation(p);
	}
}



void UEnemy_SentinelFSM::UpdateLieFloor()
{
	if (IsWaitComplete(damageDelayTime))
	{
		if (me->currHP <= 0) {
			me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			me->hpWidget->SetVisibility(false);
			currState = EEnemy_SentinelState::Die;
			bDieMove = true;
		}
		else {
			//Move ����
			ChangeState(EEnemy_SentinelState::Rise);
		}
	}
}

void UEnemy_SentinelFSM::ChangeState(EEnemy_SentinelState state)
{
	//���� ���� �α׸� �������
	UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemy_SentinelState"), true);
	if (enumPtr != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s -----> %s"),
			*enumPtr->GetNameStringByIndex((int32)currState),
			*enumPtr->GetNameStringByIndex((int32)state));
	}

	//���� ���¸� ����
	currState = state;

	//anim �� ���� ����
	anim->state = state;

	//���� �ð� �ʱ�ȭ
	currTime = 0;

	//ai �� ������ ������
	ai->StopMovement();

	//���¿� ���� �ʱ⼳��
	switch (currState)
	{
	case EEnemy_SentinelState::Attack:
		//currTime = attackDelayTime;
		break;
	case EEnemy_SentinelState::Move:
	
		break;
	case EEnemy_SentinelState::Damaged:
	{
		//1. ������ ���� �̴´� (0, 1 ��)
		int32 rand = FMath::RandRange(0, 1);
		//2. Damage0, Damage1 �̶� ���ڿ��� �����.
		FString sectionName = FString::Printf(TEXT("Damage%d"), rand);
		//3. ��Ÿ�ָ� �÷����Ѵ�.
		me->PlayAnimMontage(damageMontage, 1.0f, FName(*sectionName));
	}
	break; 
	case EEnemy_SentinelState::Die:
		//�浹�ȵǰ� ����
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		me->hpWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		me->hpWidget->SetVisibility(false);
		//Die ��Ÿ�� ����
		me->PlayAnimMontage(damageMontage, 1.0f, FName(TEXT("Die")));
		break;
	case EEnemy_SentinelState::LieFloor:
		break;
	case EEnemy_SentinelState::Rise:
		me->PlayAnimMontage(damageMontage, 1.0f, TEXT("Rise"));
		break;
	}
}

bool UEnemy_SentinelFSM::IsWaitComplete(float delayTime)
{
	//1. �ð��� �帣�� �Ѵ�.
	currTime += GetWorld()->DeltaTimeSeconds;

	//2. ���࿡ ����ð��� delayTime���� Ŀ���� 
	if (currTime > delayTime)
	{
		//3. ����ð��� 0���� �ʱ�ȭ
		currTime = 0;
		//4. true ��ȯ
		return true;
	}

	//5. �׷��� ������ false �� ��ȯ
	return false;
}


bool UEnemy_SentinelFSM::IsTargetTrace()
{
	//1. �� ----> �÷��̾ ���ϴ� ����
	FVector dir = target->GetActorLocation() - me->GetActorLocation();

	//2. ���� �չ���� 1���� ���� ������ ����
	float dotValue = FVector::DotProduct(me->GetActorForwardVector(), dir.GetSafeNormal());

	//3. 2���� ���� ���� Acos --> �� ������ ���̰�
	float angle = UKismetMathLibrary::DegAcos(dotValue);

	//4. ���࿡ 3������ ���� ���� 30���� �۰�(�þ߰� 60)
	//   �� - Ÿ�� ���� �Ÿ��� traceRange ���� ������
	if (angle < 30 && dir.Length() < traceRange)
	{
		//Enemy -----> target LineTrace ����!!
		FHitResult hitInfo;
		FCollisionQueryParams param;
		param.AddIgnoredActor(me);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			hitInfo,
			me->GetActorLocation(),
			target->GetActorLocation(),
			ECollisionChannel::ECC_Visibility,
			param);

		//���࿡ �ε��� ���� �ִٸ�
		if (bHit)
		{
			//���࿡ �ε��� ���� �̸��� Player �� �����ϰ� �ִٸ�
			if (hitInfo.GetActor()->GetName().Contains(TEXT("Player")))
			{
				//5. true ��ȯ
				return true;
			}
		}
	}

	//6. �׷��� ������ false ��ȯ	
	return false;

}
