
#include "Core/OS/OS.h"

namespace Shapes
{
    class SHAPES_EXPORT WindowsOS : public OS
    {
    public:
        WindowsOS()  = default;
        ~WindowsOS() = default;

        void Init();
        void Run() override;
        std::string GetExecutablePath() override;
    };
}