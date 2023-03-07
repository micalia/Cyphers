// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBaseComponent.h"
#include "Cypher_Kaya_Attack.generated.h"

/**
 * 
 */
UCLASS()
class CYPHERS_API UCypher_Kaya_Attack : public UPlayerBaseComponent
{
	GENERATED_BODY()
public:
	UCypher_Kaya_Attack();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SetupInputBinding(class UInputComponent* PlayerInputComponent) override;
public:
	UPROPERTY()
	class UPlayerAnim* kayaAnim;

//평타
	UPROPERTY(BlueprintReadWrite)
	bool bAttackChk = true;
	UPROPERTY(BlueprintReadWrite)
	int32 basicAttackCombo = 1;
	UPROPERTY(BlueprintReadWrite)
	bool basicAttackComboContinueCheckSection = false;
	UPROPERTY(BlueprintReadWrite)
	bool basicAttackComboContinue = false;
public:
	void InputMouseLeft();	//평타
	/*void InputMouseRight();
	void InputMouseLeftAndRight();
	void InputKeyShiftAndMouseLeft();
	void InputKeySpace();
	void InputKeyE();*/
};
