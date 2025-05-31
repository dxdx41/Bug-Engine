#include "Timer.h"

Timer::Timer()
	: mSecondsPerCount{ 0.0 }
	, mDeltaTime{ -1.0 }
	, mBaseTime{ 0 }
	, mPausedTime{ 0 }
	, mPrevTime{ 0 }
	, mCurrTime{ 0 }
	, mStopped{ false }
{
	int64_t countsPerSec{};
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&countsPerSec));
	mSecondsPerCount = 1.0 / static_cast<double>(countsPerSec);
}

float Timer::TotalTime() const {
	if (mStopped) {
		return static_cast<float>(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
	else {
		return static_cast<float>(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
}

float Timer::DeltaTime() const {
	return static_cast<float>(mDeltaTime);
}

void Timer::Tick() {
	if (mStopped) {
		mDeltaTime = 0.0;
		return;
	}

	int64_t currentTick{};
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTick));
	mCurrTime = currentTick;

	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

	mPrevTime = mCurrTime;

	if (mDeltaTime < 0.0) {
		mDeltaTime = 0.0;
	}
}

void Timer::Reset() {
	int64_t currTick{};
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currTick));
	
	mBaseTime = currTick;
	mPrevTime = currTick;
	mStopTime = 0;
	mStopped = false;
}

void Timer::Stop() {
	if (!mStopped) {
		int64_t currTick{};
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currTick));

		mStopTime = currTick;
		mStopped = true;
	}
}

void Timer::Start() {
	int64_t startTick{};
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&startTick));

	if (mStopped) {
		mPausedTime += (startTick - mStopTime);

		mPrevTime = startTick;

		mStopTime = 0;
		mStopped = false;
	}
}