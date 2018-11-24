// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyPC.h"
#include "Local/MyPlayerCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "UI/ItemNameWidgetBase.h"
#include "UI/InventoryWidgetBase.h"
#include "Components/TextBlock.h"
#include "UI/InventoryWidgetBase.h"
#include "UI/ItemSlotWidgetBase.h"
#include "Lobby/LobbyWidgetBase.h"
#include "Local/BDGameInstance.h"
#include "Kismet/GameplayStatics.h"

ALobbyPC::ALobbyPC()
{
	PlayerCameraManagerClass = AMyPlayerCameraManager::StaticClass(); // COD 에 있는 Class를 참조해서 등록한 클래스를 생성
}

void ALobbyPC::BeginPlay()
{
	Super::BeginPlay();

	S2C_SetupWidget();
}

void ALobbyPC::S2C_SetupWidget_Implementation()
{
	//Lobby 위젯 생성
	FStringClassReference LobbyWidgetClass(TEXT("WidgetBlueprint'/Game/Blueprints/Lobby/UI/LobbyWidget.LobbyWidget_C'"));

	if (UClass* MyWidgetClass = LobbyWidgetClass.TryLoadClass<UUserWidget>())
	{
		LobbyWidget = Cast<ULobbyWidgetBase>(CreateWidget<UUserWidget>(this, MyWidgetClass));
		LobbyWidget->AddToViewport();
		SetInputMode(FInputModeGameAndUI());
		bShowMouseCursor = true;
	}
}

void ALobbyPC::ShowItemName(FString & ItemName, bool Show)
{
	ItemNameWidget->ItemName->SetText(FText::FromString(ItemName));
	if (Show)
	{
		ItemNameWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ItemNameWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ALobbyPC::ShowInventory()
{
	if (InventoryWidget->GetVisibility() == ESlateVisibility::Collapsed)
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
		bShowMouseCursor = true;
		SetInputMode(FInputModeGameAndUI());
	}
	else
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
	}
}

void ALobbyPC::UpdateSlotData()
{
	InventoryWidget->HideAllSlot();

	UBDGameInstance* GI = Cast<UBDGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI)
	{
		//for (int i = 0; i < GI->Inventory.Num(); i++)
		int i = 0;
		for (auto ItemData : GI->Inventory)
		{
			UItemSlotWidgetBase* Slot = InventoryWidget->GetEmptySlot();
			if (Slot)
			{
				Slot->SetItemData(ItemData);
				Slot->InventoryIndex = i;
			}
			i++;
			//빈 UI 슬롯이 없으면 아이템 그리지 않음
		}
	}
}
