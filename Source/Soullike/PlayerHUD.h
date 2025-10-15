// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//���� ������ WBP���� ����
	//Player HUD HP ���� �Լ�
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Bar")
	void SetHp(float CurrentHp, float MaxHp);

	//Player HUD EP ���� �Լ�
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Bar")
	void SetEp(float CurrentEp, float MaxEp);

};
