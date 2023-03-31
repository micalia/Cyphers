// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossAppearBox.generated.h"

UCLASS()
class CYPHERS_API ABossAppearBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossAppearBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY()
	class ACyphersGameModeBase* gameMode;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	FVector boxDefaultSize = FVector(100);

	UPROPERTY(EditAnywhere)
		class UBoxComponent* collisionBox;
	UPROPERTY()
		class AGolem* enemy;
	UPROPERTY()
		class ACypher_Kaya* player;

public:	
	bool bStart;
	FTransform cameraOrigin;
	FTransform MoveDest;
	float alpha = 0;
	float backAlpha = 0;
	float currTime = 0;
	UPROPERTY(EditAnywhere)
		float moveTime = 2.5;
	UPROPERTY(EditAnywhere)
		float roarTime = 5.4;
	float roarCurrTime = 0;
	float BackMoveCurrTime = 0;
	FTransform cameraRelative;
	FTransform cameraRelativeDest;
	bool bAnimOn = false;
	float BackMoveTime = 1;

	UPROPERTY(EditAnywhere)
	float cameraFocusHeight = -120;
	UPROPERTY(EditAnywhere)
	float cameraFocusDistanceToBoss = 2700;
public:
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);
};
