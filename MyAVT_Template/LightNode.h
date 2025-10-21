#pragma once

#include "renderer.h"
#include "Node.h"

extern Renderer renderer;

class LightNode {
public:
	virtual void CalculateLight(int* shaderArrayIndex) {};
	virtual std::string GetType() { return "LightNode"; }
	virtual float* GetPosition() { return position; }

	int shaderArrayIndex;
	float position[4];

private:

};

class PointLightNode : public LightNode {
public:
	void CalculateLight(int* shaderArrayIndex) override;
	std::string GetType() override { return "PointLight"; }

	PointLightNode(float pos[4], Node* parent = nullptr) {
		memcpy(position, pos, sizeof(float) * 4);
		this->parent = parent;

		shaderArrayIndex = shaderArrayIndexCount;
		shaderArrayIndexCount++;
	}

private:
	Node* parent;
	static int shaderArrayIndexCount;
};

class DirectionalLightNode : public LightNode {
public:
	void CalculateLight(int* shaderArrayIndex) override;
	std::string GetType() override { return "DirectionalLight"; }


	DirectionalLightNode(float pos[4]) {
		memcpy(position, pos, sizeof(float) * 4);
	}
};

class SpotLightNode : public LightNode {
public:
	void CalculateLight(int* shaderArrayIndex) override;
	std::string GetType() override { return "SpotLight"; }

	SpotLightNode(float pos[4], float coneDir[4], float cutOff, Node* parent = nullptr) {

		memcpy(position, pos, sizeof(float) * 4);

		memcpy(coneDirection, coneDir, sizeof(float) * 4);

		this->cutOff = cutOff;

		this->parent = parent;

		shaderArrayIndex = shaderArrayIndexCount;
		shaderArrayIndexCount++;
	}

	float* GetPosition() override;

private:
	Node* parent;
	float coneDirection[4];
	float cutOff;
	static int shaderArrayIndexCount;
};
