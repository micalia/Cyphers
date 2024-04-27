#include "PlayerBaseComponent.h"
#include "Cypher_Kaya.h"

UPlayerBaseComponent::UPlayerBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	me = Cast<ACreature>(GetOwner());
	if (me != nullptr)
	{
		moveComp = me->GetCharacterMovement();
	}	
}

void UPlayerBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlayerBaseComponent::SetupInputBinding(class UInputComponent* PlayerInputComponent)
{

}

