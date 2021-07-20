#include "Animation_Parser/3D/Headers/blend_tree.h"

BlendTree::BlendTree()
{
	//create new output and input nodes
	m_output = new OutputNode(this);
	node = new InputNode(this);
}

BlendTree::~BlendTree()
{
	//delete pointers
	delete m_output;
	delete node;
}

void BlendTree::Init(gef::SkeletonPose bind_pose)
{
	//set output to the bind pose
	m_output_pose = bind_pose;
	m_bind_pose = bind_pose;

	//initilaise input node
	node->Init();
}

void BlendTree::Start()
{
	m_output->Start();
}

void BlendTree::Update(float dt)
{
	m_output->Update(dt);
}

//------------------------------------------------------------------------------------------------//

BlendNode::BlendNode(BlendTree* tree)

{
	//make a copy of the blend tree
	m_tree_cpy = tree;

	//set the output pose to the bind pose
	m_output_pose = m_tree_cpy->m_bind_pose;
	set = false;
}

void BlendNode::Start()
{
	//for each input
	for (int i = 0; i < inputs.size(); ++i)
	{
		BlendNode& input = *inputs[i];

		//Start the input node
		if (&input)
			input.Start();
	}

	StartInternal();
}

void BlendNode::setInput(int id, BlendNode* node, float speed)
{
	if (node && id < inputs.size())
	{
		//set input at the id to the node
		inputs[id] = node;

		//insert the animation speed into a map
		speeds.insert(std::pair<int, float>(id, speed));
	}


}

void BlendNode::setVariable(float id, const std::string variable)
{
	//insert the variable in the variables maps
	m_tree_cpy->m_variables.insert(std::pair<std::string, float>(&variable[0], id));
	variables.insert(std::pair<int, std::string>(id, &variable[0]));
}

bool BlendNode::Update(float dt)
{
	bool valid_input{ true };
	bool valid_output{ false };

	//if there is an input
	if (inputs.size() > 0)
	{
		//for each input
		for (int i = 0; i < inputs.size(); ++i)
		{
			BlendNode& input = *inputs[i];
			bool valid;

			//update the input node
			if (&input)
				valid = input.Update(dt);
			else
				valid = false;

			if (valid_input && !valid)
				valid_input = false;
		}
	}
	
	//Run the overriden Process functions
	if (valid_input)
		valid_output = Process(dt);

	return valid_output;

}
//------------------------------------------------------------------------------------------------//

InputNode::InputNode(BlendTree* tree)
	:BlendNode(tree)
{
	m_tree_cpy = tree;
	Init();
}

void InputNode::Init()
{
	//set the clip node to the bind pose
	if (m_tree_cpy)
		m_clip_player.Init(m_tree_cpy->m_bind_pose);

	m_clip_player.set_looping(true);
}

void InputNode::setClip(const gef::Animation* animation)
{
	//set the clip to the passed in animation
	m_clip_player.set_clip(animation);
}

void InputNode::StartInternal()
{
	//set the the start of the animation
	m_clip_player.set_anim_time(0.f);
}

bool InputNode::Process(float dt)
{
	bool valid = false;
	if (m_clip_player.clip())
	{
		//Update the animation
		m_clip_player.Update(dt, m_tree_cpy->m_bind_pose);

		//set the output pose to the current animation pose
		m_output_pose = m_clip_player.pose();
		valid = true;
	}
	return valid;

}


//------------------------------------------------------------------------------------------------//

OutputNode::OutputNode(BlendTree* tree)
	:BlendNode(tree)
{
	inputs.resize(1);
}

bool OutputNode::Process(float dt)
{
	//set the output pose to the blend tree output pose
	m_output_pose = inputs[0]->m_output_pose;
	return true;
}

//------------------------------------------------------------------------------------------------//

Linear2BlendNode::Linear2BlendNode(BlendTree* tree)
	:BlendNode(tree)
{
	inputs.resize(2);
}

bool Linear2BlendNode::Process(float dt)
{
	//get the 0th element in the variabled map
	float blend_value = m_tree_cpy->m_variables.find(variables.find(0)->second)->second;

	//blend the two animations together using the blend value
	m_output_pose.Linear2PoseBlend(inputs[0]->m_output_pose, inputs[1]->m_output_pose, blend_value);
	return true;
}
