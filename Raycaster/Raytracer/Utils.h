#pragma once

#include <iostream>
#include <vector>
#include <glm.hpp>

#include <utility>
#include <string>
#include <vector>
#include <iostream>


//Needed for unordered map
// custom specialization of std::hash can be injected in namespace std
template<>
struct std::hash<std::pair<unsigned int, unsigned int>>
{
	std::size_t operator()(std::pair<unsigned int, unsigned int> const& pair) const noexcept
	{
		return pair.first ^ (pair.second << 1); // or use boost::hash_combine
	}
};

inline float randFloat() { return ((double)rand()) / RAND_MAX; }

inline int randInt(int x, int y) { return ((y - x + 1) == 0) ? 0 : rand() % (y - x + 1) + x; };

int randGeoDist(float p, int max);

bool saveVectorToCsv(std::string name, std::vector<float>& data);

inline int randPosNeg() { return (rand() % 2) == 1 ? 1 : -1; };

std::ostream& operator<<(std::ostream& os, const std::vector<float>& dt);
std::ostream& operator<<(std::ostream& os, const std::pair<int, int>& dt);
std::ostream& operator<<(std::ostream& os, const glm::vec3& dt);
std::ostream& operator<<(std::ostream& os, const glm::uvec3& dt);
std::ostream& operator<<(std::ostream& os, const glm::ivec3& dt);
std::ostream& operator<<(std::ostream& os, const glm::dvec3& dt);

//bool intersectPlane(const glm::vec3& normal, const glm::vec3& posPlane, const glm::vec3& posRay, const glm::vec3& rayDir, float& t);