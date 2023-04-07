// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StoneMoveSmoke.generated.h"

UCLASS()
class CYPHERS_API AStoneMoveSmoke : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStoneMoveSmoke();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		class USceneComponent* rootComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UParticleSystemComponent* vfxComp;

};
