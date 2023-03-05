// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMoveInput.h"
#include "Cypher_Kaya.h"
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/CharacterMovementComponent.h>



UPlayerMoveInput::UPlayerMoveInput()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerMoveInput::BeginPlay()
{
	Super::BeginPlay();
	
	me->GetCharacterMovement()->MaxWalkSpeed = moveSpeed;
	//Controller �� ȸ������ ���� �� �Ӽ� ����
	me->bUseControllerRotationYaw = true;
	me->compArm->bUsePawnControlRotation = true;
}

void UPlayerMoveInput::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	MoveAction(DeltaTime);
	UE_LOG(LogTemp, Warning, TEXT("Direction: %s"), *direction.ToString())
		UE_LOG(LogTemp, Warning, TEXT("H: %f"),h)
		UE_LOG(LogTemp, Warning, TEXT("V: %f"),v)
}

void UPlayerMoveInput::SetupInputBinding(class UInputComponent* PlayerInputComponent)
{
	Super::SetupInputBinding(PlayerInputComponent);
	//A, D
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &UPlayerMoveInput::InputHorizontal);
	//W, S
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &UPlayerMoveInput::InputVertical);
	//���콺 �¿�
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &UPlayerMoveInput::InputTurn);
	//���콺 ����
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &UPlayerMoveInput::InputLookUp);

}

void UPlayerMoveInput::MoveAction(float deltaTime)
{
	direction = me->GetActorForwardVector() * v + me->GetActorRightVector() * h;

	me->AddMovementInput(direction.GetSafeNormal());
}

void UPlayerMoveInput::RotateAction()
{
	me->SetActorRotation(FRotator(0, mx, 0));
	me->compArm->SetRelativeRotation(FRotator(-my, 0, 0));
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
	me->AddControllerPitchInput(value);
}

void UPlayerMoveInput::InputTurn(float value)
{
	me->AddControllerYawInput(value);
}