// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMoveInput.h"
#include "Cypher_Kaya.h"
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "Cypher_Kaya_Attack.h"


UPlayerMoveInput::UPlayerMoveInput()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerMoveInput::BeginPlay()
{
	Super::BeginPlay();
	kaya = Cast<ACypher_Kaya>(me);
	me->GetCharacterMovement()->MaxWalkSpeed = moveSpeed;
	//Controller 의 회전값을 따라 갈 속성 셋팅
	me->bUseControllerRotationYaw = true;
	//me->compArm->bUsePawnControlRotation = true;

	my = -kaya->camTarget->GetRelativeRotation().Pitch;
	UE_LOG(LogTemp, Warning, TEXT("my: %f"), my)
}

void UPlayerMoveInput::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (kaya->bDamageState == false) {
		MoveAction(DeltaTime);
	}

}

void UPlayerMoveInput::SetupInputBinding(class UInputComponent* PlayerInputComponent)
{
	Super::SetupInputBinding(PlayerInputComponent);
	//A, D
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &UPlayerMoveInput::InputHorizontal);
	//W, S
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &UPlayerMoveInput::InputVertical);
	//마우스 좌우
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &UPlayerMoveInput::InputTurn);
	//마우스 상하
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &UPlayerMoveInput::InputLookUp);

}

void UPlayerMoveInput::MoveAction(float deltaTime)
{
	if(kaya->compKayaAttack->bBackCameraOringinPos || kaya->bBossCameraEffect)return;
	direction = me->GetActorForwardVector() * v + me->GetActorRightVector() * h;
	
	me->AddMovementInput(direction.GetSafeNormal());
}

void UPlayerMoveInput::RotateAction()
{
	me->SetActorRotation(FRotator(0, mx, 0));
	//me->compArm->SetRelativeRotation(FRotator(-my, 0, 0));
}

void UPlayerMoveInput::InputHorizontal(float value)
{
	h = value;
}

void UPlayerMoveInput::InputVertical(float value)
{
	v = value;
}

void UPlayerMoveInput::InputLookUp(float value)
{
	if(kaya->bCameraPosFix) return;
	if(kaya->compKayaAttack->bAttackCharge) return;
	me->AddControllerPitchInput(value);
	my+=value;

		if (-my < minCamPitch) {
			my = -minCamPitch;
		}
		else if(-my > maxCamPitch) {
			my = -maxCamPitch;
		}
	kaya->camTarget->SetRelativeRotation(FRotator(FMath::Clamp(-my, minCamPitch, maxCamPitch), 0, 0));
}

void UPlayerMoveInput::InputTurn(float value)
{
	if (kaya->bCameraPosFix) return;
	if (kaya->compKayaAttack->bAttackCharge) return;
	me->AddControllerYawInput(value);
}

float UPlayerMoveInput::GetH()
{
	return h;
}

