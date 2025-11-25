
#include "TerrainManager.hpp"
#include "Camera.hpp"
/*
* Cu cat e mare mare div cu atat matricea de teren este mai mare
* 
*/
TerrainManager::TerrainManager() {

}
TerrainManager::~TerrainManager() {

}




static float computeHeight(const glm::vec2& samplePos, PerlinNoise& noiseGen) {
	const int OCTAVES = 6;
	const float BASE_FREQ = 0.0025f;   // joas? frecven?? pentru mun?i
	const float LACUNARITY = 2.0f;
	const float GAIN = 0.5f;
	const float BASE_AMP = 80.0f;      // scale vertical? (ajusteaz?)

	float h = 0.0f;
	float freq = BASE_FREQ;
	float amp = BASE_AMP;

	// domain warp can be applied externally if desired; here samplePos should be already warped
	for (int o = 0; o < OCTAVES; ++o) {
		h += noiseGen.noise(samplePos.x * freq, samplePos.y * freq) * amp;
		freq *= LACUNARITY;
		amp *= GAIN;
	}

	// add some ridged detail (small scale)
	float ridge = 0.0f;
	{
		float rfreq = 0.02f;
		for (int i = 0; i < 3; i++) {
			ridge += (1.0f - fabs(noiseGen.noise(samplePos.x * rfreq, samplePos.y * rfreq))) * (10.0f / (i + 1));
			rfreq *= 2.2f;
		}
	}

	// mild global falloff (optional) to shape central massif — remove if not wanted
	// float dist = glm::length(samplePos) * 0.001f;
	// h *= exp(-dist*dist);

	return h + ridge * 0.8f;
}


glm::vec2 TerrainManager::domainWrap(const glm::vec2& p, PerlinNoise& n) {
	// tuned warp that uses global coordinates -> deterministic across chunks
	float qx = n.noise(p.x * 0.02f, p.y * 0.02f);
	float qy = n.noise((p.x + 5.2f) * 0.02f, (p.y + 1.3f) * 0.02f);
	glm::vec2 q(qx, qy);
	glm::vec2 r;
	r.x = n.noise(p.x * 0.05f + 4.0f * q.x, p.y * 0.05f + 4.0f * q.y);
	r.y = n.noise(p.x * 0.05f + 4.0f * q.y, p.y * 0.05f + 4.0f * q.x);
	// scale warp modestly so adjacent chunks remain consistent
	return p + r * 6.0f; // am redus de la 10.0f la 6.0f ca s? evit deform?ri mari la margini
}
float ridged(float x, float y, PerlinNoise& n) {
	float v = 1.0f - fabs(n.noise(x, y));
	return v * v;
}
std::vector<GLfloat>TerrainManager::verySimplePlane(int divisions, int width) {
	std::vector<GLfloat> plane;
	float triangleSide = width / divisions;
	for (int i = 0; i < divisions + 1; i++) {
		for (int j = 0; j < divisions + 1; j++) {
			glm::vec3 crntVec = glm::vec3(j * triangleSide, 0.0f, i * -triangleSide);
			plane.push_back(crntVec.x);
			plane.push_back(crntVec.y);
			plane.push_back(crntVec.z);
		}
	}
	return plane;
}
std::vector<GLuint>TerrainManager::genPlaneInd(int divisions) {
	std::vector<GLuint> indices;

	for (int row = 0; row < divisions; ++row) {
		for (int col = 0; col < divisions; ++col) {
			GLuint topLeft = row * (divisions + 1) + col;
			GLuint topRight = topLeft + 1;
			GLuint bottomLeft = (row + 1) * (divisions + 1) + col;
			GLuint bottomRight = bottomLeft + 1;

			// Primul triunghi
			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);

			// Al doilea triunghi
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
		}
	}

	return indices;
}
std::vector<GLfloat>TerrainManager::line(glm::vec3 start, glm::vec3 finish, int divisions) {
	std::vector<GLfloat> line;
	glm::vec3 vectorDiv = (finish - start) / (float)divisions;

	for (int i = 0; i < divisions + 1; i++) {
		glm::vec3 crntVec = start + vectorDiv * (float)i;
		line.push_back(crntVec.x);
		line.push_back(crntVec.y);
		line.push_back(crntVec.z);
	}
	return line;
}
float TerrainManager::globalHeight(glm::vec3 origin) {
	float centerX = 0.0f;
	float centerZ = 0.0f;
	float maxHeight = 50.0f;
	float distance = glm::length(glm::vec2(origin.x - centerX, origin.z - centerZ));

	float height = maxHeight * exp(-0.001f * distance * distance);
	return height;
}
float TerrainManager::getHeightAtWorldPos(float x, float z) {
	PerlinNoise noise(1234); // global noise
	float height = 0.0f;
	float freq = 0.005f;
	float amp = 20.0f;
	for (int o = 0; o < 5; o++) {
		height += noise.noise(x * freq, z * freq) * amp;
		freq *= 2.0f;
		amp *= 0.5f;
	}
	return height;
}
std::vector<GLfloat> TerrainManager::planeVerticies(
	glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, int divisions,glm::vec3 origin)
{
	std::vector<GLfloat> plane;
	plane.reserve((divisions + 1) * (divisions + 1) * 3);

	glm::vec3 vec03 = (v3 - v0) / (float)divisions;
	glm::vec3 vec12 = (v2 - v1) / (float)divisions;
	int ix = static_cast<int>(origin.x);
	int iz = static_cast<int>(origin.z);
	int seed = (ix * 73856093) ^ (iz * 19349663);
	PerlinNoise noiseGen(1234);
	
	float minGlobalHeight = 23.0f;
	float maxGlobalHeight = 100.0f; // înlimea maxim a centrului
	float centerX = 0.0f;
	float centerZ = 0.0f;
	float distance = glm::length(glm::vec2(origin.x - centerX, origin.z - centerZ));
	float falloff = exp(-0.001f * distance * distance); // 1.0 la centru, 0 la margini
	float baseAmp = 32.5f;


	// CONFIGURATIE TEREN NATURAL
	const int octaves = 5;
	const float lacunarity = 2.0f;
	const float gain = 0.5f;
	const float baseFreq = 0.005f;
	//const float baseAmp = 20.0f * (1.0f + globalHeight(origin) / 50.0f);


	for (int i = 0; i <= divisions; i++)
	{
		glm::vec3 start = v0 + vec03 * (float)i;
		glm::vec3 finish = v1 + vec12 * (float)i;

		for (int j = 0; j <= divisions; j++)
		{
			float t = (float)j / divisions;
			glm::vec3 p = glm::mix(start, finish, t);

			// Fractal Brownian Motion (fBm)
			float height = 0.0f;
			float freq = baseFreq;
			float amp = baseAmp;

			for (int o = 0; o < octaves; o++)
			{
				glm::vec3 worldPos = origin + p;
				height += noiseGen.noise(p.x * freq, p.z * freq) * amp;
				freq *= lacunarity;
				amp *= gain;
			}

			p.y =height;

			plane.push_back(p.x);
			plane.push_back(p.y);
			plane.push_back(p.z);
		}
	}

	return plane;
}


std::vector<glm::vec3> TerrainManager::computeNormal(
	const std::vector<glm::vec3>& vertices,
	const std::vector<GLuint>& indices)
{
	std::vector<glm::vec3> normals(vertices.size(), glm::vec3(0.0f));

	for (size_t i = 0; i + 2 < indices.size(); i += 3)
	{
		GLuint i0 = indices[i + 0];
		GLuint i1 = indices[i + 1];
		GLuint i2 = indices[i + 2];

		if (i0 >= vertices.size() || i1 >= vertices.size() || i2 >= vertices.size())
		{
			std::cerr << "[Warning] Invalid triangle indices: "
				<< i0 << ", " << i1 << ", " << i2
				<< " | vertices size: " << vertices.size() << "\n";
			continue;
		}
		glm::vec3 v0 = vertices[i0];
		glm::vec3 v1 = vertices[i1];
		glm::vec3 v2 = vertices[i2];

		glm::vec3 n = glm::cross(v1 - v0, v2 - v0);

		if (glm::length(n) > 0.0f)
			n = glm::normalize(n);

		normals[i0] += n;
		normals[i1] += n;
		normals[i2] += n;
	}

	for (auto& n : normals)
	{
		if (glm::length(n) > 0.0f)
			n = glm::normalize(n);
	}

	return normals;
}


TerrainManager::Chunk TerrainManager::generateChunk(glm::vec3 origin, int divisions, float width) {
	Chunk ch;
	glm::vec3 v0 = origin + glm::vec3(0, 0, 0);
	glm::vec3 v1 = origin + glm::vec3(width, 0, 0);
	glm::vec3 v2 = origin + glm::vec3(width, 0, width);
	glm::vec3 v3 = origin + glm::vec3(0, 0, width);

	std::vector<GLfloat> vertices = planeVerticies(v0, v1, v2, v3, divisions,origin);
	std::vector<GLuint> indices = genPlaneInd(divisions);

	// normals ? vertexBufferData
	std::vector<glm::vec3> v3vec;
	for (int i = 0; i < vertices.size(); i += 3) {
		v3vec.emplace_back(vertices[i], vertices[i + 1], vertices[i + 2]);
	}
	auto normals = computeNormal(v3vec, indices);

	std::vector<GLfloat> buffer;
	buffer.reserve(v3vec.size() * 6);

	for (int i = 0; i < v3vec.size(); ++i) {
		buffer.push_back(v3vec[i].x);
		buffer.push_back(v3vec[i].y);
		buffer.push_back(v3vec[i].z);

		buffer.push_back(normals[i].x);
		buffer.push_back(normals[i].y);
		buffer.push_back(normals[i].z);
		ch.heightMap.push_back(v3vec[i].y);

		float u = (v3vec[i].x - origin.x) / width * 4.0f; // repeat = 4
		float v = (v3vec[i].z - origin.z) / width * 4.0f;
		buffer.push_back(u);
		buffer.push_back(v);
	}
	//populateTrees(ch, nullptr); 
	ch.indexCount = indices.size();
	ch.origin = origin;
	ch.divisions = divisions;
	ch.width = width;
	ch.vao.init();
	ch.vbo.init(buffer, BufferUsage::STATIC_DRAW);
	ch.ebo.init(indices, BufferUsage::STATIC_DRAW);

	ch.vao.Bind();
	ch.vbo.Bind();
	ch.ebo.Bind();

	///POS
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//UV	
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	ch.vao.UnBind();

	return ch;
}
TerrainManager::Chunk* TerrainManager::getChunkFor(float x, float z)
{
	for (auto& ch : chunks)
	{
		if (x >= ch.origin.x && x <= ch.origin.x + ch.width &&
			z >= ch.origin.z && z <= ch.origin.z + ch.width)
			return &ch;
	}
	return nullptr;
}
float TerrainManager::getHeightAt(float x, float z)
{
	Chunk* c = getChunkFor(x, z);
	if (!c) return 0.0f;

	float lx = x - c->origin.x;
	float lz = z - c->origin.z;
	float gridSize = c->width / c->divisions;

	int ix = glm::clamp(int(floor(lx / gridSize)), 0, c->divisions - 1);
	int iz = glm::clamp(int(floor(lz / gridSize)), 0, c->divisions - 1);

	int rowLength = c->divisions + 1;
	int i0 = iz * rowLength + ix;
	int i1 = i0 + 1;
	int i2 = i0 + rowLength;
	int i3 = i2 + 1;

	float h0 = c->heightMap[i0];
	float h1 = c->heightMap[i1];
	float h2 = c->heightMap[i2];
	float h3 = c->heightMap[i3];

	float tx = (lx - ix * gridSize) / gridSize;
	float tz = (lz - iz * gridSize) / gridSize;

	float hA = glm::mix(h0, h1, tx);
	float hB = glm::mix(h2, h3, tx);

	return glm::mix(hA, hB, tz);
}
/*
* void TerrainManager::populateTrees(Chunk& ch, gps::Model3D* treeModel) {
	int numsTrees = 50;
	float minSlope = 0.4f;
	for (int i = 0; i < numsTrees; i++) {
		float x = ch.origin.x + (rand() / (float)RAND_MAX) * ch.width;
		float z = ch.origin.z + (rand() / (float)RAND_MAX) * ch.width;

		float h = getHeightAtWorldPos(x, z);
		if (h < 5.0f || h>40.0f) {
			continue;
		}
		float hX = getHeightAtWorldPos(x + 1, z);
		float hZ = getHeightAtWorldPos(x, z + 1);

		float slope = fabs(h - hX) + fabs(h - hZ);
		if (slope > minSlope) continue;

		ch.trees.emplace_back(glm::vec3(x, h, z), treeModel);
	}
	}
*/


