#include "node_editor.h"

struct LinkInfo
{
	ed::LinkId Id;
	ed::PinId  InputId;
	ed::PinId  OutputId;
};

static ed::EditorContext*   g_Context = nullptr;
static bool                 g_FirstFrame = true;
static ImVector<LinkInfo>   g_Links;
static int                  g_NextLinkId = 100;

NodeEditor::NodeEditor()
{
	Init();

	newNode = false;
	newPinOutput = false;
	newPinInput = false;
	image = "NULL";
}

NodeEditor::~NodeEditor()
{
	ed::DestroyEditor(g_Context);
}

void NodeEditor::Init()
{
	ed::Config config;
	config.SettingsFile = "BasicInteraction.json";
	g_Context = ed::CreateEditor(&config);
}

void NodeEditor::BeginColumn()
{
	ImGui::BeginGroup();
}

void NodeEditor::NextColumn()
{
	ImGui::EndGroup();
	ImGui::SameLine();
	ImGui::BeginGroup();
}

void NodeEditor::EndColumn()
{
	ImGui::EndGroup();
}

void NodeEditor::Frame(bool* pressed, int* id)
{
	int uniqueId = 1;

	if (ImGui::Button("Create Connector"))
	{
		ed::NodeId Id = uniqueId + 1;
		connectorNodes.push_back(Id);
	}

	ImGui::SameLine();

	if (ImGui::Button("Create Input"))
	{
		ed::NodeId Id = uniqueId + 1;
		inputNodes.insert(std::pair<int, std::string>(inputNodes.size(), &image[0]));
	}
	else if (inputNodes.size() < 1)
	{
		ed::NodeId Id = uniqueId + 1;
		inputNodes.insert(std::pair<int, std::string>(0, &image[0]));
	}

	ImGui::SameLine();

	ed::SetCurrentEditor(g_Context);
	ed::Begin("My Editor");

	ed::NodeId new_Id = uniqueId++;

	CreateOutput(new_Id, uniqueId);

	for (int i = 0; i < inputNodes.size(); ++i)
	{
		ed::NodeId new_Id = uniqueId++;
		*pressed = CreateInput(new_Id, uniqueId, i, inputNodes.find(i)->second);

		if (*pressed == true)
		{
			*id = i;
		}
	}


	for (int i = 0; i < connectorNodes.size(); ++i)
	{
		ed::NodeId new_Id = uniqueId++;
		CreateConnector(new_Id, uniqueId, i+1);
	}

	for (auto& linkInfo : g_Links)
		ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);

	CreateLink();

	if (g_Links.size() > 1)
	{
		g_Links;
		int y = 0;
	}

	DeleteLink();

	ed::End();

	if (g_FirstFrame)
		ed::NavigateToContent(0.f);

	ed::SetCurrentEditor(nullptr);

	g_FirstFrame = false;
}

void NodeEditor::UpdateName(int id, std::string name)
{
	if (inputNodes.find(id) != inputNodes.end())
	{
		inputNodes.find(id)->second = name;
	}
}

void NodeEditor::CreateConnector(ed::NodeId Id, int& uniqueId, int count)
{
	
	CreateNode(Id, std::string{ "Connector: " + std::to_string(count) }, ImVec2(210, 60), 2, 1, uniqueId);
	BeginColumn();
	CreatePin(currentNodeInputs[0], ed::PinKind::Input, std::string{ "-> In1" });
	CreatePin(currentNodeInputs[1], ed::PinKind::Input, std::string{ "-> In2" });
	NextColumn();
	CreatePin(currentNodeOutputs[0], ed::PinKind::Output, std::string{ "Out ->" });
	EndColumn();
	ed::EndNode();
}

void NodeEditor::CreateOutput(ed::NodeId Id, int& uniqueId)
{
	CreateNode(Id, std::string{ "Output"}, ImVec2(210, 60), 1, 0, uniqueId);
	BeginColumn();
	CreatePin(currentNodeInputs[0], ed::PinKind::Input, std::string{ "-> In" });
	EndColumn();
	ed::EndNode();
}

bool NodeEditor::CreateInput(ed::NodeId Id, int& uniqueId, int count, std::string name)
{
	bool button = false;
	CreateNode(Id, std::string{ name }, ImVec2(210, 60), 0, 1, uniqueId);
	BeginColumn();
	std::string tmp = std::to_string(count  + 1);
	std::string Anim_name = "Animation " + tmp;
	if (ImGui::Button(Anim_name.c_str()))
	{
		button = true;
	}
	NextColumn();
	CreatePin(currentNodeOutputs[0], ed::PinKind::Output, std::string{ "Out ->" });
	EndColumn();
	ed::EndNode();

	return button;
}

void NodeEditor::CreateNode(ed::NodeId& Id, std::string& name, ImVec2 position, int inputs, int outputs, int& uniqueId)
{
	currentNodeInputs.clear();
	currentNodeOutputs.clear();

	for (int i = 0; i < inputs; ++i)
	{
		currentNodeInputs.push_back(uniqueId++);
	}

	for (int i = 0; i < outputs; ++i)
	{
		currentNodeOutputs.push_back(uniqueId++);
	}

	/*
	if (g_FirstFrame)
		ed::SetNodePosition(Id, position);
		*/

	ed::BeginNode(Id);
	ImGui::Text(name.c_str());

}

void NodeEditor::CreatePin(ed::PinId& Id, ed::PinKind input, std::string& text)
{
	ed::BeginPin(Id, input);
	ImGui::Text(text.c_str());
	ed::EndPin();
}

void NodeEditor::CreateLink()
{
	if (ed::BeginCreate())
	{
		ed::PinId inputPinId, outputPinId;

		if (ed::QueryNewLink(&inputPinId, &outputPinId))
		{

			if (inputPinId && outputPinId)
			{
				// ed::AcceptNewItem() return true when user release mouse button.
				if (ed::AcceptNewItem())
				{
					g_Links.push_back({ ed::LinkId(g_NextLinkId++), inputPinId, outputPinId });

					ed::Link(g_Links.back().Id, g_Links.back().InputId, g_Links.back().OutputId);
				}
			}
		}
	}
	ed::EndCreate();
}

void NodeEditor::DeleteLink()
{
	if (ed::BeginDelete())
	{
		ed::LinkId deletedLinkId;
		while (ed::QueryDeletedLink(&deletedLinkId))
		{
			if (ed::AcceptDeletedItem())
			{
				for (auto& link : g_Links)
				{
					if (link.Id == deletedLinkId)
					{
						g_Links.erase(&link);
						break;
					}
				}
			}
		}
	}
	ed::EndDelete();
}
