// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyWidgetBase.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Kismet/GameplayStatics.h"
#include "Lobby/LobbyPC.h"
#include "Local/BDGameInstance.h"

void ULobbyWidgetBase::NativeConstruct()
{
	PublicMessage = Cast<UTextBlock>(GetWidgetFromName("PublicMessage"));
	ConnectCount = Cast<UTextBlock>(GetWidgetFromName("ConnectCount"));
	StartGameButton = Cast<UButton>(GetWidgetFromName("StartGameButton"));
	ChatInput = Cast<UEditableTextBox>(GetWidgetFromName("ChatInput"));
	ChattingBox = Cast<UScrollBox>(GetWidgetFromName("ChattingBox"));

	if (StartGameButton)
	{
		StartGameButton->OnClicked.AddDynamic(this, &ULobbyWidgetBase::StartGame);
	}

	if (ChatInput)
	{
		ChatInput->OnTextCommitted.AddDynamic(this, &ULobbyWidgetBase::OnTextCommited);
	}
}

void ULobbyWidgetBase::OnTextCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	ALobbyPC* PC = Cast<ALobbyPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); //Get Local PC
	if (PC)
	{
		if (Text.IsEmpty())
		{
			//채팅 모드 끝
		}
		else
		{
			//서버로 메시지 전송, User ID가 필요
			auto GI = Cast<UBDGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			if (GI)
			{
				FString Message;
				Message = FString::Printf(TEXT("%s: %s"), *GI->UserID, *Text.ToString());
				//전송 Client to Server
				PC->C2S_SendChatMessage(FText::FromString(Message));
				ChatInput->SetText(FText::FromString(TEXT("")));
			}
		}
	}
	else if (CommitMethod == ETextCommit::OnCleared)
	{
		if (PC)
		{
			ChatInput->SetUserFocus(PC);
		}
	}
}

void ULobbyWidgetBase::StartGame()
{
	//UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Sample01")));
	GetWorld()->ServerTravel(TEXT("Battle"));
}

void ULobbyWidgetBase::AddChatMessage(const FText & Message)
{
	if (ChattingBox)
	{
		//Actor
		//GetWorld()->SpawnActor
		//Widget
		//CreateWidget
		UTextBlock* NewMessage = NewObject<UTextBlock>(ChattingBox);
		if (NewMessage)
		{
			NewMessage->SetText(Message);
			ChattingBox->AddChild(NewMessage);
			ChattingBox->ScrollToEnd();
		}
	}
}
