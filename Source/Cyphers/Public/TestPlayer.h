// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Creature.h"
#include "TestPlayer.generated.h"

/**
 * 
 */
UCLASS()
class CYPHERS_API ATestPlayer : public ACreature
{
	GENERATED_BODY()
	
public:
	ATestPlayer();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = Component)
	class UPlayerMoveInput* compPlayerMove;
};
