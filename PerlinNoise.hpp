#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <numeric>

class PerlinNoise {

public:
	PerlinNoise();
	PerlinNoise(unsigned int seed);

	float noise(float x, float y)const;
	std::vector<int> p;
	float fade(float t)const;
	float lerp(float a,float b,float t) const;
	float grad(int hash,float x,float y) const;


private:

};