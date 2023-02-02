#include "hzpch.h"
#include "CoreSystem.h"
#include "Log.h"
#include "Version.h"
#include "JobSystem.h"
#include "VFS.h"

namespace Shape
{
	namespace Internal
	{
		/// 对应用程序相关预置内容初始化
		bool CoreSystem::Init()
		{
			Debug::Log::OnInit();

			SHAPE_LOG_INFO("Shape Engine - Version {0}.{1}.{2}", ShapeVersion.major, ShapeVersion.minor, ShapeVersion.patch);

			System::JobSystem::OnInit();
			SHAPE_LOG_INFO("Initilising System");

			VFS::Get();
			SHAPE_LOG_INFO("Mount VFS System");

			return true;
		}

		void CoreSystem::Shutdown()
		{
			SHAPE_LOG_INFO("Shutting down System");
			VFS::Release();

			Debug::Log::OnRelease();
		}
	}
}