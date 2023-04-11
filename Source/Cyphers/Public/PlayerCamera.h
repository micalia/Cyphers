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
	UPROPERTY(EditAnywhere, Category = Camera)
		class USceneComponent* powerAttackCamMove;
	UPROPERTY(EditAnywhere, Category = Camera)
		class USceneComponent* compRoot;

	UPROPERTY(EditDefaultsOnly, Category = CameraMotion)
		TSubclassOf<class UCameraShakeBase> cameraShake;
	
	//ī�޶� ��鸲
	FCameraMoveDelegate CameraMoveDelegate;

	void CameraMoveEvent();
	bool bCameraShake = false;
	FVector cameraOriginPos;
	UPROPERTY(EditAnywhere)
		float cameraShakeTime = 1.1;
	float camCurrTime;
	void CameraShakeRandom();
	//�ñر� ȿ��
	bool bSkillCamMove;
	bool bSkillReady;
	FVector camOriginPosMove;
	void SkillCamMove();
	FVector prevCamPos;
	UPROPERTY(EditAnywhere)
	float SkillCamSpeed = 3;
};
