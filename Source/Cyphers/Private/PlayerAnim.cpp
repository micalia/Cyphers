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
		//계속 콤보 공격을 넣겠다는 입력을 받았다
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
			//다음 애니메이션에도 콤보 컨티뉴 체크를 할거라 false를 한다.
			me->compKayaAttack->basicAttackComboContinue = false;
		}
		else {
			//아무 입력도 받지 않아서 move상태로 돌아가고 다음 평타 애니메이션은 처음으로 돌아간다
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
/// 마우스를 클릭한다
/// 만약 현재 애니메이션이 진행중이다
/// 현재 진행중인 애니메이션의 combo count가 0이라면 
/// 만약 클릭이 됐다면 다음 애니메이션을 예약한다.
/// ㅁ
///