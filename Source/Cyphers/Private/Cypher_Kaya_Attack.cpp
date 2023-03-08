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
//�̵�� �Ϲݰ��� ����
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
	/// �ϴ� ù��° �޺� �ִϸ��̼ǟ� �����Ѵ�
	/// ���� �ִϸ��̼��� 
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
	//���� ���� ����
	//if (kayaAnim->playerState != EPlayerState::BasicAttack) {
	//	//���� ���� ���°� �ƴ϶��
	//	kayaAnim->playerState = EPlayerState::BasicAttack;
	//	UE_LOG(LogTemp, Warning, TEXT("kayaAnim->playerState != EPlayerState::BasicAttack"))
	//	kayaAnim->BasicAttackPlayAnim();
	//}
	//else {
	//	//���� BasicAttack���¶��
	//	//�ι�° �޺� ���ʹ� ���� �������� �����ϰ� �Լ��� �����Ѵ�
	//	//���콺�� �������� ���� ���� ���� �ִϸ��̼� üũ �����̶��?
	//	if (basicAttackComboContinueCheckSection == true) {
	//		//�⺻���� �ִϸ��̼� ��Ÿ�ְ� ������ ȣ��Ǵ� �Լ����� �޺� �ε��� ����
	//		if(basicAttackComboContinue == true)return; // �̹� ���� �ִϸ��̼��� �����ϰڴٰ� ������ �� �����̱� ������ ����� �ִϸ��̼��� ����Ǳ� ������ �޺� �ε����� �ø��� �ʰڴ�
	//		//�޺� �ִ� �ε����� ������Ų��.
	//		//�޺� üũ �������� Ŭ���� �������ϱ� ���� ������ �޺���
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
	//�̵�� ����
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();

		//���� ����
			//UE_LOG(LogTemp, Warning, TEXT("basicCom: %d"), me->compKayaAttack->basicAttackCombo)
			//switch (playerState)
			//{
			//case EPlayerState::Move:
			//	break;
			//case EPlayerState::BasicAttack:
			//	//��� �޺� ������ �ְڴٴ� �Է��� �޾Ҵ�
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
			//		//���� �ִϸ��̼ǿ��� �޺� ��Ƽ�� üũ�� �ҰŶ� false�� �Ѵ�.
			//		me->compKayaAttack->basicAttackComboContinue = false;
			//	}
			//	else {
			//		//�ƹ� �Էµ� ���� �ʾƼ� move���·� ���ư��� ���� ��Ÿ �ִϸ��̼��� ó������ ���ư���
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
