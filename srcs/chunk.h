#pragma once

#include "shader.h"
#include "block.h"
#include "FastNoise.h"

#define CHUNK_Z 24
#define CHUNK_X 24
#define CHUNK_Y 256

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
	void update();
	void render(Shader shader);
	void renderWater(Shader shader);
	void faceRendering();
	void buildVAO(void);
	void addFace(int face, int x, int y, int z, int val, vector<float> *m, int *ps);
	void cleanVAO(void);
	
	// neighbors
	inline void setXMinus(Chunk *chunk) { this->xMinus = chunk; }
	inline void setXPlus(Chunk *chunk) { this->xPlus = chunk; }
	inline void setZMinus(Chunk *chunk) { this->zMinus = chunk; }
	inline void setZPlus(Chunk *chunk) { this->zPlus = chunk; }
	inline Chunk *getXMinus() { return (this->xMinus); }
	inline Chunk *getXPlus() { return (this->xPlus); }
	inline Chunk *getZMinus() { return (this->zMinus); }
	inline Chunk *getZPlus() { return (this->zPlus); }

	// lighting
	inline int getSunLight(int x, int y, int z) { return (sunLightMap[x][y][z]); };
	inline void setSunLight(int x, int y, int z, int val) { sunLightMap[x][y][z] = val; };
	inline int getTorchLight(int x, int y, int z) { return (torchLightMap[x][y][z]); };
	inline void setTorchLight(int x, int y, int z, int val) { torchLightMap[x][y][z] = val; };
	inline void clearSunLightMap() { memset(this->sunLightMap, 0, sizeof(this->sunLightMap)); }

	void setTerrain(FastNoise terrainNoise, FastNoise temperatureNoise, FastNoise humidityNoise);
	int	getWorld(int x, int y, int z);
	bool neighborsSet = false;
	bool built = false;
	Block *getBlock(int x, int y, int z);
	inline int getXOff() { return xoff; }
	inline int getZOff() { return zoff; }
private:
	// friend class Player;
	int xoff;
	int zoff;
	Block blocks[CHUNK_X][CHUNK_Y][CHUNK_Z];
	char torchLightMap[CHUNK_X][CHUNK_Y][CHUNK_Z];
	char sunLightMap[CHUNK_X][CHUNK_Y][CHUNK_Z];
	glm::mat4 offsetMatrix;
	unsigned int VAO;
	unsigned int VBO;

	int pointSize;
	int transparentPointSize;

	unsigned int transparentVAO;
	unsigned int transparentVBO;

	vector<float> mesh;
	vector<float> transparentMesh;

	ChunkState state = GENERATE;
	Chunk *xMinus = NULL;
	Chunk *xPlus = NULL;
	Chunk *zMinus = NULL;
	Chunk *zPlus = NULL;
};