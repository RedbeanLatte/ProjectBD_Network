// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API ULobbyWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	class UTextBlock* PublicMessage;
	class UTextBlock* ConnectCount;

	class UButton* StartGameButton;
	class UScrollBox* ChattingBox;
	class UEditableTextBox* ChatInput;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnTextCommited(const FText & Text, ETextCommit::Type CommitMethod);
	
	UFUNCTION()
	void StartGame();


	void AddChatMessage(const FText& Message);
};
