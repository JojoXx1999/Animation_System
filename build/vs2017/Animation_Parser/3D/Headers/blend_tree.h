#pragma once
#include<map>
#include <animation/skeleton.h>
#include "motion_clip_player.h"

class BlendTree;

//Connector node in node editor
class BlendNode
{
public:
	BlendNode(BlendTree* tree);

	bool Update(float dt);
	void Start();

	//Will be overritten by child classes
	virtual bool Process(float dt) = 0;
	virtual void StartInternal() {};

	//set animation
	void setInput(int input_num, BlendNode* node, float speed);

	//set variable such as animation blend
	void setVariable(float variable_num, const std::string variable);

	//input animations
	std::vector<BlendNode*> inputs;

	//variables
	std::map<int, std::string> variables;

	//change playback speed based on duration
	std::map<int, float> speeds;

	BlendTree* m_tree_cpy;
	bool set;

	//pose to be rendered
	gef::SkeletonPose m_output_pose;
};


//Output node in node editor
class OutputNode : public BlendNode
{
public:
	OutputNode(BlendTree* tree);
	bool Process(float dt);
};

class InputNode : public BlendNode
{
public:
	InputNode(BlendTree* tree);

	void Init();
	void setClip(const gef::Animation* animation);
	void StartInternal () override;
	bool Process(float dt) override;

	//animation input
	MotionClipPlayer m_clip_player;
};

//Parent blend tree
class BlendTree
{
public:
	BlendTree();
	~BlendTree();

	void Init(gef::SkeletonPose bind_pose);
	void Start();
	void Update(float dt);

	//Pointer to output node
	OutputNode* m_output;
	
	//mesh bind pose
	gef::SkeletonPose m_bind_pose;

	//all blend tree variabled
	std::map<std::string, float> m_variables;

	//output pose to be rendered
	gef::SkeletonPose m_output_pose;

	//Pointer to input node
	InputNode* node;
};

class Linear2BlendNode : public BlendNode
{
public:
	Linear2BlendNode(BlendTree* tree);

	//Blend the 2 input animations
	bool Process(float dt) override;
};