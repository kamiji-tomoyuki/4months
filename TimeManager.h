#pragma once
#include <string>
#include <map>

class TimeManager{
public:
	struct Timer
	{
		float kLimitTime = 0.5f;
		float timeCount = 0.0f;
		bool isStart = false;
	};
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void TimeReset();

	void SetTimeStart(float timeSpeed,float kLimitTime);
	//デルタタイム
	static inline float deltaTime_ = 1.0f / 60.0f;
	static inline const float kFlamTime_ = 1.0f / 60.0f;
private:
	float kLimitTime_ = 0.5f;
	float timeCount_ = 0.0f;
	std::map<std::string, Timer> timers_;

	float currentTime_ = 0.0f; // 現在の経過時間
public:
	Timer GetTimer(std::string name) { return timers_[name]; }

	// 現在の時間を取得
	float GetCurrentTime() const { return currentTime_; }

	void SetTimer(std::string name, float kLimitTime);
};

