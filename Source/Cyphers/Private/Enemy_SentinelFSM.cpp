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

	//���� �ʱ� ü���� ��������
	currHP = maxHP;

	//���� �ʱ� ��ġ�� ��������
	originPos = me->GetActorLocation();

}


// Called every frame
void UEnemy_SentinelFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
	case EEnemy_SentinelState::ReturnPos:
		UpdateReturnPos();
		break;
	}
}

void UEnemy_SentinelFSM::UpdateIdle()
{
return; // �ϴ� ���ʹ� ������ ����


	//���࿡ �÷��̾ �Ѿ� �� �� �ִ�?
	if (IsTargetTrace())
	{
		//���¸� Move �� ��ȯ
		ChangeState(EEnemy_SentinelState::Move);
	}
	else
	{
		//idleDelayTime �� ������	
		if (IsWaitComplete(idleDelayTime))
		{
			//������¸� Move �� �Ѵ�.
			ChangeState(EEnemy_SentinelState::Move);
		}
	}
}

void UEnemy_SentinelFSM::UpdateMove()
{
	// �þ߿� ���Դ��� ����
	bool bTrace = IsTargetTrace();
	
	//1. Ÿ���� ���ϴ� ������ ���ϰ�(target - me)
	FVector dir = target->GetActorLocation() - me->GetActorLocation();

	//ó�� ��ġ, ���� ���� ��ġ�� �Ÿ�
	float dist = FVector::Distance(originPos, me->GetActorLocation());

	//���࿡ dist �� moveRange ���� Ŀ���� (������ �� �ִ� �ݰ��� �Ѿ�ٸ�)
	if (dist > moveRange)
	{
		//���¸� ReturnPos �� ����
		ChangeState(EEnemy_SentinelState::ReturnPos);
	}
	//���࿡ �þ߿� ���Դٸ�
	else if (bTrace)
	{
		//���࿡ target - me �Ÿ��� ���ݹ������� ������
		if (dir.Length() < attackRange)
		{
			//���¸� Attack ���� ����
			ChangeState(EEnemy_SentinelState::Attack);
		}
		//�׷��� ������
		else
		{
			//2. �� �������� �̵��ϰ� �ʹ�.
			//me->AddMovementInput(dir.GetSafeNormal());
			//ai �� �̿��ؼ� ���������� �̵��ϰ� �ʹ�.	
			ai->MoveToLocation(target->GetActorLocation());
		}
	}
	//�þ߿� ������ �ʾҴٸ�
	else
	{
		// ������ ��ġ���� ������ �� Idle ���·� ��ȯ
		MoveToPos(randPos);
	}

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
	if (p.Z < -200)
	{
		//me->Destroy();

		//���� ��Ȱ��ȭ
		me->SetActive(false);
		////EnemyManager ã��
		//AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyManager::StaticClass());
		//AEnemyManager* am = Cast<AEnemyManager>(actor);
		////ã�� �𿡼� enemyArray �� ���� �ٽ� ����
		//am->enemyArray.Add(me);
		//currHP �� maxHP
		currHP = maxHP;
		//���¸� Idle
		ChangeState(EEnemy_SentinelState::Idle);
		//��Ÿ�ָ� �����ش�
		//me->StopAnimMontage(damageMontage);
		//bDieMove �� false ��!
		bDieMove = false;
	}
	//3. �׷��� ������ �ش� ��ġ�� �����Ѵ�
	else
	{
		me->SetActorLocation(p);
	}
}

void UEnemy_SentinelFSM::UpdateReturnPos()
{
	// ó�� ��ġ�� ���� �����ϸ� Idle ���·� ��ȯ�Ѵ�.
	MoveToPos(originPos);


	////1. �� ----> ó����ġ�� ���ϴ� ������ ���Ѵ�.
	//FVector dir = originPos - me->GetActorLocation();

	////2. ���࿡ �� --- ó����ġ�� �Ÿ��� 10���� ������
	//if (dir.Length() < 10)
	//{
	//	//3. Idle ���·� ��ȯ
	//	ChangeState(EEnemy_SentinelState::Idle);
	//}
	////4. �׷��� ������ 
	//else
	//{
	//	//5. ��� 1������ ���� �������� �̵��Ѵ�
	//	me->AddMovementInput(dir.GetSafeNormal());
	//}
}

void UEnemy_SentinelFSM::ChangeState(EEnemy_SentinelState state)
{
	//���� ���� �α׸� �������
	/*UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemy_SentinelState"), true);
	if (enumPtr != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s -----> %s"),
			*enumPtr->GetNameStringByIndex((int32)currState),
			*enumPtr->GetNameStringByIndex((int32)state));
	}*/

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
	{
		//�׺���̼� �ý��� ��������
		UNavigationSystemV1* ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		//������ ��ġ�� �����
		FNavLocation loc;
		ns->GetRandomReachablePointInRadius(originPos, 1000, loc);
		randPos = loc.Location;
	}
	break;
	case EEnemy_SentinelState::Damaged:
	{
		//1. ������ ���� �̴´� (0, 1 ��)
		int32 rand = FMath::RandRange(0, 1);
		//2. Damage0, Damage1 �̶� ���ڿ��� �����.
		FString sectionName = FString::Printf(TEXT("Damage%d"), rand);
		//3. ��Ÿ�ָ� �÷����Ѵ�.
		//me->PlayAnimMontage(damageMontage, 1.0f, FName(*sectionName));
	}
	break; 
	case EEnemy_SentinelState::Die:
		//�浹�ȵǰ� ����
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Die ��Ÿ�� ����
		//me->PlayAnimMontage(damageMontage, 1.0f, FName(TEXT("Die")));
		break;
	}
}

void UEnemy_SentinelFSM::ReceiveDamage()
{
	//�Ǹ� ������
	currHP--;
	//hp �� 0���� ũ�� Damage ���·� ��ȯ
	if (currHP > 0)
	{
		ChangeState(EEnemy_SentinelState::Damaged);
	}
	//�׷��� ������ Die ���·� ��ȯ
	else
	{

		ChangeState(EEnemy_SentinelState::Die);
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

void UEnemy_SentinelFSM::MoveToPos(FVector pos)
{
	//�ش� ��ġ(pos) �� ����
	EPathFollowingRequestResult::Type result = ai->MoveToLocation(pos);

	//���࿡ �������� �����ߴٸ�
	if (result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		//Idle ���·� ��ȯ
		ChangeState(EEnemy_SentinelState::Idle);
	}
}
