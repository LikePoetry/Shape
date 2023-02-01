#include "hzpch.h"
#include "CoreSystem.h"
#include "Log.h"
#include "Version.h"
#include "JobSystem.h"

namespace Shape
{
	namespace Internal
	{
		/// ��Ӧ�ó������Ԥ�����ݳ�ʼ��
		bool CoreSystem::Init()
		{
			Debug::Log::OnInit();

			SHAPE_LOG_INFO("Shape Engine - Version {0}.{1}.{2}", ShapeVersion.major, ShapeVersion.minor, ShapeVersion.patch);

			System::JobSystem::OnInit();
			SHAPE_LOG_INFO("Initilising System");

			return true;
		}

		void CoreSystem::Shutdown()
		{
			Debug::Log::OnRelease();
		}
	}
}