#include "log.hpp"

int main()
{
    xxs::log::info("Hello World{}", ".");
    xxs::log::warn("Hello World{}", "?");
    xxs::log::error("Hello World{}", "!");
    return 0;
}
