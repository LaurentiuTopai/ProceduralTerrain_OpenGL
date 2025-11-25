#include <GL/glew.h>
#include <vector>

class VAO {
public:
	VAO():ID(0){}
	void init(){ glGenVertexArrays(1, &ID); }
	void Bind() { glBindVertexArray(ID); }
	void UnBind() { glBindVertexArray(0); }
	void Delete() { glDeleteVertexArrays(1, &ID); }
	GLuint getID() { return ID; }
private:
	GLuint ID;
};