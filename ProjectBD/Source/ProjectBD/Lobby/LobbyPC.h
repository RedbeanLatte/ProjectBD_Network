// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPC.generated.h"

/**
 *
 */
UCLASS()
class PROJECTBD_API ALobbyPC : public APlayerController
{
	GENERATED_BODY()

public:
	ALobbyPC();

	virtual void BeginPlay() override;

	//소유중인 클라이언트에서 UI 생성
	UFUNCTION(Client, Reliable)
	void S2C_SetupWidget();
	void S2C_SetupWidget_Implementation();

	class UItemNameWidgetBase* ItemNameWidget;
	class UInventoryWidgetBase* InventoryWidget;

	void ShowItemName(FString& ItemName, bool Show = true);

	void ShowInventory();
	void UpdateSlotData();

	class ULobbyWidgetBase* LobbyWidget;

	//0. UMG에서 입력
	//1. 클라이언트에서 입력을 받아서 서버로 전송
	UFUNCTION(Server, Reliable, WithValidation)
	void C2S_SendChatMessage(const FText& Message);
	bool C2S_SendChatMessage_Validate(const FText& Message);
	void C2S_SendChatMessage_Implementation(const FText& Message);

	//3. 서버에서 클라이언트로 전송
	//4. UMG 자료 추가
	UFUNCTION(Client, Reliable, WithValidation)
	void S2C_AddChatMessage(const FText& Message);
	bool S2C_AddChatMessage_Validate(const FText& Message);
	void S2C_AddChatMessage_Implementation(const FText& Message);
};
