#include <string>

namespace xxs::script
{
    void initialize(const std::string& main_script);
    void shutdown();
    void update(double dt);
    void render();
}