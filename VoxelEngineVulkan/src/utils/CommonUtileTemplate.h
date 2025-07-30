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
		Map& map, const Key& key, const std::string& errorMsg = "key not found") {
		auto it = map.find(key);
		if (it == map.end()) {
			throw std::runtime_error(errorMsg);
		}
		return it->second;
	}
}