#pragma once

namespace xxs
{
    struct handle
    {
        size_t id = 0;
        bool operator==(const handle& other) const { return id == other.id; }
        bool operator!=(const handle& other) const { return id != other.id; }
    };
}