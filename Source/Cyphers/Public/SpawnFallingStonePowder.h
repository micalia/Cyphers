// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnFallingStonePowder.generated.h"

UCLASS()
class CYPHERS_API ASpawnFallingStonePowder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnFallingStonePowder();

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLoop = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Interval = 0.3;

	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteStonePowderEffect();
};
