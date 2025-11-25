#include <GL/glew.h>
#include <vector>
#include "BufferUsage.hpp"
class EBO {
public:
	EBO():ID(0){}
	void init(const std::vector<GLuint>& indices,BufferUsage usage = BufferUsage::STATIC_DRAW) {
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), ConvertUsage(usage));
	}
	void Bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }
	void UnBind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
	void Delete() { glDeleteBuffers(1, &ID); }
	GLuint getID() { return ID; }
private:
	GLuint ID;
};