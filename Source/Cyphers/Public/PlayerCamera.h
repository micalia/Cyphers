// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerCamera.generated.h"

DECLARE_DELEGATE(FCameraMoveDelegate);

UCLASS()
class CYPHERS_API APlayerCamera : public AActor
{
	GENERATED_BODY()
public:	
	virtual void BeginPlay() override;
	// Sets default values for this actor's properties
	APlayerCamera();
	void SetAsMainCamera();
	virtual void Tick(float DeltaSeconds) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = CameraMotion)
		TSubclassOf<class UCameraShakeBase> cameraShake;
	
	//카메라 흔들림
	FCameraMoveDelegate CameraMoveDelegate;

	void CameraMoveEvent();
	bool bCameraShake = false;
	FVector cameraOriginPos;
	UPROPERTY(EditAnywhere)
		float cameraShakeTime = 1.1;
	float camCurrTime;
	void CameraShakeRandom();
};
