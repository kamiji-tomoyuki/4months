#include "TimeManager.h"
#include <cassert>

void TimeManager::Initialize(){
	deltaTime_ = kFlamTime_;
}

void TimeManager::Update(){
	if (timeCount_ != 0.0f) {
		timeCount_ += kFlamTime_;
		if (timeCount_ > kLimitTime_) {
			timeCount_ = 0.0f;
			deltaTime_ = kFlamTime_;
		}
	}
	for (auto& [name, timer] : timers_) {
		if (timer.isStart == true) {
			timer.timeCount += deltaTime_;
			if (timer.timeCount > timer.kLimitTime) {
				timer.timeCount = 0.0f;
				timer.isStart = false;
			}
		}
	}

	currentTime_ += deltaTime_; // フレームごとに時間を加算
}

void TimeManager::TimeReset(){
	deltaTime_ = kFlamTime_;
}

void TimeManager::SetTimeStart(float timeSpeed, float kLimitTime){
	kLimitTime_ = kLimitTime;
	deltaTime_ = timeSpeed;
	timeCount_ = kFlamTime_;
}

void TimeManager::SetTimer(std::string name, float kLimitTime){
	Timer& timer = timers_[name];
	timer.isStart = true;
	timer.kLimitTime = kLimitTime;
	timer.timeCount = 0;
}