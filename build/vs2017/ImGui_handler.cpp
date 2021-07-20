#include "ImGui_handler.h"


#include "Animation_Parser/2D/Headers/2D_animation_handler.h"
#include "Animation_Parser/3D/Headers/3D_animation_handler.h"

#include <iostream>
#include <fstream>



ImGuiHandler::ImGuiHandler(std::string& image, AnimationHandler2D& m_animation_handler_2D_, AnimationHandler3D& m_animation_handler_3D_, std::string& animation, gef::Platform& Platform)
	: image_cpy(image), m_animation_handler_2D_cpy(m_animation_handler_2D_), m_animation_handler_3D_cpy(m_animation_handler_3D_), animation_cpy(animation), Platform_cpy(Platform)
{


	m_show_properties = false;
	m_node_editor = false;
	m_diag_toggle = true;
	m_toggle = false;
	m_current_anim_toggle = false;

	mesh_file = false;
	refresh = true;
	is2D = false;
	m_help_toggle = false;
	m_2d_documentation = false;
	m_3d_documentation = false;
	m_full_documentation = false;
	m_bugs = false;
	m_blend_tree_toggle = false;
	inputNode = false;
	inputNodeCpy = false;
	nodeId = -1;
	count = 1;

	solutionDir = path = std::experimental::filesystem::current_path();
	path_string = path.u8string();

	previous_filename = filename = "NULL";

	std::string file = "Dragon_tex.png";
	
	m_node = new NodeEditor();

	std::ifstream infile;

	//Read text files for help menu
	infile.open("Bugs.txt");
	std::string tmpTxt = "";
	if (infile.is_open()) {
		while (!infile.eof())
		{
			std::getline(infile, tmpTxt);
			m_bugsFile.push_back(tmpTxt);
		}
	}
	infile.close();

	infile.open("Doc_2D.txt");
	tmpTxt = "";
	if (infile.is_open()) {
		while (!infile.eof())
		{
			std::getline(infile, tmpTxt);
			textFile_2D.push_back(tmpTxt);
		}
	}
	infile.close();

	infile.open("Doc_3D.txt");
	tmpTxt = "";
	if (infile.is_open()) {
		while (!infile.eof())
		{
			std::getline(infile, tmpTxt);
			textFile_3D.push_back(tmpTxt);
		}
	}
	infile.close();

	infile.open("Doc_standard.txt");
	tmpTxt = "";
	if (infile.is_open()) {
		while (!infile.eof())
		{
			std::getline(infile, tmpTxt);
			textFile_standard.push_back(tmpTxt);
		}
	}
	infile.close();

}

ImGuiHandler::~ImGuiHandler()
{
	delete m_node;
}

void ImGuiHandler::Init()
{
	//start gui
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiHandler::MenuBar()
{
	//create menu
	Init();

	ImGui::StyleColorsClassic();

	ImGui::BeginMainMenuBar();

	File();

	Project();

	Debug();

	Help();

	ImGui::EndMainMenuBar();

	End();
}

void ImGuiHandler::Debug()
{
	//display debug and its options
	if (ImGui::BeginMenu("Debug"))
	{
		ImGui::MenuItem("Diagnostics", NULL, &m_diag_toggle);

		ImGui::EndMenu();
	}

	if (m_diag_toggle)
	{
		Diagnostics();
	}
}

void ImGuiHandler::Diagnostics()
{
	//display diagnostics window
		ImGui::Begin("Diagnostics", &m_diag_toggle);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();

}

void ImGuiHandler::Help()
{
	//display help and its options
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::BeginMenu("Documentation"))
		{

			ImGui::MenuItem("2D", NULL, &m_2d_documentation);
			ImGui::MenuItem("3D", NULL, &m_3d_documentation);
			ImGui::MenuItem("Full", NULL, &m_full_documentation);

			ImGui::EndMenu();
		}
		ImGui::MenuItem("Bugs", NULL, &m_bugs);
		ImGui::EndMenu();

	}

	if (m_2d_documentation || m_3d_documentation || m_full_documentation)
	{
		Documentation();
	}

	if (m_bugs)
	{
		//display bugs window
		ImGui::Begin("Known Bugs", &m_bugs);

		for (int i = 0; i < m_bugsFile.size(); i++)
		{
			ImGui::Text(m_bugsFile.at(i).c_str());
		}

		ImGui::End();
	}
}

void ImGuiHandler::Documentation()
{
	static const ImVec4 Color{ 1.f, 0.2f, 0.2f, 1.f };

	if (m_2d_documentation)
	{
		//display 2D documentation
		ImGui::Begin("2D Documentation", &m_2d_documentation);

		ImGui::PushStyleColor(ImGuiCol_Text, Color);
		ImGui::Text("2D");
		ImGui::PopStyleColor(1);
		for (int i = 0; i < textFile_2D.size(); ++i)
		{
			ImGui::Text(textFile_2D.at(i).c_str());
		}


		ImGui::End();
	}


	if (m_3d_documentation)
	{
		//display 3d documentaion
		ImGui::Begin("3D Documentation", &m_3d_documentation);

		ImGui::PushStyleColor(ImGuiCol_Text, Color);
		ImGui::Text("3D");
		ImGui::PopStyleColor(1);
		for (int i = 0; i < textFile_3D.size(); ++i)
		{
			ImGui::Text(textFile_3D.at(i).c_str());
		}
		ImGui::End();
	}

	if (m_full_documentation)
	{
		//display full documentation
		ImGui::Begin("Full Documentation", &m_full_documentation);

		for (int i = 0; i < textFile_standard.size(); ++i)
		{
			ImGui::Text(textFile_standard.at(i).c_str());
		}

		ImGui::Spacing();

		ImGui::PushStyleColor(ImGuiCol_Text, Color);
		ImGui::Text("2D");
		ImGui::PopStyleColor(1);
		for (int i = 0; i < textFile_2D.size(); ++i)
		{
			ImGui::Text(textFile_2D.at(i).c_str());
		}

		ImGui::Spacing();

		ImGui::PushStyleColor(ImGuiCol_Text, Color);
		ImGui::Text("3D");
		ImGui::PopStyleColor(1);
		for (int i = 0; i < textFile_3D.size(); ++i)
		{
			ImGui::Text(textFile_3D.at(i).c_str());
		}
		ImGui::End();
	}
}

void ImGuiHandler::File()
{
	if (ImGui::BeginMenu("File"))
	{
		//start file and all of it's options
		ImGui::MenuItem("Load Mesh or Texture", NULL, &m_current_anim_toggle);

		ImGui::MenuItem("Close Application", NULL, &m_toggle);

		ImGui::EndMenu();
	}


	if (m_toggle)
	{
		//end the application
		exit(1);
	}

	if (m_current_anim_toggle)
	{
		//Load new mesh from file system
		LoadNewMesh();

		if (mesh_file)
		{
			LoadMesh();
		}

		//update filepath
		if (refresh == false)
		{
			Refresh(path);
			refresh = true;
		}
	}
	else if (refresh == true)
		refresh = false;
}

void ImGuiHandler::LoadMesh()
{
	//load in mesh or texture
		if (!m_animation_handler_2D_cpy.LoadFile(&image_cpy[0]))
		{
			modelType = ModelType::D3;
			m_animation_handler_3D_cpy.setFilename(&image_cpy[0]);
			m_animation_handler_3D_cpy.Load(&image_cpy[0], animation_cpy, m_animation_handler_3D_cpy.getCurrentAnim());
			m_current_anim_toggle = false;
		}
		else
		{
			modelType = ModelType::D2;
			m_animation_handler_2D_cpy.setFilename(&image_cpy[0]);
			m_animation_handler_2D_cpy.LoadFile(&image_cpy[0]);
			m_current_anim_toggle = false;
		}

}

void ImGuiHandler::LoadNewMesh()
{
	if (ShowDirectory() == true)
	{
		std::string cpy = tmp_cpy;
		//cpy.erase(cpy.begin(), cpy.begin() + (solutionDir.u8string().length() + 1));

		if (image_cpy != &cpy[0])
		{
			//remove file extensions
			if (is2D == true)
				cpy.erase(cpy.end()-9, cpy.end());
			else
				cpy.erase(cpy.end() - 4, cpy.end());

			image_cpy = &cpy[0];
			Properties(true);
		}
	}
}

bool ImGuiHandler::ShowDirectory()
{
	//show file file directory
		ImGui::Begin("Directory", &m_current_anim_toggle);

		
		if (m_current_anim_toggle == false)
		{
			inputNodeCpy = false;
		}
		

		if (ImGui::InputText("FilePath", &path_string[0], 256, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			Refresh(path_string);
		}

		//colours to highlight files of interest
		static const ImVec4 BackButtonColor{ 0.53f, 0.2f, 0.28f, 1.0f };
		static const ImVec4 BackTextColor{ 0.9f, 0.56f, 0.76f, 1.f };
		ImGui::PushStyleColor(ImGuiCol_Text, BackTextColor);
		ImGui::PushStyleColor(ImGuiCol_Button, BackButtonColor);
		if (ImGui::Button("<<"))
		{
			//go back in the file directory
			size_t pos = path_string.find_last_of("\\");
			path_string.erase(pos);
			Refresh(path_string);
		}
		ImGui::PopStyleColor(2);

		for (const auto & entry : std::experimental::filesystem::directory_iterator(path))
		{
			std::string tmp = entry.path().u8string();
			tmp_cpy = tmp;
			std::string isFileType = tmp;
			std::string result = isFileType.substr(isFileType.length() - 4);
			std::string result2 = isFileType.substr(isFileType.length() - 5);
			tmp.erase(tmp.begin(), tmp.begin() + path_string.size()+1);

			static const ImVec4 ButtonColorImportant{ 0.24f, 0.24f, 0.16f, 1.0f };
			static const ImVec4 TextColorImportant{ 0.81f, 0.81f, 0.4f, 1.f };

			static const ImVec4 ButtonColor{ 0.2f, 0.2f, 0.23f, 1.0f };
			static const ImVec4 TextColor{ 0.63f, 0.63f, 0.63f, 1.f };

			int counter = 0;

				for (const auto & entry2 : std::experimental::filesystem::directory_iterator(entry))
				{
					counter++;
					if (counter > 0)
					{
						break;
					}

				}

			if (result == ".scn" || counter > 0 || result2 == ".json")
			{
				//if .scn or .json render as a yellow button
				ImGui::PushStyleColor(ImGuiCol_Text, TextColorImportant);
				ImGui::PushStyleColor(ImGuiCol_Button, ButtonColorImportant);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, TextColor);
				ImGui::PushStyleColor(ImGuiCol_Button, ButtonColor);
			}


			if (ImGui::Button(&tmp[0]))
			{

				if (result == ".scn" || result2 == ".json")
				{
					//get file type
					if (result2 == ".json")
						is2D = true;
					else
						is2D = false;

					Refresh(entry);
					ImGui::PopStyleColor(2);
					ImGui::End();

					filename = &tmp[0];
					return true;
				}
				Refresh(entry);
					ImGui::PopStyleColor(2);
				break;
			}
			else
			{
				ImGui::PopStyleColor(2);
			}
		}

		ImGui::End();
		return false;
}

void ImGuiHandler::Refresh(const std::experimental::filesystem::v1::directory_entry& entry)
{
	//update file path
	if (std::experimental::filesystem::is_directory(entry))
	{
		path = entry;
		path_string = entry.path().u8string();
	}
}

void ImGuiHandler::Refresh(const std::experimental::filesystem::path& entry)
{
	//update file path
	if (std::experimental::filesystem::is_directory(entry))
	{
		path = entry;
		path_string = entry.u8string();
	}
}

void ImGuiHandler::Project()
{
	//display project menu and it's options
	if (ImGui::BeginMenu("Project"))
	{
		ImGui::MenuItem("Node Editor", NULL, &m_node_editor);
		ImGui::MenuItem("Properties", NULL, &m_show_properties);

		ImGui::EndMenu();
	}

	if (m_show_properties)
	{
		Properties(false);
	}

	if (m_node_editor)
	{
		NodeEditorGUI();
	}
}

void ImGuiHandler::NodeEditorGUI()
{
	//display the node editor
	ImGui::Begin("Node Editor", &m_node_editor);

		m_node->Frame(&inputNode, &nodeId);

		ImGui::End();

		LoadInputNodes();

		if (inputNodeCpy == true)
		{
			if (filename != previous_filename)
			{
				previous_filename = filename;
				m_node->UpdateName(nodeId, filename);
			}
		}


}

void ImGuiHandler::Properties(bool over_ride_enter)
{
	//display properties menu
	ImGui::Begin("Properties", &m_show_properties);

	if (ImGui::InputText("File Name", &image_cpy[0], 256, ImGuiInputTextFlags_EnterReturnsTrue) || over_ride_enter == true)
	{
		LoadMesh();
		//load inputted mesh in file bar
	}
	for (int i = 0; i < 5; ++i) ImGui::Spacing();


	//display gui properties based on animation type
	if (modelType == D2)
	{
		if (ImGui::Button("3D animation"))
		{
			modelType = ModelType::D3;
			m_animation_handler_3D_cpy.Load(&image_cpy[0], animation_cpy, m_animation_handler_3D_cpy.getCurrentAnim());
			animation_cpy = solutionDir.u8string() + "/" + animation_cpy;
			image_cpy = m_animation_handler_3D_cpy.getFilename();
		}
		m_animation_handler_2D_cpy.gui(Platform_cpy);

		if (m_animation_handler_2D_cpy.getToggle())
		{
			m_animation_handler_2D_cpy.setToggle(false);
			image_cpy = m_animation_handler_2D_cpy.getFilename();
		}
	}
	else
	{
		if (ImGui::Button("2D Animation"))
		{
			modelType = ModelType::D2;
			m_animation_handler_2D_cpy.LoadFile(&image_cpy[0]);
			image_cpy = m_animation_handler_2D_cpy.getFilename();
		}
		m_animation_handler_3D_cpy.gui(image_cpy, animation_cpy);
	}

	

	for (int i = 0; i < 5; ++i) ImGui::Spacing();
	ImGui::End();
}

void ImGuiHandler::End()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiHandler::LoadInputNodes()
{
	if (inputNode == true)
	{
		inputNodeCpy = true;
	}
}