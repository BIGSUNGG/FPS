// Fill out your copyright notice in the Description page of Project Settings.


#include "LedgeGrabCameraShake.h"

ULedgeGrabCameraShake::ULedgeGrabCameraShake()
{
	OscillationDuration = 0.5f;
	RotOscillation.Roll.Amplitude = -25.f;
	FOVOscillation.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	LocOscillation.Z.Amplitude = 100.f;
	LocOscillation.Z.Frequency = 1.f;
	LocOscillation.Z.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
}
