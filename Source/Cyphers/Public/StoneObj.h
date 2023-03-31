// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StoneObj.generated.h"

UCLASS()
class CYPHERS_API AStoneObj : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStoneObj();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY()
		class AGolem* enemy;
	UPROPERTY()
		class UStaticMesh* rockMesh;
	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* compRock;
	UPROPERTY(EditAnywhere)
		class USphereComponent* compCollision;
	UPROPERTY(EditDefaultsOnly, Category = Sound)
		class USoundBase* stoneSound;
	UPROPERTY(EditAnywhere)
		class UGeometryCollectionComponent* compGcStone;

public:
	UFUNCTION()
		void CrashWithPlayer(
			UPrimitiveComponent* HitComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			FVector NormalImpulse,
			const FHitResult& Hit);
public:
	UPROPERTY(EditAnywhere)
		float destroyTime = 5;
	UPROPERTY(EditAnywhere)
		float transparentStartTime = 2.8;
	UPROPERTY(EditAnywhere)
		float transparentTerm = 1.5;
	float alpha = 0;
	float currTime = 0;
	float transparentCurrTime = 0;
	bool bIsCrash = false;
	bool bTransparent = false;
};
