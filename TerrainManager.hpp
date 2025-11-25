#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <Vector>
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include "PerlinNoise.hpp"
#include "Tree.hpp"

class TerrainManager {

public:
	struct Chunk {
		VAO vao;
		VBO vbo;
		EBO ebo;
		int indexCount;
		glm::vec3 origin;
		std::vector<float> heightMap;
		int divisions;
		float width;

		//std::vector<Tree> trees;
	};
	TerrainManager();
	~TerrainManager();
	std::vector<Chunk> chunks;
	Chunk generateChunk(glm::vec3 origin, int divisions, float width);
	std::vector<GLfloat> verySimplePlane(int divisions, int width);
	std::vector<GLuint> genPlaneInd(int divisions);
	std::vector<GLfloat> line(glm::vec3 start, glm::vec3 finish, int divisions);
	std::vector<GLfloat> planeVerticies(glm::vec3 v0,
		glm::vec3 v1,
		glm::vec3 v2,
		glm::vec3 v3,
		int divisions,glm::vec3 origin);
	std::vector<glm::vec3> computeNormal(const std::vector<glm::vec3>& vertices,
		const std::vector<GLuint>& indices);
	float globalHeight(glm::vec3 origin);
	float getHeightAtWorldPos(float x, float z);
	float getHeightAt(float x, float y);
	Chunk* getChunkFor(float x, float z);
	glm::vec2 domainWrap(const glm::vec2& p, PerlinNoise& n);
	void populateTrees(Chunk& chunk, gps::Model3D* treeModel);
private:

};