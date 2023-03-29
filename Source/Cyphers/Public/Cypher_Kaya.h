// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Creature.h"
#include "Cypher_Kaya.generated.h"

/**
 * 
 */
UCLASS()
class CYPHERS_API ACypher_Kaya : public ACreature
{
	GENERATED_BODY()
public:
	ACypher_Kaya();

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* aimMeshComp;

	UPROPERTY()
		class ACyphersGameModeBase* CyphersGameMode;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float maxHP;
	float currHP;

	bool bDamageState;
	void ReceiveDamage(int32 damage);

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* compSword;
	UPROPERTY()
	class APlayerCamera* Camera;
	FVector beforeActCameraPos;
	FVector afterActCameraPos;
	FRotator beforeActCameraRot;
	FRotator afterActCameraRot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
		class UChildActorComponent* CameraActorComponent;
	TSubclassOf<class APlayerCamera> cameraFactory;
	bool bCameraPosFix = false;
	FVector CameraOffset = FVector(-510, 0.f, 85.f);
	void DetachCameraActor();
	void AttachCameraActor();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* powerAttackColl;

public:
//만약 나중에 AI로도 Kaya를 만들경우 수정이 필요할수도 있음
	UPROPERTY(VisibleAnywhere, Category = Component)
	class UPlayerMoveInput* compPlayerMove;
	UPROPERTY(VisibleAnywhere, Category = Component)
		class UCypher_Kaya_Attack* compKayaAttack;
	/*UPROPERTY(EditAnywhere)
		class USpringArmComponent* compArm;*/
		UPROPERTY(EditAnywhere)
		class USceneComponent* camTarget;
	/*UPROPERTY(EditAnywhere)
		class UCameraComponent* compCam;*/

	UFUNCTION()
	void OnPowerAttackOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
		TSubclassOf<class APowerAttackDecal> decalFactory;
	UPROPERTY()
		class APowerAttackDecal* powerAttackDecal;
		UPROPERTY(EditAnywhere)
		class USceneComponent* footPos;
public:
	//카야 사운드
	UPROPERTY(EditAnywhere, Category = Sound)
	class USoundBase* damageSound;
	UPROPERTY(EditAnywhere, Category = Sound)
	class USoundBase* swing1;
	UPROPERTY(EditAnywhere, Category = Sound)
	class USoundBase* swing2;
	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* swing3;
	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* gaUpAtkSound;
	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* gaDownAtkSound;
	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* dashSound;

	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* powerAttackSwordReday;
	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* powerAttackStart;
	UPROPERTY(EditAnywhere, Category = Sound)
	class USoundBase* powerAttackEnd;
	UPROPERTY(EditAnywhere, Category = Sound)
	class USoundBase* powerAttackEndGroundSound;
	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* dashAttackSound;
	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* powerAttackComboSound;

	void PlayPowerAttackSwordReadySound();
	void PlayPowerAttackGroundSound();
	void PlayDashSound();
	void PlayGaUpAtkSound();
	void PlayGaDownAtkSound();
	UPROPERTY(EditAnywhere)
		class UParticleSystem* powerAttackEndEffect;

	void PlayDashAttackSound();
	void PlayDamageSound();

	UPROPERTY()
	class UNiagaraSystem* NiagaraSystemAsset;
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* compNiagra;

	UPROPERTY(EditAnywhere)
	float timer = 6;
	float currtimer;
public:
//카메라 흔들림
	bool bCameraShake = false;
	FVector cameraOriginPos;
	void CameraShakeRandom();
	UPROPERTY(EditAnywhere)
	float cameraShakeTime = 0.7;
	float camCurrTime;
};
