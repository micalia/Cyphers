// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndPoint.generated.h"

UCLASS()
class CYPHERS_API AEndPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEndPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* compBox;
	UPROPERTY()
		class UNiagaraSystem* NiagaraSystemAsset;
	UPROPERTY(EditAnywhere)
		class UNiagaraComponent* compNiagra;
};
