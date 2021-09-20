#include <vk_engine.h>

float getRand01() {
	return ((double)rand()) / RAND_MAX;
}

void createObjects(VulkanEngine& engine) {
	int n = 200;
	for (int i = 0; i < n; i++) {
		RenderObject monkey;
		monkey.meshName = "monkey";
		monkey.textureName = "monkey";
		monkey.materialName = "defaultMaterial";
		glm::mat4 translation = glm::translate(glm::mat4{ 1.0 },
			glm::vec3((getRand01() - 0.5f) * 15.0f, (getRand01() - 0.5f) * 15.0f, (getRand01() - 0.5f) * 15.0f));
		glm::mat4 scale = glm::scale(glm::mat4{ 1.0 },
			glm::vec3(0.5, 0.5, 0.5));
		monkey.transformMatrix = translation * scale;
		monkey.color = glm::vec4(getRand01(), getRand01(), getRand01(), 1.0f);
		engine._renderables.push_back(monkey);
	}
}

int main(int argc, char* argv[])
{
	VulkanEngine engine;

	engine.get_mesh("../../assets/monkey_flat.obj", "monkey");
	engine.get_image("../../assets/monkey.png", "monkey");

	createObjects(engine);

	bool done = false;
	while (!done) {
		done = engine.looper();
	}

	return 0;
}
