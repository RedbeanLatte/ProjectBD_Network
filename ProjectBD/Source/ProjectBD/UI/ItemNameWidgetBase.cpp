// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemNameWidgetBase.h"
#include "Components/TextBlock.h"

void UItemNameWidgetBase::NativeConstruct()
{
	//�������Ʈ�� ���� ���� �ν��Ͻ��� �θ��� ������ ������ ����
	ItemName = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemName")));
}