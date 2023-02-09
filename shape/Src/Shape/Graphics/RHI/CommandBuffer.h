#pragma once

namespace Shape
{
	namespace Graphics
	{
		class CommandBuffer
		{
		public:
			virtual ~CommandBuffer() = default;

			static CommandBuffer* Create();

			virtual bool Init(bool primary = true) = 0;

		protected:
			static CommandBuffer* (*CreateFunc)();
		};
	}
}