#pragma once

#include <iostream>
#include <vector>
#include <glm.hpp>

std::ostream& operator<<(std::ostream& os, const std::vector<float>& dt);
std::ostream& operator<<(std::ostream& os, const std::pair<int, int>& dt);
std::ostream& operator<<(std::ostream& os, const glm::vec3& dt);
std::ostream& operator<<(std::ostream& os, const glm::uvec3& dt);

bool intersectPlane(const glm::vec3& normal, const glm::vec3& posPlane, const glm::vec3& posRay, const glm::vec3& rayDir, float& t);