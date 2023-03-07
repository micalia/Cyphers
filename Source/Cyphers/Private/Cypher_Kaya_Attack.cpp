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
	/// �ϴ� ù��° �޺� �ִϸ��̼ǟ� �����Ѵ�
	/// ���� �ִϸ��̼��� 
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
		//���� ���� ���°� �ƴ϶��
		kayaAnim->playerState = EPlayerState::BasicAttack;
		UE_LOG(LogTemp, Warning, TEXT("kayaAnim->playerState != EPlayerState::BasicAttack"))
		kayaAnim->BasicAttackPlayAnim();
	}
	else {
		//���� BasicAttack���¶��
		//�ι�° �޺� ���ʹ� ���� �������� �����ϰ� �Լ��� �����Ѵ�
		//���콺�� �������� ���� ���� ���� �ִϸ��̼� üũ �����̶��?
		if (basicAttackComboContinueCheckSection == true) {
			//�⺻���� �ִϸ��̼� ��Ÿ�ְ� ������ ȣ��Ǵ� �Լ����� �޺� �ε��� ����
			if(basicAttackComboContinue == true)return; // �̹� ���� �ִϸ��̼��� �����ϰڴٰ� ������ �� �����̱� ������ ����� �ִϸ��̼��� ����Ǳ� ������ �޺� �ε����� �ø��� �ʰڴ�
			//�޺� �ִ� �ε����� ������Ų��.
			//�޺� üũ �������� Ŭ���� �������ϱ� ���� ������ �޺���
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
///���� basicAttackComboContinue �� true��� ���� ��Ÿ�ְ� �����´�� ���� ��Ÿ�� ����

	/// <summary>
	/// �ִϸ��̼� �޺��� ������ ������ idle���·�
	/// attack = true =>ó������ ������ �� �ִ�.
	///���콺 ��Ŭ���� �Ѵ�.
	///attack = false�� �����. = > ���� �ִϸ��̼��� ����Ǹ� �ȵȴ�.
	/// if������ ��Ÿ�ָ� �����Ų��.
	/// ��Ÿ�ֿ���
	/// attackBegin�� ���۵Ǹ� 
	/// ���� �������� �ٲ��ְ� ������ ������ �ִϸ��̼��� �־��ش�
	/// �־��ִ� ������ 
	/// �׸��� attackEnd�� �Ǹ� idle�ִϸ��̼��� �־��ش�.
	/// ���� Attackbeing�� End���̿� Ŭ���� ���ϸ� �ʱ� �ִϸ��̼� iodle�� ����
	/// ��Ÿ�ֿ����� ���� ���� �ִϸ��̼��� �־��ش�.
	/// 
	/// ���� Begin �� end���̿� Ŭ���� �ѹ� �ƴٸ� 
	/// ���� �ִϸ��̼��� �����Ѵ�.
	/// </summary>