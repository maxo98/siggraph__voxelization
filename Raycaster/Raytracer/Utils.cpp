#include "Utils.h"


std::ostream& operator<<(std::ostream& os, const std::vector<float>& dt)
{
	for (int i = 0; i < dt.size(); i++)
	{
		if (i != 0)
		{
			os << ", ";
		}

		os << dt[i];
	}

	return os;
}

std::ostream& operator<<(std::ostream& os, const std::pair<int, int>& dt)
{
	os << "first: " << dt.first << " second " << dt.second;

	return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& dt)
{
	os << dt.x << " " << dt.y << " " << dt.z;

	return os;
}

std::ostream& operator<<(std::ostream& os, const glm::uvec3& dt)
{
	os << dt.x << " " << dt.y << " " << dt.z;

	return os;
}

bool intersectPlane(const glm::vec3& normal, const glm::vec3 &posPlane, const glm::vec3& posRay, const glm::vec3& rayDir, float& t)
{
	// assuming vectors are all normalized
	float denom = glm::dot(normal, rayDir);

	if (denom > 1e-6) 
	{
		glm::vec3 dir = posPlane - posRay;
		t = glm::dot(dir, normal) / denom;
		return (t >= 0);
	}

	return false;
}