// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"


AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Scene Root"));

}


void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;

	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy != EEffectRemovalPolicy::DoNotRemove)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}

}

void AAuraEffectActor::ApplyEffectsToTarget(AActor* TargetActor, TArray<TSubclassOf<UGameplayEffect>> GameplayEffectClasses)
{
	for (auto& Effect : GameplayEffectClasses)
	{
		ApplyEffectToTarget(TargetActor, Effect);
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	// Instant Gameplay Effect Applications
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if (InstantEffectsApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectsToTarget(TargetActor, InstantGameplayEffectClasses);
	}

	// Duration Gameplay Effect Applications
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if (DurationEffectsApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectsToTarget(TargetActor, DurationGameplayEffectClasses);
	}

	// Infinite Gameplay Effect Applications
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	if (InfiniteEffectsApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectsToTarget(TargetActor, InfiniteGameplayEffectClasses);
	}

	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (auto HandlePair : ActiveEffectHandles)
		{
			if (TargetASC == HandlePair.Value)
			{
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}

		for (auto& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	// Instant Gameplay Effect Applications
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if (InstantEffectsApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectsToTarget(TargetActor, InstantGameplayEffectClasses);
	}

	// Duration Gameplay Effect Applications
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if (DurationEffectsApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectsToTarget(TargetActor, DurationGameplayEffectClasses);
	}

	// Infinite Gameplay Effect Applications
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	if (InfiniteEffectsApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectsToTarget(TargetActor, InfiniteGameplayEffectClasses);
	}

	// Infinite Removal on End Overlap
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (auto HandlePair : ActiveEffectHandles)
		{
			if (TargetASC == HandlePair.Value)
			{
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}

		for (auto& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}
