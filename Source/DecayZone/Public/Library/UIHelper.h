#pragma once

class UPanelWidget;
class UWidget;
class UUserWidget;

class UIHelper
{
public:
	static UWidget* GetChildByName(const UWidget* Widget, const FName& Name);
	static UWidget* SeekWidget(const UWidget* Widget, const FString& Path);
	static int32 GetZOrder(const UUserWidget* Widget);
	
	static void ShowTips(const UObject* WorldContextObject, const FText& TipText, float Duration = 3);
};
