#pragma once
#include "Vector3.h"

//パラメータ
struct Parameter {
	Vector3 startNum;
	Vector3 randomStartNum;
	Vector3 endNum;
	Vector3 ramdomEndNum;
	Vector3 velocity;
	Vector3 randomVelocity;
	Vector3 acceleration;
	Vector3 randomAcceleration;
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
