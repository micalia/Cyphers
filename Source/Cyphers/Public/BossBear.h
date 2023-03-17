// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Creature.h"
#include "BossBear.generated.h"

/**
 * 
 */
UCLASS()
class CYPHERS_API ABossBear : public ACreature
{
	GENERATED_BODY()
//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AppearBoss)
//		bool bossAppear = false;
//	UPROPERTY(EditDefaultsOnly, Category = AppearBoss)
//		class USoundBase* roarSound;
//
//	UPROPERTY(EditDefaultsOnly, Category = CameraMotion)
//		TSubclassOf<class UCameraShakeBase> cameraShake;
//	void BossAppearCameraShake();
//	void DoDamage(AActor* Victim);
//
//	void StunCharacter(class ACPP_GunWoman* Victim);
//public:
//	UPROPERTY(BlueprintReadWrite)
//		float FullHp;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float hp = 3000;
//	UPROPERTY(EditAnywhere)
//		float currAttackDamage = 0;
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
//
//	// Called to bind functionality to input
//	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
//public:
//	UPROPERTY(EditAnywhere, Category = Bezier)
//		float endPosSub = -40;
//
//	FVector startPos;
//	FVector originEndPos;
//	FVector endPos;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FSMComponent)
//		class UBossFsm* fsm;
//
//	UPROPERTY(EditAnywhere, Category = ThrowStoneAttack)
//		TSubclassOf<class AStoneObj> stoneFactory;
//	UPROPERTY(EditAnywhere)
//		class UMaterialParameterCollection* stoneOpacity;
//
//	UPROPERTY(EditAnywhere)
//		class USphereComponent* headCollision;
//
//	UPROPERTY(EditAnywhere)
//		class UBoxComponent* leftColliderBox;
//	UPROPERTY(EditAnywhere)
//		class UBoxComponent* rightColliderBox;
//
//	/*UPROPERTY(EditAnywhere)
//		class USceneComponent* EnemyFootPos;*/
//
//		/*UPROPERTY()
//		class UArrowComponent* EnemyFootPosArrow;*/
//
//	void MoveJumpAttack();
//
//	UPROPERTY()
//		class ACPP_GunWoman* mainPlayer;
//	UPROPERTY(EditAnywhere, Category = Bezier)
//		float spawnSphere = 38;
//
//	void JumpAttackPath(FVector startPos, FVector betweenPos, FVector endPos);
//
//	/*UPROPERTY(EditAnywhere, Category = Bezier)
//	TArray<AActor*> points;*/
//
//	FVector CalculateBezier(float ratio, FVector startPos, FVector betweenPos, FVector endPos);
//
//	UPROPERTY(EditAnywhere, Category = Bezier)
//		TSubclassOf<class ABezierPathObj> pathObj;
//
//	UFUNCTION()
//		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
//			AActor* OtherActor,
//			UPrimitiveComponent* OtherComp,
//			int32 OtherBodyIndex,
//			bool bFromSweep,
//			const FHitResult& SweepResult);
//public:
//
//	/*class UAnimInstance* animInstance;
//	virtual void PostInitializeComponents() override;*/
//	UPROPERTY(EditDefaultsOnly, Category = Sound)
//		class USoundBase* punchSound;
//
//	UPROPERTY(EditAnywhere, Category = Effect)
//		class UParticleSystem* punchEffect;
//
//	UPROPERTY(VisibleAnywhere)
//		class UBossAttack* bossAttack;
//
//
//public:
//	class APlayerCameraManager* camera;
//	UPROPERTY(EditAnywhere)
//		class UCameraComponent* cam;
//	bool bCameraShake = false;
//	FVector cameraOriginPos;
//	UPROPERTY(EditAnywhere)
//		float cameraShakeTime = 0.5;
//	float camCurrTime = 0;
//	void CameraShakeRandom();
//
//	/*bool startChk= false;
//	FVector startShakeOriginPos;
//	bool bStartShake = false;
//	void StartCameraShake();*/
//public:
//	//hit UI
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
//		FString HeadBone = "Head";
//	FORCEINLINE FString GetHeadBone() const { return HeadBone; }
//	UFUNCTION()
//		void DestroyHitNumber(UUserWidget* HitNumber);
//	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
//		TMap<UUserWidget*, FVector>HitNumbers;
//	UFUNCTION(BlueprintImplementableEvent)
//		void ShowHitNumber(int32 Damage, FVector HitLocation, bool bHeadShot);
//
//	UFUNCTION(BlueprintCallable)
//		void StoreHitNumber(UUserWidget* HitNumber, FVector Location);
//	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
//		float HitNumberDestroyTime;
//
//	UPROPERTY(EditAnywhere)
//		TSubclassOf<class UBossWidget> bossUI;
//	class UBossWidget* bossHpBar;
//	UFUNCTION(BlueprintCallable, Category = "UI")
//		void ShowBossHPbar();
};
