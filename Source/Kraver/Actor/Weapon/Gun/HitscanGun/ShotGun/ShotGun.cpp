// Fill out your copyright notice in the Description page of Project Settings.


#include "ShotGun.h"

void AShotGun::FireBullet()
{
	for (int i = 0; i < NumberOfBullet; i++) // �ѹ��� �߻��� �Ѿ� ������ŭ �߻�
	{
		Super::FireBullet();
	}
}

float AShotGun::CalculateCurSpread()
{
	float Result = CurBulletSpread;
	if (!bIsSubAttacking)
	{
		Result += AdditiveSpreadInAir + AdditiveSpreadPerSpeed;
	}

	return Result;
}
