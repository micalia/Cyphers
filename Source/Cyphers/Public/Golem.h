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
		float curvePointCount = 60;
	FVector CalculateBezier(float ratio, FVector startPos, FVector betweenPos, FVector endPos);
	//������ ������Ʈ ��� �����
	void JumpAttackPath(FVector startPos, FVector betweenPos, FVector endPos);
	void MoveJumpAttack();
//��������
	UPROPERTY(EditAnywhere)
		class UMaterialParameterCollection* stoneOpacity;
};
