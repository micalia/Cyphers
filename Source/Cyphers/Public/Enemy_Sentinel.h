// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Creature.h"
#include "Enemy_Sentinel.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_OneParam(FEnemySentinelDieDelegate, class AEnemy_Sentinel*);

UCLASS()
class CYPHERS_API AEnemy_Sentinel : public ACreature
{
	GENERATED_BODY()
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	AEnemy_Sentinel();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//적 AI 관리 하는 컴포넌트(FSM)
	UPROPERTY(EditAnywhere)
		class UEnemy_SentinelFSM* fsm;

	// 활성 / 비활성 하는 기능
	void SetActive(bool bActive);

	//죽었을 때 호출되는 함수를 담는 변수
	FEnemySentinelDieDelegate dieDelegate;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UEnemy_SentinelHpUI> sentinelUIfactory;
	UPROPERTY(EditAnywhere)
	class UWidgetComponent* hpWidget;

private:
	APlayerController* PlayerController;
	FVector CameraLocation;
	FRotator CameraRotation;
	FRotator WidgetRotation;

	void UpdateWidgetRotation();
	FVector GetPlayerCameraLocation();
	FRotator GetPlayerCameraRotation();
	FTimerHandle TimerHandle_UpdateWidgetRotation;
};
