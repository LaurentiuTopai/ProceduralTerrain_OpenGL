#include <GL/glew.h>
#include <vector>
#include "BufferUsage.hpp"
class VBO {
public:
	VBO():ID(0){}
	void init(const std::vector<float>&data,BufferUsage usage = BufferUsage::STATIC_DRAW){
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), ConvertUsage(usage));
	}
	void Bind() { glBindBuffer(GL_ARRAY_BUFFER, ID); }
	void UnBind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
	void Delete() { glDeleteBuffers(1, &ID); }
	GLuint getID() { return ID; }
private:
	GLuint ID;
};