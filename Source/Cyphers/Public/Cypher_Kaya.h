// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Creature.h"
#include "Cypher_Kaya.generated.h"

/**
 * 
 */
UCLASS()
class CYPHERS_API ACypher_Kaya : public ACreature
{
	GENERATED_BODY()
public:
	ACypher_Kaya();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* compSword;

public:
//만약 나중에 AI로도 Kaya를 만들경우 수정이 필요할수도 있음
	UPROPERTY(VisibleAnywhere, Category = Component)
	class UPlayerMoveInput* compPlayerMove;
	UPROPERTY(VisibleAnywhere, Category = Component)
		class UCypher_Kaya_Attack* compKayaAttack;
	UPROPERTY(EditAnywhere)
		class USpringArmComponent* compArm;

	UPROPERTY(EditAnywhere)
		class UCameraComponent* compCam;
};
