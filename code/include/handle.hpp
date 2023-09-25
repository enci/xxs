#pragma once

namespace xxs
{
    struct handle
    {
        int id = 0;
        bool operator==(const handle& other) const { return id == other.id; }
        bool operator!=(const handle& other) const { return id != other.id; }
        bool is_valid() const { return id != 0; }
    };
}