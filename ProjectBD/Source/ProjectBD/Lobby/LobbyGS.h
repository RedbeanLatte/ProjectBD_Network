// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyGS.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API ALobbyGS : public AGameStateBase
{
	GENERATED_BODY()
public:
	//UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Replicated)
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, ReplicatedUsing = "OnRep_UserCount")
	int UserCount = 0;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, ReplicatedUsing = "OnRep_LeftTime")
	int LeftTime = 60;

	ALobbyGS();

	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_UserCount();

	UFUNCTION()
	void OnRep_LeftTime();
};
