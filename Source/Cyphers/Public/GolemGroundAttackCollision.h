// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GolemGroundAttackCollision.generated.h"

UCLASS()
class CYPHERS_API AGolemGroundAttackCollision : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGolemGroundAttackCollision();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		class UBoxComponent* groundAttackCollision;

	UPROPERTY(EditAnywhere)
	float MoveSpeed = 100;

	UPROPERTY(EditAnywhere)
	float lifeTime = 2.7;
	float currTime;

	UFUNCTION()
		void OnCollisionOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);
};
