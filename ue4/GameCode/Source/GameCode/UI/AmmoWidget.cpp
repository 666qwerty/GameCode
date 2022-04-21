// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AmmoWidget.h"

void UAmmoWidget::UpdateAmmoCount(int32 AmmoCount, int32 TotalAmmoCount, int32 GrenadeCount)
{
	Ammo = AmmoCount;
	TotalAmmo = TotalAmmoCount;
	Grenade = GrenadeCount;
}
