#include "Math.h"
glm::vec3 utils::math::VecMin(const glm::vec3& a, const glm::vec3& b)
{
	return glm::vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}
glm::vec3 utils::math::VecMax(const glm::vec3& a, const glm::vec3& b) {
	return glm::vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}