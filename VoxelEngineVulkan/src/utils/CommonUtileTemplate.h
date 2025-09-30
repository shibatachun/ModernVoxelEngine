#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <set>
#include <string>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <map>
#include <unordered_map>
#include <filesystem>

namespace utils {
    template<typename Map, typename Key>
    const typename Map::mapped_type& findInMap(
        const Map& map, const Key& key, typename Map::mapped_type defaultValue = {})
    {
        auto it = map.find(key);
        if (it == map.end()) {
            return defaultValue; // 没找到，返回默认值
        }
        return it->second;
    }
}