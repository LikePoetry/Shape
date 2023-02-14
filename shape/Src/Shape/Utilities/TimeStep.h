#pragma once
namespace Shape
{
	class Timer;
	class SHAPE_EXPORT TimeStep
	{
	public:
		TimeStep();
		~TimeStep();

		void OnUpdate();
		inline double GetMillis() const { return m_Timestep; }
		inline double GetElapsedMillis() const { return m_Elapsed; }

		inline double GetSeconds() const { return m_Timestep * 0.001; }
		inline double GetElapsedSeconds() const { return m_Elapsed * 0.001; }
	
	private:
		double m_Timestep;
		double m_LastTime;
		double m_Elapsed;

		Timer* m_Timer;
	};
}