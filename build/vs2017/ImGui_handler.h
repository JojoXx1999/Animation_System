//-------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------ImGui Handler---------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
#pragma once
#include <string.h>
#include <string>
#include <iostream>
#include <filesystem>

#include "node_editor.h"
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
//Forward Declaration

class AnimationHandler2D;
class AnimationHandler3D;

namespace gef
{
	class Platform;
}
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//
class ImGuiHandler
{
public:
	//Initialise gui and create copies of objects
	ImGuiHandler::ImGuiHandler(std::string& image, AnimationHandler2D& m_animation_handler_2D_, AnimationHandler3D& m_animation_handler_3D_, std::string& animation, gef::Platform& Platform);
	ImGuiHandler::~ImGuiHandler();

	void MenuBar();																	//Display the menu bar on the gui -----------------------//

	enum ModelType { D2 = 2, D3 };													//Animation types, 2D or 2D -----------------------------//
	ModelType modelType;															//Current animation type --------------------------------//

private:
	void Init();																	//Initialise the gui ------------------------------------//
	void File();																	//File option on menu -----------------------------------//
	void Debug();																	//Debug option on menu ----------------------------------//
	void Project();																	//Project option on menu --------------------------------//
	void End();																		//End the menu ------------------------------------------//
	void Help();																	//Help option on menu -----------------------------------//

private:
	void Properties(bool over_ride_enter);											//Properties window accessed in Project -----------------//
	void Diagnostics();																//Diagnostics window accessed in Debug ------------------//
	void Documentation();															//Documentation accessed in Help ------------------------//
	void NodeEditorGUI();															//Node editor accessed in Project -----------------------//	
	void LoadInputNodes();															//Load Input nodes for node editor ----------------------//
	void LoadNewMesh();																//If correct file type selected from file system --------//
	void LoadMesh();																//Load the new mesh -------------------------------------//
	bool ShowDirectory();															//Open up file system -----------------------------------//

	//Overloaded Refresh, change directory path from selected file in filepath
	void Refresh(const std::experimental::filesystem::v1::directory_entry& entry);
	void Refresh(const std::experimental::filesystem::path& entry);

	//Booleans for gui options
	bool m_show_properties, m_camera, m_toggle, m_node_editor, m_diag_toggle, m_current_anim_toggle;
	bool refresh, mesh_file, m_help_toggle;

																					
	std::string& image_cpy, &animation_cpy;											//File and animation names for gui ----------------------//
	gef::Platform& Platform_cpy;													//Copy application platform so it is only passed once ---//

	//Copy of animation handlers to access functions
	AnimationHandler2D& m_animation_handler_2D_cpy;
	AnimationHandler3D& m_animation_handler_3D_cpy;

	//Varaibles for file system
	std::vector< std::experimental::filesystem::v1::path> folder;
	std::experimental::filesystem::path solutionDir;								//Starting solution directory ---------------------------//
	std::experimental::filesystem::path path;										//Current file path -------------------------------------//
	std::string path_string;														//string of file path -----------------------------------//

	std::string tmp_cpy;															//Store temporary strings -------------------------------//

	//Booleans for Help 
	bool is2D, m_2d_documentation, m_3d_documentation, m_back_button, m_full_documentation, m_bugs;

	//Store text loaded in from external files for Help
	std::vector<std::string> textFile_2D, textFile_3D, textFile_standard, m_bugsFile;

	//Booleans for blend tree and node editor
	bool m_blend_tree_toggle, inputNode, inputNodeCpy;

	//Node editor
	NodeEditor* m_node;
	int nodeId, count;
	
	std::string filename, previous_filename;										//Make sure new file is different from current ----------//

};
//-------------------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------------------------------//