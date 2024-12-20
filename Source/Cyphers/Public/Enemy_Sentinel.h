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
	
//데미지 효과
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UWidgetComponent* damageIndicatorWidget;
	UFUNCTION(BlueprintImplementableEvent)
		void DamageNumber(float Damage);
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UDamageIndicator> DamageIndicatorFactory;
		UPROPERTY()
		class UDamageIndicator* damageIndicator;

	UPROPERTY()
	class ACypher_Kaya* kaya;

	void ReceiveDamage();
	void ReceiveGripAttackDamage();
	//적 AI 관리 하는 컴포넌트(FSM)
	UPROPERTY(EditAnywhere)
		class UEnemy_SentinelFSM* fsm;

	//죽었을 때 호출되는 함수를 담는 변수
	FEnemySentinelDieDelegate dieDelegate;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UEnemy_SentinelHpUI> sentinelUIfactory;
	UPROPERTY(EditAnywhere)
	class UWidgetComponent* hpWidget;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* attackCollision;
	UPROPERTY(EditAnywhere)
	FVector atKCollisionSize = FVector(92, 58, 86);
	UPROPERTY()
	class UEnemy_SentinelHpUI* sentinelHpUI;
	UPROPERTY(EditAnywhere)
	float maxHP = 12000;
	float currHP;

	UFUNCTION()
		void OnAttackOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* GA2_Sound;
private:
	
	FVector CameraLocation;
	FRotator CameraRotation;
	FRotator WidgetRotation;

	void UpdateWidgetRotation();
	FVector GetPlayerCameraLocation();
	FRotator GetPlayerCameraRotation();
	FTimerHandle TimerHandle_UpdateWidgetRotation;

};
