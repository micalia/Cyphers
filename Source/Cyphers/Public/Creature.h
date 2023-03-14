// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Creature.generated.h"

UCLASS()
class CYPHERS_API ACreature : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACreature();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	UPROPERTY(EditAnywhere, Category = Stat)
	float power=1;
	UPROPERTY(EditAnywhere, Category = Stat)
	float fatal=1;
	UPROPERTY(EditAnywhere, Category = Stat)
	float health=1;
	UPROPERTY(EditAnywhere, Category = Stat)
	float avoid=1;
	UPROPERTY(EditAnywhere, Category = Stat)
	float defence=1;
	UPROPERTY(EditAnywhere, Category = Stat)
	float moveSpeed=1;
};
