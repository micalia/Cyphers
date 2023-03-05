// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseComponent.h"
#include "PlayerMoveInput.generated.h"

/**
 * 
 */
UCLASS()
class CYPHERS_API UPlayerMoveInput : public UPlayerBaseComponent
{
	GENERATED_BODY()

public:	
	UPlayerMoveInput();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SetupInputBinding(class UInputComponent* PlayerInputComponent) override;

public:
	//W, A, S, D Ű�� ���� �� ����
	float h = 0;
	float v = 0;

	//���콺 �����ӿ� ���� �� ����
	float mx = 0;
	float my = 0;

	FVector direction;

	UPROPERTY(EditAnywhere, Category=PlayerSetting)
	float moveSpeed = 600;

public:
	void MoveAction(float deltaTime);
	void RotateAction();

	void InputHorizontal(float value);
	void InputVertical(float value);
	void InputLookUp(float value);
	void InputTurn(float value);
};