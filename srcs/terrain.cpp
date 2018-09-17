#include "engine.h"
#include "terrain.h"

void Terrain::updateChunk(glm::ivec2 pos)
{
	if (this->world.find(pos) != this->world.end()) // built may be the interchangable with neighborsSet
	{
		this->world[pos]->clearSunLightMap();
		if (!this->world[pos]->neighborQueue.empty())
			this->world[pos]->neighborQueueUnload();
		this->lightEngine.sunlightInit(this->world[pos]);
		this->world[pos]->update();
	}
	else
	{
		this->world[pos] = new Chunk(pos.x, pos.y, this);
		this->setNeighbors(pos);
		this->world[pos]->setTerrain();
		this->lightEngine.sunlightInit(this->world[pos]);
		this->world[pos]->update();
	}
}

bool Terrain::renderChunk(glm::ivec2 pos, Shader shader)
{
	if (this->world.find(pos) != this->world.end() && this->world[pos]->getState() == RENDER)
	{
		this->world[pos]->render(shader);
		if (!this->world[pos]->neighborsSet)
			this->setNeighbors(pos);
	}
	else if (this->updateList == glm::ivec2(-100000,-100000))
	{	
		this->updateList = pos;
		return (false);
	}
	else if (this->world.find(pos) != this->world.end() && this->world[pos]->getState() == UPDATE) // render updates
	{
		this->world[pos]->render(shader);
	}
	return (true);
}

bool Terrain::renderWaterChunk(glm::ivec2 pos, Shader shader)
{
	if (this->world.find(pos) != this->world.end())
		this->world[pos]->renderWater(shader);
	else
		return (false);
	return (true);
}

void Terrain::setNoise(void)
{
	this->terrainNoise.SetSeed(std::time(0));
	this->terrainNoise.SetNoiseType(FastNoise::PerlinFractal);
	
	// this->terrainNoise.SetFrequency(0.002f); // planes 
	// this->terrainNoise.SetFractalOctaves(2);

	this->terrainNoise.SetFrequency(0.004f); // hills
	this->terrainNoise.SetFractalOctaves(3);

	// this->terrainNoise.SetFrequency(0.006f); // mountains
	// this->terrainNoise.SetFractalOctaves(4);

	this->temperatureNoise.SetSeed(terrainNoise.GetSeed()/2);
	this->temperatureNoise.SetNoiseType(FastNoise::PerlinFractal);
	this->temperatureNoise.SetFrequency(0.001f);

	this->temperatureNoise.SetSeed(terrainNoise.GetSeed()*2);
	this->humidityNoise.SetNoiseType(FastNoise::Perlin);
	this->humidityNoise.SetFrequency(0.001f);
}

void Terrain::setNeighbors(glm::ivec2 pos)
{
	if (!this->world[pos]->getXMinus() && this->world.find(glm::ivec2(pos.x-1, pos.y)) != this->world.end())
		this->world[pos]->setXMinus(this->world[glm::ivec2(pos.x-1, pos.y)]);

	if (!this->world[pos]->getXPlus() && this->world.find(glm::ivec2(pos.x+1, pos.y)) != this->world.end())
		this->world[pos]->setXPlus(this->world[glm::ivec2(pos.x+1, pos.y)]);

	if (!this->world[pos]->getZMinus() && this->world.find(glm::ivec2(pos.x, pos.y-1)) != this->world.end())
		this->world[pos]->setZMinus(this->world[glm::ivec2(pos.x, pos.y-1)]);

	if (!this->world[pos]->getZPlus() && this->world.find(glm::ivec2(pos.x, pos.y+1)) != this->world.end())
		this->world[pos]->setZPlus(this->world[glm::ivec2(pos.x, pos.y+1)]);

	if (this->world[pos]->getXPlus() && this->world[pos]->getXMinus() &&
		this->world[pos]->getZPlus() && this->world[pos]->getZMinus())
	{
		this->world[pos]->neighborsSet = true;
		if (!this->world[pos]->neighborQueue.empty())
			this->world[pos]->neighborQueueUnload();
	}
}