// Fill out your copyright notice in the Description page of Project Settings.


#include "ShotGun.h"

void AShotGun::FireBullet()
{
	for (int i = 0; i < NumberOfBullet; i++) // 한번에 발사할 총알 개수만큼 발사
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
