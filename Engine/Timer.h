#include <Windows.h>
#include <cstdint>

class Timer {
public:
	Timer();
	float TotalTime() const;
	float DeltaTime() const;

	void Reset();
	void Start();
	void Stop();
	void Tick();
private:
	double mSecondsPerCount{};
	double mDeltaTime{};
	int64_t mBaseTime{};
	int64_t mPausedTime{};
	int64_t mStopTime{};
	int64_t mPrevTime{};
	int64_t mCurrTime{};
	bool mStopped{};
};