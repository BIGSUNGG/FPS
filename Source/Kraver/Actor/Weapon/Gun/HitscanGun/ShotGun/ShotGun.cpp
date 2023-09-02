// Fill out your copyright notice in the Description page of Project Settings.


#include "ShotGun.h"

bool AShotGun::RefillAmmo()
{
	if (CurAmmo == MaxAmmo)
		return false;

	++CurAmmo;
	--TotalAmmo;

	if (!HasAuthority())
	{
		Server_SetCurAmmo(CurAmmo);
		Server_SetTotalAmmo(TotalAmmo);
	}
	return true;
}

void AShotGun::FireBullet()
{
	for (int i = 0; i < NumberOfBullet; i++)
	{
		Super::FireBullet();
	}
}

float AShotGun::CalculateCurSpread()
{
	float Result = CurBulletSpread;
	if (!IsSubAttacking)
	{
		Result += AdditiveSpreadInAir + AdditiveSpreadPerSpeed;
	}

	return Result;
}
