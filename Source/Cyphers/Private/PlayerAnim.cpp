#include "PlayerAnim.h"
#include "Cypher_Kaya.h"
#include <GameFramework/CharacterMovementComponent.h>

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
	switch (basicAttackCombo)
	{
	case 0:
		me->PlayAnimMontage(basicAttackAnimMontage, 1, TEXT("BasicAttack1"));
		break;
	case 1:
		me->PlayAnimMontage(basicAttackAnimMontage, 1, TEXT("BasicAttack2"));
		break;
	case 2:
		me->PlayAnimMontage(basicAttackAnimMontage, 1, TEXT("BasicAttack3"));
		break;
	}
	//Montage_Play(basicAttackAnimMontage);
	//basicAttackCombo++;
}

///
/// 마우스를 클릭한다
/// 만약 현재 애니메이션이 진행중이다
/// 현재 진행중인 애니메이션의 combo count가 0이라면 
/// 
/// ㅁ
///