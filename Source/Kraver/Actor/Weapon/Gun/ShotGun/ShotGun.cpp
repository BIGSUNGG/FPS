// Fill out your copyright notice in the Description page of Project Settings.


#include "ShotGun.h"

void AShotGun::FireBullet()
{
	for (int i = 0; i < NumberOfBullet; i++)
	{
		AGun::FireBullet();
	}
}
