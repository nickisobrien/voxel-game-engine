#pragma once

#include "shader.h"
#include "block.h"
#include "FastNoise.h"

#define CHUNK_Z 16
#define CHUNK_X 16
#define CHUNK_Y 128

enum ChunkState
{
	GENERATE,
	UPDATE,
	RENDER,
	UNLOAD
};

class Chunk
{
public:
	Chunk(int xoff = 0, int zoff = 0);
	~Chunk(void);
	void update(void);
	void render(Shader shader);
	bool touchingAir(int x, int y, int z);
	void buildVAO(void);
	float *getVertices(void);
	float *getUVs(void);
	void add_face(int face, int x, int y, int z, int val);
	void cleanVAO(void);
	void setXMinus(Chunk *chunk);
	void setXPlus(Chunk *chunk);
	void setZMinus(Chunk *chunk);
	void setZPlus(Chunk *chunk);
	Chunk *getXMinus();
	Chunk *getXPlus();
	Chunk *getZMinus();
	Chunk *getZPlus();
	void set_terrain(FastNoise myNoise);
	int	getWorld(int x, int y, int z);
	glm::vec3 getUVBlock(int val, int i, int face);
	bool neighborsSet = false;
private:
	int xoff;
	int zoff;
	Block ***blocks;
	glm::mat4 offsetMatrix;
	unsigned int VAO;
	unsigned int VBO_VERT;
	unsigned int VBO_UV;
	vector<glm::vec3> points;
	vector<glm::vec3> uvs;
	ChunkState state = GENERATE;
	Chunk *xMinus = NULL;
	Chunk *xPlus = NULL;
	Chunk *zMinus = NULL;
	Chunk *zPlus = NULL;
};