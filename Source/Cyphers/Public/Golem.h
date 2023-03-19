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
		//타겟
	UPROPERTY()
		class ACypher_Kaya* mainPlayer;
//보스 등장
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AppearBoss)
		bool bossAppear = false;
//현재상태
	UPROPERTY(EditAnywhere)
		float currAttackDamage = 0;
//점프공격
	//베지어곡선
	FVector startPos;
	FVector originEndPos;
	FVector endPos;
	TArray<FVector> lineLoc;
	//베지어 도착지점 z축
	UPROPERTY(EditAnywhere, Category = Bezier)
		float endPosSub = -40;
	UPROPERTY(EditAnywhere, Category = Bezier)
		float curvePointCount = 60;
	FVector CalculateBezier(float ratio, FVector startPos, FVector betweenPos, FVector endPos);
	//베지어 오브젝트 경로 만들기
	void JumpAttackPath(FVector startPos, FVector betweenPos, FVector endPos);
	void MoveJumpAttack();
//돌던지기
	UPROPERTY(EditAnywhere)
		class UMaterialParameterCollection* stoneOpacity;
};
