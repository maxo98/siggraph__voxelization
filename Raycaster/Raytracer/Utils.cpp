#include "Utils.h"
#include <fstream>
#include <random>
#include <iostream>

bool saveVectorToCsv(std::string name, std::vector<float>& data)
{
	int n = 1;
	std::ifstream f;

	do {
		if (f.is_open() == true) f.close();

		f = std::ifstream(name + "_" + std::to_string(n) + ".csv");
		n++;
	} while (f.good() && n < 100);

	f.close();

	if (n >= 100)
	{
		std::cout << "Could not find path, or files already exist." << std::endl;

		return false;
	}

	n--;

	std::fstream csvFile;
	csvFile.open(name + "_" + std::to_string(n) + ".csv", std::fstream::in | std::fstream::out | std::fstream::trunc);

	if (csvFile.is_open() == false)
	{
		std::cout << "Fstream failed to create file." << std::endl;

		return false;
	}

	for (std::vector<float>::iterator it = data.begin(); it != data.end(); ++it)
	{
		std::string str = std::to_string(*it);
		size_t pos = str.find(".");

		if (pos == -1)
		{
			csvFile << str << std::endl;
		}
		else {
			csvFile << str.substr(0, pos) << "," << str.substr(pos + 1, str.size()) << std::endl;
		}
	}

	csvFile.close();
	return true;
}

int randGeoDist(float p, int max)
{
	std::default_random_engine generator;
	std::geometric_distribution<int> distribution(p);

	int value;

	do {
		value = distribution(generator);

	} while (value > max);

	return value;
}

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

std::ostream& operator<<(std::ostream& os, const glm::ivec3& dt)
{
	os << dt.x << " " << dt.y << " " << dt.z;

	return os;
}

std::ostream& operator<<(std::ostream& os, const glm::dvec3& dt)
{
	os << dt.x << " " << dt.y << " " << dt.z;

	return os;
}

//bool intersectPlane(const glm::vec3& normal, const glm::vec3 &posPlane, const glm::vec3& posRay, const glm::vec3& rayDir, float& t)
//{
//	// assuming vectors are all normalized
//	float denom = glm::dot(normal, rayDir);
//
//	if (denom > std::numeric_limits<float>::epsilon())
//	{
//		glm::vec3 dir = posPlane - posRay;
//		t = glm::dot(dir, normal) / denom;
//
//		return (t >= 0.f);
//	}
//
//	return false;
//}