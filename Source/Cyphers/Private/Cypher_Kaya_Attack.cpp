// Fill out your copyright notice in the Description page of Project Settings.


#include "Cypher_Kaya_Attack.h"
#include "PlayerAnim.h"
#include "Cypher_Kaya.h"


UCypher_Kaya_Attack::UCypher_Kaya_Attack()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCypher_Kaya_Attack::BeginPlay()
{
	Super::BeginPlay();
	AttackEndComboState();
	kayaAnim = Cast<UPlayerAnim>(me->GetMesh()->GetAnimInstance());
	
	kayaAnim->OnMontageEnded.AddDynamic(this, &UCypher_Kaya_Attack::OnAttackMontageEnded);

	kayaAnim->OnNextAttackCheck.AddLambda([this]()->void {
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo=false;
		UE_LOG(LogTemp, Warning, TEXT("add lamda!!!!!!!!"))
		if (IsComboInputOn) {
			AttackStartComboState();
			UE_LOG(LogTemp, Warning, TEXT("Lamda _ CurrentCombo : %d"), CurrentCombo)
			kayaAnim->BasicAttackMontageSection(CurrentCombo);
		}
	});
}

void UCypher_Kaya_Attack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCypher_Kaya_Attack::SetupInputBinding(class UInputComponent* PlayerInputComponent)
{
	Super::SetupInputBinding(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("MouseLeft"), IE_Pressed, this, &UCypher_Kaya_Attack::InputMouseLeft);
}

void UCypher_Kaya_Attack::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo-1));
	CurrentCombo =FMath::Clamp<int32>(CurrentCombo+1, 1, MaxCombo);
}

void UCypher_Kaya_Attack::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void UCypher_Kaya_Attack::InputMouseLeft()
{
	UE_LOG(LogTemp, Warning, TEXT("mouseLeft"))
//이득우 일반공격 로직
	if (IsAttacking) {
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (CanNextCombo) {
			IsComboInputOn = true;
		}
	}
	else {
		ABCHECK(CurrentCombo == 0);
		AttackStartComboState();
		kayaAnim->BasicAttackPlayAnim();
		kayaAnim->BasicAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
	///
	/// 일단 첫번째 콤보 애니메이션읈 실행한다
	/// 현재 애니메이션이 
	///


	/*UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPlayerState"), true);
	if (enumPtr != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Current PlayerState : %s"), *enumPtr->GetNameStringByIndex((int32)kayaAnim->playerState));
	}
	UE_LOG(LogTemp, Warning, TEXT("currentCombo: %d"), basicAttackCombo)
		if (basicAttackComboContinueCheckSection == true) {
			UE_LOG(LogTemp, Warning, TEXT("basicAttackComboContinueCheckSection : true"))
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("basicAttackComboContinueCheckSection : false"))
		}*/
	//내가 직접 구현
	//if (kayaAnim->playerState != EPlayerState::BasicAttack) {
	//	//현재 공격 상태가 아니라면
	//	kayaAnim->playerState = EPlayerState::BasicAttack;
	//	UE_LOG(LogTemp, Warning, TEXT("kayaAnim->playerState != EPlayerState::BasicAttack"))
	//	kayaAnim->BasicAttackPlayAnim();
	//}
	//else {
	//	//현재 BasicAttack상태라면
	//	//두번째 콤보 부터는 여기 스코프만 실행하고 함수를 종료한다
	//	//마우스를 눌렀을때 현재 다음 어택 애니메이션 체크 구간이라면?
	//	if (basicAttackComboContinueCheckSection == true) {
	//		//기본공격 애니메이션 몽타주가 끝날때 호출되는 함수에서 콤보 인덱스 증가
	//		if(basicAttackComboContinue == true)return; // 이미 다음 애니메이션을 실행하겠다고 예약을 한 상태이기 때문에 예약된 애니메이션이 실행되기 전까지 콤보 인덱스를 늘리지 않겠다
	//		//콤보 애님 인덱스를 증가시킨다.
	//		//콤보 체크 구간에서 클릭을 눌렀으니까 다음 공격은 콤보다
	//		basicAttackComboContinue = true;
	//		//kayaAnim->BasicAttackPlayAnim();
	//	
	//		return;
	//	}
	//}



	/*if (bAttackChk) {
		bAttackChk = false;
		if (basicAttackCombo > 3) basicAttackCombo = 1;
		UE_LOG(LogTemp, Warning, TEXT("MouseLeft, Current Basic Combo : %d"), basicAttackCombo)
			kayaAnim->BasicAttackPlayAnim(basicAttackCombo);
		basicAttackCombo++;

	}
	else {
		return;
	}*/

}

void UCypher_Kaya_Attack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//이득우 로직
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();

		//내가 구현
			//UE_LOG(LogTemp, Warning, TEXT("basicCom: %d"), me->compKayaAttack->basicAttackCombo)
			//switch (playerState)
			//{
			//case EPlayerState::Move:
			//	break;
			//case EPlayerState::BasicAttack:
			//	//계속 콤보 공격을 넣겠다는 입력을 받았다
			//	if (me->compKayaAttack->basicAttackComboContinue == true) {
			//		me->compKayaAttack->basicAttackCombo++;
			//		switch (me->compKayaAttack->basicAttackCombo)
			//		{
			//		case 2:
			//			me->PlayAnimMontage(basicAttackAnimMontage, 1, TEXT("BasicAttack2"));
			//			break;
			//		case 3:
			//			me->PlayAnimMontage(basicAttackAnimMontage, 1, TEXT("BasicAttack3"));
			//			playerState = EPlayerState::Move;
			//			break;
			//		}
			//		//다음 애니메이션에도 콤보 컨티뉴 체크를 할거라 false를 한다.
			//		me->compKayaAttack->basicAttackComboContinue = false;
			//	}
			//	else {
			//		//아무 입력도 받지 않아서 move상태로 돌아가고 다음 평타 애니메이션은 처음으로 돌아간다
			//		playerState = EPlayerState::Move;
			//		me->compKayaAttack->basicAttackCombo = 1;
			//	}
			//	//me->compKayaAttack->basicAttackCombo
			//	break;
			//default:
			//	break;
			//}
		UE_LOG(LogTemp, Warning, TEXT("mongtage end!!"))
}
