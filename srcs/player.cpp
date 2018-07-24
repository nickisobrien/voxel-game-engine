#include "player.h"

void Player::processInput(GLFWwindow *window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	glm::vec3 savePos = this->getPosition();

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		this->camera.ProcessKeyboard(FORWARD, deltaTime, this->getChunk());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		this->camera.ProcessKeyboard(BACKWARD, deltaTime, this->getChunk());
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		this->camera.ProcessKeyboard(LEFT, deltaTime, this->getChunk());
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		this->camera.ProcessKeyboard(RIGHT, deltaTime, this->getChunk());
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		this->jump();
	// if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
	// {
	// 	this->getChunk()->update();
	// }

	glm::vec3 newPos = this->getPosition();

	int x = (int)floor(newPos.x) % CHUNK_X;
	int y = floor(newPos.y);
	int z = (int)floor(newPos.z) % CHUNK_Z;
	if (x < 0)
		x = CHUNK_X + x;
	if (z < 0)
		z = CHUNK_Z + z;

	Chunk *c;
	if ((c = this->getChunk()) != NULL)
	{
		Block *b = c->getBlock(x,y,z);
		if (b != NULL && b->isActive())
		{
			b = c->getBlock(x, y+1, z);
			if (b != NULL && !b->isActive())
				this->setPosition(glm::vec3(newPos.x, newPos.y+1, newPos.z));
			else
				this->setPosition(savePos); // need to change to only reverting x/y/z, not necessarily all of them
		}
	}
}

Chunk *Player::getChunk()
{
	int cx = this->camera.Position.x >= 0.0f ? this->camera.Position.x / CHUNK_X : ceil(this->camera.Position.x) / CHUNK_X - 1.0f;
	int cz = this->camera.Position.z >= 0.0f ? this->camera.Position.z / CHUNK_Z : ceil(this->camera.Position.z) / CHUNK_X - 1.0f;
	glm::ivec2 pos(cx, cz);
	if (this->terr->world.find(pos) != this->terr->world.end())
		return (this->terr->world[pos]);
	return (NULL);
}

bool Player::isGrounded()
{
	glm::vec3 current = this->getPosition();
	int x = (int)floor(current.x) % CHUNK_X;
	int y = floor(current.y);
	int z = (int)floor(current.z) % CHUNK_Z;
	if (x < 0)
		x = CHUNK_X + x;
	if (z < 0)
		z = CHUNK_Z + z;

	// cout << "Block:	(" << x << ", " << y << ", " << z << ")" << endl;
	// cout << "Player: (" << current.x << ", " << current.y << ", " << current.z << ")" << endl;
	Block *b = getChunk()->getBlock(x,y-1,z);
	if (b != NULL && (b->isActive()))
		return (true);
	return (false);
}

void Player::jump()
{
	glm::vec3 cur = this->getPosition();
	if (this->isGrounded())
		this->setPosition(glm::vec3(cur.x, cur.y+5.0f, cur.z));
}

void Player::applyGravity(float time)
{
	glm::vec3 current = this->getPosition();
	if (!this->isGrounded())
		current.y -= this->gravity * time;
	this->setPosition(current);
}

void Player::mouseClickEvent()
{
	glm::vec3 current = this->getPosition();
	int x = (int)floor(current.x) % CHUNK_X;
	int y = floor(current.y);
	int z = (int)floor(current.z) % CHUNK_Z;
	if (x < 0)
		x = CHUNK_X + x;
	if (z < 0)
		z = CHUNK_Z + z;
	// cout << "Block:	(" << x << ", " << y << ", " << z << ")" << endl;
	// cout << "Player: (" << current.x << ", " << current.y << ", " << current.z << ")" << endl;
	// cout << "View vector: "<< this->camera.GetViewVector().x << " " <<
	// this->camera.GetViewVector().y << " " << this->camera.GetViewVector().z << endl;
	glm::vec3 currentView = this->camera.GetViewVector();

	float _bin_size = 1.0f;

	// This id of the first/current voxel hit by the ray.
	// Using floor (round down) is actually very important,
	// the implicit int-casting will round up for negative numbers.
	glm::ivec3 current_voxel((int)floor(current.x) % CHUNK_X, floor(current.y), (int)floor(current.z) % CHUNK_Z);

	// Compute normalized ray direction.
	glm::vec3 ray = this->camera.GetViewVector();

	// In which direction the voxel ids are incremented.
	int stepX = (ray.x >= 0) ? 1:-1;
	int stepY = (ray.y >= 0) ? 1:-1;
	int stepZ = (ray.z >= 0) ? 1:-1;

	// Distance along the ray to the next voxel border from the current position (tMaxX, tMaxY, tMaxZ).
	float next_voxel_boundary_x = (current_voxel.x+stepX)*_bin_size;
	float next_voxel_boundary_y = (current_voxel.y+stepY)*_bin_size;
	float next_voxel_boundary_z = (current_voxel.z+stepZ)*_bin_size;

	// tMaxX, tMaxY, tMaxZ -- distance until next intersection with voxel-border
	// the value of t at which the ray crosses the first vertical voxel boundary
	float tMaxX = (next_voxel_boundary_x - current.x) / ray.x;
	float tMaxY = (next_voxel_boundary_y - current.y) / ray.y;
	float tMaxZ = (next_voxel_boundary_z - current.z) / ray.z;

	// tDeltaX, tDeltaY, tDeltaZ --
	// how far along the ray we must move for the horizontal component to equal the width of a voxel
	// the direction in which we traverse the grid
	// can only be FLT_MAX if we never go in that direction
	float tDeltaX = _bin_size/ray.x*stepX;
	float tDeltaY = _bin_size/ray.y*stepY;
	float tDeltaZ = _bin_size/ray.z*stepZ;

	// cout << "Current_voxel" << current_voxel.x << " " << current_voxel.y << " " << current_voxel.z << endl;
	// cout << "Steps:" << stepX <<" "<<stepY<<" "<<stepZ<<endl;
	// cout << "Next boundary:"<<next_voxel_boundary_x<<" "<<next_voxel_boundary_y<<" "<<next_voxel_boundary_z<<endl;
	// cout << "tMaxs:"<< tMaxX << " " << tMaxY << " " << tMaxZ << endl;
	// cout << "tDeltas:"<<tDeltaX<<" "<<tDeltaY<< " " << tDeltaZ << endl;

	glm::ivec3 diff(0,0,0);
	bool neg = false;
	if (ray.x < 0)
	{
		diff.x--;
		neg = true;
	}
	if (ray.y < 0)
	{
		diff.y++;
		neg = true;
	}
	if (ray.z < 0)
	{
		diff.z--;
		neg = true;
	}
	
	Block *b = this->getChunk()->getBlock(current_voxel.x,current_voxel.y,current_voxel.z);
	if (b && b->isActive())
	{
		// cout << "EARLY FIND1" << endl;
		// cout << "FOUND " << current_voxel.x << " " << current_voxel.y << " " << current_voxel.z << endl;
		b->setType(0);
		this->getChunk()->update();
		return;
	}
	if (neg)
	{
		current_voxel += diff;
		b = this->getChunk()->getBlock(current_voxel.x,current_voxel.y,current_voxel.z);
		if (b && b->isActive())
		{
			// cout << "EARLY FIND2" << endl;
			// cout << "FOUND " << current_voxel.x << " " << current_voxel.y << " " << current_voxel.z << endl;
			b->setType(0);
			this->getChunk()->update();
			return;
		}
	}
	int ctr = 0;
	do
	{
		if (tMaxX < tMaxY)
		{
			if (tMaxX < tMaxZ)
			{
				current_voxel.x += stepX;
				tMaxX += tDeltaX;
			}
			else
			{
				current_voxel.z += stepZ;
				tMaxZ += tDeltaZ;
			}
		}
		else
		{
			if (tMaxY < tMaxZ)
			{
				current_voxel.y += stepY;
				tMaxY += tDeltaY;
			}
			else
			{
				current_voxel.z += stepZ;
				tMaxZ += tDeltaZ;
			}
			b = this->getChunk()->getBlock(current_voxel.x,current_voxel.y,current_voxel.z);
			ctr++;
		}
	} while ((!b || !b->isActive()) && ctr < 20);
	if (b && b->isActive())
	{
		// cout << "FOUND " << current_voxel.x << " " << current_voxel.y << " " << current_voxel.z << endl;
		b->setType(0);
		this->getChunk()->update();
	}


	// int ctr = 0;
	// while (ctr < 20)
	// {
	// 	cout << "Current check(s): "<< (int)currentCheck.x << " " <<
	// 	(int)currentCheck.y << " " << (int)currentCheck.z << " floats: " << currentCheck.x << " "
	// 	<< currentCheck.y << " " << currentCheck.z << endl;
		
	// 	// need to implement a way to do checks where i only subtract certain axis's line bresenham
	// 	if (this->getChunk()->getBlock(round(currentCheck.x),round(currentCheck.y),round(currentCheck.z)) &&
	// 		this->getChunk()->getBlock(round(currentCheck.x),round(currentCheck.y),round(currentCheck.z))->getType()!=0)
	// 	{
	// 		this->getChunk()->getBlock(round(currentCheck.x),round(currentCheck.y),round(currentCheck.z))->setType(0);
	// 		this->getChunk()->update();
	// 		break;
	// 	}
	// 	currentCheck.x -= currentView.x/10.0f;
	// 	currentCheck.y -= currentView.y/10.0f;
	// 	currentCheck.z -= currentView.z/10.0f;
	// 	ctr++;
	// }



	
}
