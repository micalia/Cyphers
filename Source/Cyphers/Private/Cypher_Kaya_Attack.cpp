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
		//UE_LOG(LogTemp, Warning, TEXT("M"))
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		//CanNextCombo=false;
		if (IsComboInputOn) {
			AttackStartComboState();
			kayaAnim->BasicAttackMontageSection(CurrentCombo);
		}
		});
}

void UCypher_Kaya_Attack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//마우스 양클릭 체크 후 공격 실행
	if (bAttackInput == true) {
		MouseLRCheckCurrentTime += DeltaTime;
		if (MouseLRCheckCurrentTime > MouseLRCheckTime) {			
			if (bLeftMouseButtonPressed == true && bRightMouseButtonPressed == true) {
				InitInput();
				if(IsAttacking == true) return;
				AttackEndComboState(); // 만약 평타 1회 또는 2회 후에 양클릭을 한경우에는 기본 콤보를 0으로 만듦
				//마우스 양클릭 공격실행	
				UE_LOG(LogTemp, Warning, TEXT("Both Click!!"))
				DashAttack();
			}
			else if (bLeftMouseButtonPressed) {
					//마우스 왼쪽공격
					UE_LOG(LogTemp, Warning, TEXT("mouseLeft"))
					BasicAttack();
				InitInput();
			}
			else if(bRightMouseButtonPressed){
				//마우스 오른쪽 공격
				UE_LOG(LogTemp, Warning, TEXT("RightClick"))
				InitInput();
			}
			
		}
	}

}

void UCypher_Kaya_Attack::SetupInputBinding(class UInputComponent* PlayerInputComponent)
{
	Super::SetupInputBinding(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("MouseLeft"), IE_Pressed, this, &UCypher_Kaya_Attack::InputMouseLeft);
	PlayerInputComponent->BindAction(TEXT("MouseRight"), IE_Pressed, this, &UCypher_Kaya_Attack::InputMouseRight);
	PlayerInputComponent->BindAction(TEXT("MouseLeftAndShift"), IE_Pressed, this, &UCypher_Kaya_Attack::InputKeyShiftAndMouseLeft);
}

void UCypher_Kaya_Attack::InitInput()
{
	bAttackInput = false;
	MouseLRCheckCurrentTime = 0;
	bLeftMouseButtonPressed = false;
	bRightMouseButtonPressed = false;
}


void UCypher_Kaya_Attack::BasicAttack()
{
	if (IsAttacking) { //현재 이미 좌클릭 공격이 실행중인데 입력값이 들어오면
		//ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		//if (CanNextCombo) {
		IsComboInputOn = true;
		//}
	}
	else { //첫번째 공격
		//ABCHECK(CurrentCombo == 0);
		AttackStartComboState();
		kayaAnim->BasicAttackPlayAnim();
		kayaAnim->BasicAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void UCypher_Kaya_Attack::AttackStartComboState()
{
	//CanNextCombo = true;
	IsComboInputOn = false;
	//ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo-1));
	CurrentCombo =FMath::Clamp<int32>(CurrentCombo+1, 1, MaxCombo);
}

void UCypher_Kaya_Attack::AttackEndComboState()
{
	IsComboInputOn = false;
	//CanNextCombo = false;
	CurrentCombo = 0;
}

void UCypher_Kaya_Attack::InputMouseLeft()
{
	//ClickTimestamp = FPlatformTime::Seconds();
	bAttackInput = true;
	bLeftMouseButtonPressed = true;	
}

void UCypher_Kaya_Attack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("montageEnded"))
	ABCHECK(IsAttacking);
	//ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
}

void UCypher_Kaya_Attack::InputMouseLeftAndRight()
{
	UE_LOG(LogTemp, Warning, TEXT("InputMouseLeftAndRight()"))
}

void UCypher_Kaya_Attack::InputKeyShiftAndMouseLeft()
{
	UE_LOG(LogTemp, Warning, TEXT("Shift mouse left!"))
}

void UCypher_Kaya_Attack::InputMouseRight()
{
	//float TimeInterval = FPlatformTime::Seconds() - ClickTimestamp;
	//UE_LOG(LogTemp, Warning, TEXT("Time interval: %f"), TimeInterval)
	bAttackInput = true;
	bRightMouseButtonPressed = true;
}

void UCypher_Kaya_Attack::DashAttack()
{
	IsAttacking=true;
	kayaAnim->DashAttackPlayAnim();
}
