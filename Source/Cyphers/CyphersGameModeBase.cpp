// Copyright Epic Games, Inc. All Rights Reserved.


#include "CyphersGameModeBase.h"
#include "AimUI.h"
#include "PlayerWidget.h"
#include "MainMenu.h"
#include "Cypher_Kaya.h"
#include <Kismet/KismetMathLibrary.h>
#include <UMG/Public/Components/Image.h>
#include "WhiteScreen.h"
#include <Kismet/GameplayStatics.h>
#include <Components/AudioComponent.h>
#include "BlockStone.h"
#include "SpawnFallingStonePowder.h"
#include "StoneMoveSmoke.h"
#include <Particles/ParticleSystemComponent.h>

ACyphersGameModeBase::ACyphersGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Crosshair Widget Class를 설정합니다.
	static ConstructorHelpers::FClassFinder<UMainMenu> tempMainMenuWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/Widget/WB_MainMenu.WB_MainMenu_C'"));
	if (tempMainMenuWidgetClass.Succeeded())
	{
		MainMenuWidgetClass = tempMainMenuWidgetClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UAimUI> tempAimUIclass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/Widget/WB_AimUI.WB_AimUI_C'"));
	if (tempAimUIclass.Succeeded())
	{
		AimUIClass = tempAimUIclass.Class;
	}

	static ConstructorHelpers::FClassFinder<UPlayerWidget> tempPlayerWidgetclass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/Widget/BP_PlayerWidget.BP_PlayerWidget_C'"));
	if (tempPlayerWidgetclass.Succeeded())
	{
		PlayerWidgetClass = tempPlayerWidgetclass.Class;
	}

	static ConstructorHelpers::FClassFinder<UWhiteScreen> tempWhiteScreeClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/Widget/WB_WhiteScreen.WB_WhiteScreen_C'"));
	if (tempWhiteScreeClass.Succeeded())
	{
		WhiteScreenClass = tempWhiteScreeClass.Class;
	}


	static ConstructorHelpers::FObjectFinder<USoundBase> tempEarthquakeSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/earthquake_sound.earthquake_sound'"));
	if (tempEarthquakeSound.Succeeded()) {
		earthquakeSound = tempEarthquakeSound.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> tempBattleSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/BattleSound.BattleSound'"));
	if (tempBattleSound.Succeeded()) {
		battleSound = tempBattleSound.Object;
	}
	
}

void ACyphersGameModeBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Crosshair Widget 생성
	if (MainMenuWidgetClass != nullptr)
	{
		MainMenuWidget = CreateWidget<UMainMenu>(GetWorld(), MainMenuWidgetClass);

		if (MainMenuWidget != nullptr)
		{
			MainMenuWidget->AddToViewport();
		}
	}

	if (AimUIClass != nullptr)
	{
		aimUIWidget = CreateWidget<UAimUI>(GetWorld(), AimUIClass);

		if (aimUIWidget != nullptr)
		{
			aimUIWidget->AddToViewport();
		}
	}

	if (PlayerWidgetClass != nullptr)
	{
		playerWidget = CreateWidget<UPlayerWidget>(GetWorld(), PlayerWidgetClass);

		if (playerWidget != nullptr)
		{
			playerWidget->AddToViewport();
		}
	}

	if (WhiteScreenClass != nullptr)
	{
		whiteScreen = CreateWidget<UWhiteScreen>(GetWorld(), WhiteScreenClass);

		if (whiteScreen != nullptr)
		{
			whiteScreen->AddToViewport();
		}
	}

	HideUI();
}

void ACyphersGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	ShowMouseCursor();
	
	kaya = Cast<ACypher_Kaya>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (kaya != nullptr) {
		kaya->bCameraPosFix = true;

		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		kaya->DisableInput(controller);

	}
	
	ActiveWhiteScreenOpacity = true;
	OpacityOnCheck = false;
	
	//돌 효과 액터를 가져온다
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnFallingStonePowder::StaticClass(), FoundStonePowderActor);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStoneMoveSmoke::StaticClass(), FoundStoneSmokeActor);
}

void ACyphersGameModeBase::Tick(float DeltaSeconds)
{
	if (bCameraShake == true)CameraShakeRandom();
	if(ActiveWhiteScreenOpacity == true) WhiteScreenOpacityActive(OpacityOnCheck);
	if(bFadingOut)FadeOutBattleMusic();
}

void ACyphersGameModeBase::NextStage()
{
	ABlockStone* blockStone = Cast<ABlockStone>(UGameplayStatics::GetActorOfClass(GetWorld(), ABlockStone::StaticClass()));
	blockStone->bNextStage = true;

	bCameraShake = true;

	UGameplayStatics::PlaySound2D(GetWorld(), earthquakeSound);

	for (int32 i=0; i < FoundStonePowderActor.Num(); i++)
	{
		ASpawnFallingStonePowder* stonePowderEffect = Cast<ASpawnFallingStonePowder>(FoundStonePowderActor[i]);
		stonePowderEffect->ExecuteStonePowderEffect();
		stonePowderEffect->bIsLoop = true;
	}

	for (int32 i = 0; i < FoundStoneSmokeActor.Num(); i++)
	{ 
		UE_LOG(LogTemp, Warning, TEXT("FoundStoneSmokeActor Name : %s"), *FoundStoneSmokeActor[i]->GetName())
		AStoneMoveSmoke* stoneSmokeEffect = Cast<AStoneMoveSmoke>(FoundStoneSmokeActor[i]);
		stoneSmokeEffect->vfxComp->SetActive(true);
	}
	
}

void ACyphersGameModeBase::HideUI()
{
	playerWidget->SetVisibility(ESlateVisibility::Hidden);
	aimUIWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ACyphersGameModeBase::ShowUI() {
	playerWidget->SetVisibility(ESlateVisibility::Visible);
	aimUIWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACyphersGameModeBase::ShowMouseCursor()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = true;
	PlayerController->bEnableClickEvents = true;
	PlayerController->bEnableMouseOverEvents = true;
}

void ACyphersGameModeBase::HideMouseCursor()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = false;
	PlayerController->bEnableClickEvents = false;
	PlayerController->bEnableMouseOverEvents = false;
}

void ACyphersGameModeBase::CameraShakeRandom()
{
	if (camCurrTime < cameraShakeTime) {
		camCurrTime += GetWorld()->GetDeltaSeconds();
		float y = UKismetMathLibrary::RandomFloatInRange(-4, 4);
		float z = UKismetMathLibrary::RandomFloatInRange(-4, 4);
		kaya->CameraActorComponent->SetRelativeLocation(kaya->cameraOriginPos + FVector(0, y, z));
	}
	else {
		bCameraShake = false;
		kaya->CameraActorComponent->SetRelativeLocation(kaya->cameraOriginPos);
		camCurrTime = 0;

		//돌가루 효과 반복 종료
		for (int32 i = 0; i < FoundStonePowderActor.Num(); i++)
		{
			ASpawnFallingStonePowder* stonePowderEffect = Cast<ASpawnFallingStonePowder>(FoundStonePowderActor[i]);
			stonePowderEffect->bIsLoop = false;
		}

		//흙먼지 날림 연출 효과
		for (int32 i = 0; i < FoundStoneSmokeActor.Num(); i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("FoundStoneSmokeActor Name : %s"), *FoundStoneSmokeActor[i]->GetName())
			AStoneMoveSmoke* stoneSmokeEffect = Cast<AStoneMoveSmoke>(FoundStoneSmokeActor[i]);
			stoneSmokeEffect->vfxComp->SetActive(false);
		}
	}
}

void ACyphersGameModeBase::WhiteScreenOpacityActive(bool OpacityOn)
{
	renderCurrTime += GetWorld()->GetDeltaSeconds();
	if (OpacityOn) {
		if (renderCurrTime < renderOpacityTime+1.5) {
			whiteScreen->WhiteScreen->SetOpacity(renderCurrTime / renderOpacityTime);
		}
		else {
		//게임이 끝났을때화면이 점점 하얘지면서 다시 씬을 재로딩하기 됨
			renderCurrTime = renderOpacityTime;
			UGameplayStatics::OpenLevel(GetWorld(), TEXT("InGame"));
		}
	}
	else { 
		if (renderCurrTime < renderOpacityTime) {
			float opacity = 1 - renderCurrTime / renderOpacityTime;
			whiteScreen->WhiteScreen->SetOpacity(opacity);
		}
		else {
			renderCurrTime = 0;
			ActiveWhiteScreenOpacity = false;
			whiteScreen->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void ACyphersGameModeBase::PlayBattleMusic()
{
	battleAudioComp = UGameplayStatics::SpawnSound2D(GetWorld(), battleSound);
	battleAudioComp->SetVolumeMultiplier(bgmVolume);
	currVolume = battleAudioComp->VolumeMultiplier;
	/*currVolume = battleSound->GetVolumeMultiplier();
	soundClass = battleSound->GetSoundClass();*/

	UE_LOG(LogTemp, Warning, TEXT("currVolume: %f"), battleAudioComp->VolumeMultiplier)
}

void ACyphersGameModeBase::FadeOutBattleMusic()
{
	FadeOutCurrTime+= GetWorld()->GetDeltaSeconds();
	if (battleAudioComp != nullptr) {
		battleAudioComp->SetVolumeMultiplier(FMath::Lerp(currVolume, 0, FadeOutCurrTime / FadeOutDuration));
		if (battleAudioComp->VolumeMultiplier < 0) {
			battleAudioComp->SetVolumeMultiplier(0);
			battleAudioComp->Stop();
		}/*
		soundClass->Properties.Volume = FMath::Lerp(currVolume, 0, FadeOutCurrTime / FadeOutDuration);
		if (soundClass->Properties.Volume < 0) {
			soundClass->
			Properties.Volume = 0;
		}*/
		UE_LOG(LogTemp, Warning, TEXT("SoundVolume : %f"), battleAudioComp->VolumeMultiplier)
	}
}

void ACyphersGameModeBase::CheckKillCount()
{
	killCount++;
	if (TargetNumberOfKill <= killCount) {
		NextStage();
	}
}
