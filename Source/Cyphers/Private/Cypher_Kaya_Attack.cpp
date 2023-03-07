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
	
	kayaAnim = Cast<UPlayerAnim>(me->GetMesh()->GetAnimInstance());
}

void UCypher_Kaya_Attack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	/*if (basicAttackComboContinueCheckSection) {
	UE_LOG(LogTemp, Warning, TEXT("basicAttackComboContinueCheckSection : true"))

	}
	else {
	UE_LOG(LogTemp, Warning, TEXT("basicAttackComboContinueCheckSection : false"))

	}*/
	//UE_LOG(LogTemp, Warning, TEXT("msg"))
}

void UCypher_Kaya_Attack::SetupInputBinding(class UInputComponent* PlayerInputComponent)
{
	Super::SetupInputBinding(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("MouseLeft"), IE_Pressed, this, &UCypher_Kaya_Attack::InputMouseLeft);
}

void UCypher_Kaya_Attack::InputMouseLeft()
{
	///
	/// 일단 첫번째 콤보 애니메이션읈 실행한다
	/// 현재 애니메이션이 
	///

	UE_LOG(LogTemp, Warning, TEXT("mouseLeft"))

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
	if (kayaAnim->playerState != EPlayerState::BasicAttack) {
		//현재 공격 상태가 아니라면
		kayaAnim->playerState = EPlayerState::BasicAttack;
		UE_LOG(LogTemp, Warning, TEXT("kayaAnim->playerState != EPlayerState::BasicAttack"))
		kayaAnim->BasicAttackPlayAnim();
	}
	else {
		//현재 BasicAttack상태라면
		//두번째 콤보 부터는 여기 스코프만 실행하고 함수를 종료한다
		//마우스를 눌렀을때 현재 다음 어택 애니메이션 체크 구간이라면?
		if (basicAttackComboContinueCheckSection == true) {
			//기본공격 애니메이션 몽타주가 끝날때 호출되는 함수에서 콤보 인덱스 증가
			if(basicAttackComboContinue == true)return; // 이미 다음 애니메이션을 실행하겠다고 예약을 한 상태이기 때문에 예약된 애니메이션이 실행되기 전까지 콤보 인덱스를 늘리지 않겠다
			//콤보 애님 인덱스를 증가시킨다.
			//콤보 체크 구간에서 클릭을 눌렀으니까 다음 공격은 콤보다
			basicAttackComboContinue = true;
			//kayaAnim->BasicAttackPlayAnim();
		
			return;
		}
	}



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
///만약 basicAttackComboContinue 가 true라면 현재 몽타주가 끝나는대로 다음 몽타주 실행

	/// <summary>
	/// 애니메이션 콤보가 끝나면 무조건 idle상태로
	/// attack = true =>처음에는 공격할 수 있다.
	///마우스 좌클릭을 한다.
	///attack = false로 만든다. = > 다음 애니메이션이 실행되면 안된다.
	/// if문에서 몽타주를 실행시킨다.
	/// 몽타주에서
	/// attackBegin이 시작되면 
	/// 공격 가능으로 바꿔주고 다음에 실행할 애니메이션을 넣어준다
	/// 넣어주는 변수는 
	/// 그리고 attackEnd가 되면 idle애니메이션을 넣어준다.
	/// 만약 Attackbeing과 End사이에 클릭을 안하면 초기 애니메이션 iodle을 실행
	/// 몽타주에서는 다음 공격 애니메이션을 넣어준다.
	/// 
	/// 만약 Begin 과 end사이에 클릭이 한번 됐다면 
	/// 다음 애니메이션을 예약한다.
	/// </summary>