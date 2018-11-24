// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGS.h"
#include "UnrealNetwork.h" // Network"
#include "Lobby/LobbyWidgetBase.h"
#include "Kismet/GameplayStatics.h"
#include "Lobby/LobbyPC.h"
#include "Components/TextBlock.h"

ALobbyGS::ALobbyGS()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALobbyGS::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ALobbyGS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyGS, UserCount);
	DOREPLIFETIME(ALobbyGS, LeftTime);
}

void ALobbyGS::OnRep_UserCount()
{
	ALobbyPC* PC = Cast<ALobbyPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		if (PC->LobbyWidget)
		{
			FString Temp = FString::Printf(TEXT("%d명 연결"), UserCount);
			PC->LobbyWidget->ConnectCount->SetText(FText::FromString(Temp));
		}
	}
}

void ALobbyGS::OnRep_LeftTime()
{
	ALobbyPC* PC = Cast<ALobbyPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		if (PC->LobbyWidget)
		{
			FString Temp = FString::Printf(TEXT("%d초후에 게임을 시작합니다."), LeftTime);
			PC->LobbyWidget->PublicMessage->SetText(FText::FromString(Temp));
		}
	}
}
