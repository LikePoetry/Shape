#include "hzpch.h"
#include "EntityFactory.h"

namespace Shape
{
	using namespace Maths;

	glm::vec4 EntityFactory::GenColour(float alpha)
	{
		glm::vec4 c;
		c.w = alpha;

		c.x = Random32::Rand(0.0f, 1.0f);
		c.y = Random32::Rand(0.0f, 1.0f);
		c.z = Random32::Rand(0.0f, 1.0f);

		return c;
	}

	Entity EntityFactory::BuildSphereObject(
		Scene* scene,
		const std::string& name,
		const glm::vec3& pos,
		float radius,
		bool physics_enabled,
		float inverse_mass,
		bool collidable,
		const glm::vec4& colour)
	{

	}

	Entity EntityFactory::BuildCuboidObject(
		Scene* scene,
		const std::string& name,
		const glm::vec3& pos,
		const glm::vec3& halfdims,
		bool physics_enabled,
		float inverse_mass,
		bool collidable,
		const glm::vec4& colour)
	{

	}

	Entity EntityFactory::BuildPyramidObject(
		Scene* scene,
		const std::string& name,
		const glm::vec3& pos,
		const glm::vec3& halfdims,
		bool physics_enabled,
		float inverse_mass,
		bool collidable,
		const glm::vec4& colour)
	{

	}

	void EntityFactory::AddLightCube(Scene* scene, const glm::vec3& pos, const glm::vec3& dir)
	{

	}

	void EntityFactory::AddSphere(Scene* scene, const glm::vec3& pos, const glm::vec3& dir)
	{

	}

	void EntityFactory::AddPyramid(Scene* scene, const glm::vec3& pos, const glm::vec3& dir)
	{

	}
}