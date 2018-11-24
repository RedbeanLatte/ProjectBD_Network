// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PCTitle.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API APCTitle : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
};