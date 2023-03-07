#include "PlayerAnim.h"
#include "Cypher_Kaya.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Cypher_Kaya_Attack.h"

UPlayerAnim::UPlayerAnim()
{
	ConstructorHelpers::FObjectFinder<UAnimMontage> tempBasicAttackMontage(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Animations/Mongtage/AM_KayaBasicAttack.AM_KayaBasicAttack'"));
	if (tempBasicAttackMontage.Succeeded())
	{
		basicAttackAnimMontage = tempBasicAttackMontage.Object;
	}
}

void UPlayerAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	APawn* pawn = TryGetPawnOwner();

	me = Cast<ACypher_Kaya>(pawn);
	OnMontageEnded.AddDynamic(this, &UPlayerAnim::OnAttackMontageEnded);
}

void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (me != nullptr)
	{		
		velocity = me->GetVelocity();
		forward = me->GetActorForwardVector();
		right = me->GetActorRightVector();

		dirV = FVector::DotProduct(velocity, forward);
		dirH = FVector::DotProduct(velocity, right);
		
		bAir = me->GetCharacterMovement()->IsFalling();
	}
}

void UPlayerAnim::BasicAttackPlayAnim()
{	
	me->PlayAnimMontage(basicAttackAnimMontage, 1, TEXT("BasicAttack1"));

	//Montage_Play(basicAttackAnimMontage);
	//basicAttackCombo++;
}


void UPlayerAnim::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("basicCom: %d"), me->compKayaAttack->basicAttackCombo)
	switch (playerState)
	{
	case EPlayerState::Move:
		break;
	case EPlayerState::BasicAttack:
		//��� �޺� ������ �ְڴٴ� �Է��� �޾Ҵ�
		if (me->compKayaAttack->basicAttackComboContinue == true) {
			me->compKayaAttack->basicAttackCombo++;
			switch (me->compKayaAttack->basicAttackCombo)
			{
			case 2:
				me->PlayAnimMontage(basicAttackAnimMontage, 1, TEXT("BasicAttack2"));
				break;
			case 3:
				me->PlayAnimMontage(basicAttackAnimMontage, 1, TEXT("BasicAttack3"));
				playerState = EPlayerState::Move;
				break;
			}
			//���� �ִϸ��̼ǿ��� �޺� ��Ƽ�� üũ�� �ҰŶ� false�� �Ѵ�.
			me->compKayaAttack->basicAttackComboContinue = false;
		}
		else {
			//�ƹ� �Էµ� ���� �ʾƼ� move���·� ���ư��� ���� ��Ÿ �ִϸ��̼��� ó������ ���ư���
			playerState = EPlayerState::Move;
			me->compKayaAttack->basicAttackCombo = 1;
		}
		//me->compKayaAttack->basicAttackCombo
		break;
	default:
		break;
	}
	UE_LOG(LogTemp, Warning, TEXT("mongtage end!!"))
		//me->compKayaAttack->bAttackChk = true;
}

///
/// ���콺�� Ŭ���Ѵ�
/// ���� ���� �ִϸ��̼��� �������̴�
/// ���� �������� �ִϸ��̼��� combo count�� 0�̶�� 
/// ���� Ŭ���� �ƴٸ� ���� �ִϸ��̼��� �����Ѵ�.
/// ��
///