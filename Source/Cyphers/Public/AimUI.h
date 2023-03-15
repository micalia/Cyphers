// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AimUI.generated.h"

/**
 * 
 */
UCLASS()
class CYPHERS_API UAimUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	UPROPERTY(meta = (BindWidget))
	class UImage* AimImage;
	UPROPERTY(EditAnywhere)
		float RotationSpeed = 120;
		float CurrentRotate;

	UFUNCTION(BlueprintCallable, Category = "YourClass")
		void MoveAimUI(float DeltaTime, float MoveDistance);
};
