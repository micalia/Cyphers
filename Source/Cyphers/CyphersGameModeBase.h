// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CyphersGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class CYPHERS_API ACyphersGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ACyphersGameModeBase();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* earthquakeSound;
	UPROPERTY(EditAnywhere, Category = Sound)
		class USoundBase* battleSound;

	TSubclassOf<class UMainMenu> MainMenuWidgetClass;
	UPROPERTY()
		UMainMenu* MainMenuWidget;

	TSubclassOf<class UAimUI> AimUIClass;
	UPROPERTY()
		UAimUI* aimUIWidget;

	TSubclassOf<class UPlayerWidget> PlayerWidgetClass;
	UPROPERTY()
		UPlayerWidget* playerWidget;

	TSubclassOf<class UWhiteScreen> WhiteScreenClass;
	UPROPERTY()
		UWhiteScreen* whiteScreen;

	UPROPERTY()
		class ACypher_Kaya* kaya;

		void NextStage();

	void HideUI();
	void ShowUI();

	void ShowMouseCursor();
	void HideMouseCursor();

	void CameraShakeRandom();

	bool bCameraShake;
	float camCurrTime;
	UPROPERTY(EditAnywhere)
		float cameraShakeTime = 3.5;
//화면 하얘지는 효과
	UPROPERTY(EditAnywhere)
		float renderOpacityTime = 2;
		float renderCurrTime;
	void WhiteScreenOpacityActive(bool OpacityOn);
	bool OpacityOnCheck;
	bool ActiveWhiteScreenOpacity;
//전투브금
	UPROPERTY(EditAnywhere)
		float bgmVolume = 0.5;
	UPROPERTY()
		UAudioComponent* battleAudioComp;
	UPROPERTY()
	USoundClass* soundClass;
	void PlayBattleMusic();
	void FadeOutBattleMusic();
	UPROPERTY(EditAnywhere, Category = Sound)
		float FadeOutDuration = 1.5;
		float FadeOutCurrTime;

		bool bFadingOut;
		float currVolume;

//돌가루 효과
	TArray<AActor*> FoundStonePowderActor;
	TArray<AActor*> FoundStoneSmokeActor;


};
