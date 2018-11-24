// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGM.h"
#include "Lobby/LobbyGS.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"

void ALobbyGM::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(StartGameTimerHandle,
		this,
		&ALobbyGM::StartGameTimer,
		1.0f,
		true,
		0);
}

void ALobbyGM::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//UGameplayStatics::GetGameState()

	ALobbyGS* GS = GetGameState<ALobbyGS>();
	if (GS)
	{
		GS->UserCount++;

		//서버에서 UI Update
		//Gamemode는 서버에만 있고 소유는 서버가 함, 너 서버냐?
		if (HasAuthority())
		{
			GS->OnRep_UserCount();
		}
	}
}

void ALobbyGM::StartGameTimer()
{
	ALobbyGS* GS = GetGameState<ALobbyGS>();
	if (GS)
	{
		GS->LeftTime--;

		if (HasAuthority())
		{
			GS->OnRep_LeftTime();
		}

		if (GS->LeftTime == 0)
		{
			//게임 시작
		}
	}
}
