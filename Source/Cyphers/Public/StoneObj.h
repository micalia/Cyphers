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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* compRock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* compCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* compCheckPlayerCollision;
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundBase* stoneSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UGeometryCollectionComponent* compGcStone;

public:
	UFUNCTION()
	void CrashWithPlayer(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	UFUNCTION()
	void OnCheckPlayerOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
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

	UPROPERTY(EditAnywhere)
	float autoDestroyTime = 12;
	float autoDestroyCurrTime;

	bool bThrowOn;
	UPROPERTY(EditAnywhere)
	float Speed = 9200;  
	FVector ThrowDir;
	
};
