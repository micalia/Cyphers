// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class CYPHERS_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
//내가 선언
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* smComp;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* smCannonComp;
	UPROPERTY(EditAnywhere)
		USceneComponent* cannonPos;
public:
//GPT
	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		TSubclassOf<AGrenade> GrenadeClass;*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		float GrenadeVelocity = 1000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		float GrenadeLifetime = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		float GrenadeFuseTime=3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		float GrenadeThrowAngle =45;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		FName GrenadeSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grenade)
		USoundBase* GrenadeThrowSound;

protected:
	void ClearGrenadeTrajectory();
	void ShowGrenadeTrajectory();
	void OnGrenadeStart();
	void OnGrenadeRelease();

	//TArray<ADebugSphereActor*> GrenadeTrajectoryPoints;
	FVector GrenadeStartLocation;
	FRotator GrenadeStartRotation;
};
