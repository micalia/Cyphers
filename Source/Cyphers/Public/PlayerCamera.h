// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerCamera.generated.h"

UCLASS()
class CYPHERS_API APlayerCamera : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	APlayerCamera();
	void SetAsMainCamera();
	virtual void Tick(float DeltaSeconds);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* CameraComponent;
	
};
