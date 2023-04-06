// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Creature.h"
#include "Golem.generated.h"

/**
 *
 */
UCLASS()
class CYPHERS_API AGolem : public ACreature
{
	GENERATED_BODY()
public:
	AGolem();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

//������ ui
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UWidgetComponent* damageIndicatorWidget;
	UFUNCTION(BlueprintImplementableEvent)
		void DamageNumber(float Damage, FVector hitLocation, bool critical);
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UDamageIndicator> DamageIndicatorFactory;
	UPROPERTY()
		class UDamageIndicator* damageIndicator;
/////////////
	float maxHP;
	float currHP;

	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* footSound;
	void PlayFootSound();

	void ReceiveDamage(FVector hitLocation);
	UPROPERTY()
		class ACyphersGameModeBase* CyphersGameMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FSMComponent)
		class UGolemFSM* fsm;
	UPROPERTY(EditAnywhere, Category = ThrowStoneAttack)
		TSubclassOf<class AStoneObj> stoneFactory;
	//Ÿ��
	UPROPERTY()
		class ACypher_Kaya* mainPlayer;

	//���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AppearBoss)
		bool bossAppear = false;
	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* bossRoar;
	//�������
	UPROPERTY(EditAnywhere)
		float currAttackDamage = 0;
	//��������
		//������
	FVector startPos;
	FVector originEndPos;
	FVector endPos;
	TArray<FVector> lineLoc;
	//������ �������� z��
	UPROPERTY(EditAnywhere, Category = Bezier)
		float endPosSub = -40;
	UPROPERTY(EditAnywhere, Category = Bezier)
		float betweenHeight = 2500;
	UPROPERTY(EditAnywhere, Category = Bezier)
		float curvePointCount = 7;
	FVector CalculateBezier(float ratio, FVector startPos, FVector betweenPos, FVector endPos);
	//������ ������Ʈ ��� �����
	void JumpAttackPath(FVector startPos, FVector betweenPos, FVector endPos);
	void MoveJumpAttack();

	UPROPERTY(EditAnywhere)
		class UParticleSystem* jumpAttackEffect;
	UPROPERTY(EditAnywhere)
		class USceneComponent* JA_EffectPoint;

	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* jumpAttackSound;
	void PlayJumpAttackSound();
	//��������
	UPROPERTY(EditAnywhere)
		class UMaterialParameterCollection* stoneOpacity;

	//�׶��� ����
	TSubclassOf<class AGolemGroundAttackCollision> GA_CollisionFactory;
	UPROPERTY(EditAnywhere)
		class USceneComponent* groundAttackPoint;

	UPROPERTY(EditAnywhere)
		class UParticleSystem* groundAttackEffect;
	void SpawnGroundAttackCollision(FRotator dir);
	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* groundAttackSound;
	void PlayGroundAttackSound();

public:
	//�˴ٿ� ����
	UPROPERTY(EditAnywhere)
		class UParticleSystem* knockDownAttackEffect;
	UPROPERTY(EditAnywhere)
	FVector KD_atk_effect_size = FVector(2.4);
	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* KD_Sound;

	//����
	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* dieSound1;
	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* dieSound2;

		UPROPERTY()
		class AEndPoint* endPoint;
};
