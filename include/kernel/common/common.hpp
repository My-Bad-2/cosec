#pragma once

#include <algorithm>

constexpr inline bool remove_from(auto& container, auto&& value) {
    return container.erase(
               std::remove(container.begin(), container.end(), value),
               container.end()) != container.end();
}