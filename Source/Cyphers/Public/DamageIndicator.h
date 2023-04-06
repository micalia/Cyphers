// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageIndicator.generated.h"

/**
 * 
 */
UCLASS()
class CYPHERS_API UDamageIndicator : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
		class UTextBlock* DamageTxt = nullptr;
	UPROPERTY(BlueprintReadWrite)
	bool bCritical;
		//void ApplyDamage(FText Damage);
};
