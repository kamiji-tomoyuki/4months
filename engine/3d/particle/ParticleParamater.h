#pragma once
#include "Vector3.h"

struct RandomNum {
	Vector3 startNum;
	Vector3 endNum;
};

//パラメータ
struct Parameter {
	Vector3 startNum;
	Vector3 startRandomRange;
	Vector3 endNum;
	Vector3 endRandomRange;
	Vector3 velocity;
	Vector3 velocityRandomRange;
	Vector3 acceleration;
	Vector3 accelerationRandomRange;
};

enum ParameterState {
	START,
	VELOCITY,
	EASING,
};

enum EasingState {
	LERP,
	EASEIN,
	EASEOUT,
};