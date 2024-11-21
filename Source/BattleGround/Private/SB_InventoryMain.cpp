// Fill out your copyright notice in the Description page of Project Settings.


#include "SB_InventoryMain.h"
#include "Components/ScrollBox.h"
#include "SB_HelmetSlot.h"
#include "../BattleGroundCharacter.h"
#include "SB_ItemSlotWidget.h"
#include "SB_GroundSlotWidget.h"
#include "Kismet/GameplayStatics.h"

USB_InventoryMain::USB_InventoryMain(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	/*ConstructorHelpers::FClassFinder<USB_ItemSlotWidget> tempItemSlot(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UMG/ItemSlot.ItemSlot_C'"));
	if (tempItemSlot.Succeeded()) {
		ItemSlotFactory = tempItemSlot.Class;
	}*/
}

void USB_InventoryMain::NativeConstruct()
{
	Super::NativeConstruct();

	
	//BuildInventory();
}

void USB_InventoryMain::BuildInventory()
{
	if (Player == nullptr) {
		Player = Cast<ABattleGroundCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}
	ItemScrollbox->ClearChildren();
	for (int i = 0; i < Player->ItemArr.Num(); i++) {
		switch (Player->ItemArr[i].Category)
		{
		case EItemEnum::Weapon:
			break;
		case EItemEnum::Helmet:
			UseHelmetSlot->ItemData = Player->ItemArr[i];
			UseHelmetSlot->TPRef = Player;
			UseHelmetSlot->Index = i;
			break;
		case EItemEnum::Consumeables:
			UClass* WidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UMG/ItemSlot.ItemSlot_C'"));
			USB_ItemSlotWidget* ItemSlot = CreateWidget<USB_ItemSlotWidget>(GetWorld(), WidgetClass);
			ItemSlot->ItemData = Player->ItemArr[i];
			ItemSlot->Index = i;
			ItemSlot->TPC = Player;
			ItemScrollbox->AddChild(Cast<UWidget>(ItemSlot));
			break;
		}
	}
}

void USB_InventoryMain::BuildGroundItems()
{
	if (Player == nullptr) {
		Player = Cast<ABattleGroundCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}
	GroundItems->ClearChildren();
	for (auto& Item : Player->MultiItemRefs) {
		FItemData NewItemData;
		NewItemData = Item->ItemData;
		UClass* WidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UMG/GroundSlot.GroundSlot_C'"));
		USB_GroundSlotWidget* GroundSlot = CreateWidget<USB_GroundSlotWidget>(GetWorld(), WidgetClass);
		GroundSlot->TPCRef = Player;
		GroundSlot->ItemData = NewItemData;
		GroundItems->AddChild(Cast<UWidget>(GroundSlot));
	}
}
