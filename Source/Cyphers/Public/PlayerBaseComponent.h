// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CYPHERS_API UPlayerBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerBaseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetupInputBinding(class UInputComponent* PlayerInputComponent);
public:
//다른 캐릭터도 이 컴포넌트를 쓴다면 나중에 수정 필요할 수 있음
	UPROPERTY()
	class ACypher_Kaya* me;

	UPROPERTY()
	class UCharacterMovementComponent* moveComp;
};
