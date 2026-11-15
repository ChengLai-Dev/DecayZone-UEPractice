// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UIManager.generated.h"

class UDZ_UserWidget;
class UDZ_UIControllerBase;
class APlayerController;

UCLASS()
class DECAYZONE_API UUIManager : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize(APlayerController* InPlayerController);
	void Shutdown();
	void TickControllers(float DeltaSeconds);
	
	APlayerController* GetOwningPlayerController() const { return PlayerController.Get(); }
	bool IsInitialized() const { return Controllers.Num() > 0; }
	
	void OpenWidget(UDZ_UserWidget* Widget, bool bResident=false);
	void CloseWidget(UDZ_UserWidget* Widget);
	virtual void CloseTopWidget();
	void UpdateInputMode();
	
protected:
	virtual void CreateControllers();
	virtual void DestroyControllers();
	
	template<typename T>
	T* CreateController()
	{
		T* Controller = NewObject<T>(this);
		Controller->Initialize(PlayerController.Get());
		Controllers.Add(Controller);
		return Controller;
	}
	
	UPROPERTY()
	TArray<TObjectPtr<UDZ_UIControllerBase>> Controllers;
	
private:
	TWeakObjectPtr<APlayerController> PlayerController;
	
	TArray<TWeakObjectPtr<UDZ_UserWidget>> CurrentActivateWidgets;
};
