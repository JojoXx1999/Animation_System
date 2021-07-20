//-------------------------------------------------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------Node editor---------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
#pragma once
# include <imgui_node_editor.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//Short hand for the node editor
namespace ed = ax::NodeEditor;

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
class NodeEditor
{
public:
	NodeEditor();
	~NodeEditor();

	void Init();
	void Frame(bool* pressed, int* i);									//Display the node editor ------------------------------------------//
	void UpdateName(int id, std::string name);							//Change the name of the Input Node --------------------------------//

private:
	void BeginColumn();													//Begin a new node column ------------------------------------------//
	void NextColumn();													//Move to the new node column --------------------------------------//
	void EndColumn();													//End a node column ------------------------------------------------//

	void CreateLink();													//Create a new link between nodes ----------------------------------//
	void DeleteLink();													//Delete a link between nodes --------------------------------------//

	//Create a new node
	void CreateNode(ed::NodeId& Id, std::string& name, ImVec2 position, int inputs, int outputs, int& uniqueId);

	//Create a new pin
	void CreatePin(ed::PinId& Id, ed::PinKind input, std::string& text);

	//Cretae a new connctor (custom node type)
	void CreateConnector(ed::NodeId Id, int& uniqueI, int count);

	//Create a new input node (custom node type)
	bool CreateInput(ed::NodeId Id, int& uniqueI, int count, std::string name);

	//Create a new output node (custom node type)
	void CreateOutput(ed::NodeId Id, int& uniqueI);

	//Number of input, output pins and connector nodes with their Id's
	std::vector<ed::PinId> currentNodeInputs;
	std::vector<ed::PinId> currentNodeOutputs;	
	std::vector<ed::NodeId> connectorNodes;

	//Map of input nodes with their Id and name
	std::map<int, std::string> inputNodes;

	//booleans for creating new nodes and pins
	bool newNode, newPinInput, newPinOutput;

	//name of input node
	std::string image;
};
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
