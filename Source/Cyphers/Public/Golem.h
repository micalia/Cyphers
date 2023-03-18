// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Creature.h"
#include "Golem.generated.h"

/**
 * 
 */
UCLASS()
class CYPHERS_API AGolem : public ACreature
{
	GENERATED_BODY()
public:
	AGolem();

	UPROPERTY(EditAnywhere, Category = ThrowStoneAttack)
		TSubclassOf<class AStoneObj> stoneFactory;
};
