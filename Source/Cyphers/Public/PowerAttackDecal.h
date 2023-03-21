// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerAttackDecal.generated.h"

UCLASS()
class CYPHERS_API APowerAttackDecal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerAttackDecal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		class USceneComponent* rootComp;
	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* compDecal;

		FVector currScale;
		UPROPERTY(EditAnywhere)
		float RotationSpeed = 20;
		float CurrentRotate;

		UPROPERTY(EditAnywhere)
		float biggerSpeed= 26;
		float currSize;
		UPROPERTY(EditAnywhere)
		float MaxScale = 20;

		bool bPowerAttackEnd;
};
