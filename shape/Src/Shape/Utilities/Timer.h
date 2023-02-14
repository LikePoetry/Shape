#pragma once
#include <windows.h>

typedef LARGE_INTEGER TimeStamp;

namespace Shape
{
	class SHAPE_EXPORT Timer
	{
	public:
		Timer()
			: m_Start(Now())
			, m_Frequency()
		{
			m_LastTime = m_Start;
		}
		~Timer() = default;

		float GetTimedMS();

		static TimeStamp Now();

		static double Duration(TimeStamp start, TimeStamp end, double timeResolution = 1.0);
		static float Duration(TimeStamp start, TimeStamp end, float timeResolution);

		float GetElapsedMS()
		{
			return GetElapsed(1000.0f);
		}

		float GetElapsedS()
		{
			return GetElapsed(1.0f);
		}

		double GetElapsedMSD()
		{
			return GetElapsed(1000.0);
		}

		double GetElapsedSD()
		{
			return GetElapsed(1.0);
		}

	protected:
		float GetElapsed(const float timeResolution) const
		{
			return Duration(m_Start, Now(), timeResolution);
		}

		double GetElapsed(const double timeResolution = 1.0) const
		{
			return Duration(m_Start, Now(), timeResolution);
		}

		TimeStamp m_Start;
		TimeStamp m_Frequency;
		TimeStamp m_LastTime;
	};
}