// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpLandCameraShake.h"

UJumpLandCameraShake::UJumpLandCameraShake()
{
	OscillationDuration = 0.25f;
	RotOscillation.Pitch.Amplitude = -50.f;
	RotOscillation.Pitch.Frequency = 1.f;
	FOVOscillation.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;

}
