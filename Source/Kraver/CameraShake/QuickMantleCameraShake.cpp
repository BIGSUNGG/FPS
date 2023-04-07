// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickMantleCameraShake.h"

UQuickMantleCameraShake::UQuickMantleCameraShake()
{
	OscillationDuration = 0.25f;
	RotOscillation.Pitch.Amplitude = -25.f;
	RotOscillation.Pitch.Frequency = 1.f;
	FOVOscillation.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
	LocOscillation.Z.Amplitude = 100.f;
	LocOscillation.Z.Frequency = 1.f;
	LocOscillation.Z.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
}
