#pragma once

namespace Shapes
{
    namespace Graphics
    {
        class RenderDoc
        {
        public:
            RenderDoc()  = default;
            ~RenderDoc() = default;

            static void OnPreDeviceCreation();
            static void Shutdown();
            static void FrameCapture();
        };
    }
}
