// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyWidgetBase.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Kismet/GameplayStatics.h"

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

}

void ULobbyWidgetBase::StartGame()
{
	//UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Sample01")));
	GetWorld()->ServerTravel(TEXT("Battle"));
}
