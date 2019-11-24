#include <Nero/editor/ProjectManager.h>
#include <Nero/core/utility/FileUtil.h>
#include <Nero/core/utility/DateUtil.h>
#include <Nero/core/utility/LogUtil.h>
#include <experimental/filesystem>
//STD
#include <windows.h>
#include <stdio.h>
#include <boost/algorithm/string.hpp>
#include <boost/dll/import.hpp>

#include <boost/dll.hpp>

namespace nero
{
    ProjectManager::ProjectManager():
         m_ProjectTable()
        ,m_EditorPid(StringPool.BLANK)
        ,m_GameProject()
		,m_Setting(nullptr)
    {

    }


    void ProjectManager::createProject(const nlohmann::json& projectJson, int& status)
    {
        //Step 1 : Create directory struture
        std::string project_name    = projectJson["project_name"].get<std::string>();
        std::string workspace_name  = projectJson["workspace_name"].get<std::string>();
        std::string project_namspace = projectJson["project_namspace"].get<std::string>();
        std::string project_lead = projectJson["project_lead"].get<std::string>();

        nlohmann::json projectWorkpsace = findWorkspace(workspace_name);

        //assert(!isProjectExist(project_name));

        //get project name
        std::string project_id = formatString(project_name);

        nero_log(project_name);
        nero_log(projectWorkpsace);

         std::cout << "running 1 .." << std::endl;

        std::string projectFolder = getPath({projectWorkpsace["workspace_directory"].get<std::string>(), project_id});

        status = 1;

        //create project directory
        createDirectory(projectFolder);
        createDirectory(getPath({projectFolder, "Source"}));
        createDirectory(getPath({projectFolder, "Build"}));
        createDirectory(getPath({projectFolder, "Scene"}));
        createDirectory(getPath({projectFolder, "Setting"}));

        //project json
        nlohmann::json project_json;
        project_json["project_name"]            = project_name;
        project_json["project_id"]              = project_id;
        project_json["creation_date"]           = formatDateTime(getCurrentDateTime());
        project_json["modification_date"]       = formatDateTime(getCurrentDateTime());
        project_json["project_lead"]            = projectJson["project_lead"].get<std::string>();
        project_json["project_company"]         = projectJson["project_company"].get<std::string>();
        project_json["project_description"]     = projectJson["project_description"].get<std::string>();

        status = 2;

        //save project json
        saveFile(getPath({projectFolder, "nero_project"}, StringPool.EXTENSION_JSON), project_json.dump(3));

        //Step 2 : Generate file
        std::string cmake_template = loadText("template/cpp-project/CMakeLists.txt");
        std::string scene_header_template = loadText("template/cpp-project/CppScene.h");
        std::string scene_source_template = loadText("template/cpp-project/CppScene.cpp");

        auto wordTable = getWordTable(project_name);

        std::string scene_class = formatSceneClassName(wordTable);
        std::string class_header = formatHeaderGard(wordTable);

        //file 1 : header
        boost::algorithm::replace_all(scene_header_template, "::Scene_Class::", scene_class);
        boost::algorithm::replace_all(scene_header_template, "::Namespace::", project_namspace);
        boost::algorithm::replace_all(scene_header_template, "::Header_Gard::", class_header);
        boost::algorithm::replace_all(scene_header_template, "::Project_Name::", project_name);
        boost::algorithm::replace_all(scene_header_template, "::Project_Lead::", project_lead);
        boost::algorithm::replace_all(scene_header_template, "::Coypright_Date::", toString(getCurrentDateTime().date().year()));

        //file 2 : source
        boost::algorithm::replace_all(scene_source_template, "::Scene_Class::", scene_class);
        boost::algorithm::replace_all(scene_source_template, "::Namespace::", project_namspace);
        boost::algorithm::replace_all(scene_source_template, "::Project_Name::", project_name);
        boost::algorithm::replace_all(scene_source_template, "::Project_Lead::", project_lead);
        boost::algorithm::replace_all(scene_source_template, "::Coypright_Date::", toString(getCurrentDateTime().date().year()));

        //file 3 : cmake
        boost::algorithm::replace_all(cmake_template, "::Project_Name::", formatCmakeProjectName(wordTable));
        boost::algorithm::replace_all(cmake_template, "::Engine_Installation_Directory::", getEngineDirectory());
        boost::algorithm::replace_all(cmake_template, "::Project_Library::", formatCmakeProjectLibrary(wordTable));


        saveFile(getPath({projectFolder, "Source", "CMakeLists"}, StringPool.EXTENSION_TEXT), cmake_template);
        saveFile(getPath({projectFolder, "Source", scene_class}, StringPool.EXTENSION_CPP_HEADER), scene_header_template);
        saveFile(getPath({projectFolder, "Source", scene_class}, StringPool.EXTENSION_CPP_SOURCE), scene_source_template);

        status = 3;

        //Step 3 : compile the project
        compileProject(projectFolder);

        //openEditor

        status = 4;

        //lua project
        //create scene folder, script, chunk, startup, saving
        //create resource folder
        //create setting folder
    }

    bool ProjectManager::isProjectExist(const std::string& projectName)
    {
        //check if project director exist
        if(!directoryExist(getPath({m_EditorSetting["workspace_folder"].get<std::string>(), formatString(projectName)})))
        {
            return false;
        }

        //check if project file exist
        std::string project_file = getPath({m_EditorSetting["workspace_folder"].get<std::string>(), formatString(projectName), "nero_project"}, StringPool.EXTENSION_JSON);
        if(!fileExist(project_file))
        {
            return false;
        }

        //check if project_id exist
        nlohmann::json project_json = loadJson(project_file, true);
        std::string project_id = project_json["project_id"].get<std::string>();
        if(project_id != formatString(projectName))
        {
            return false;
        }

        return true;
    }

	void ProjectManager::setSetting(const Setting::Ptr& setting)
    {
		m_Setting = setting;
    }

    void ProjectManager::loadAllProject()
    {
        using namespace  std::experimental::filesystem;
        path folder_path(m_EditorSetting["workspace_folder"].get<std::string>());

        directory_iterator it{folder_path};
        while (it != directory_iterator{})
        {
            std::string project_folder = it->path().string();
            std::string project_file = getPath({project_folder, "nero_project"}, StringPool.EXTENSION_JSON);

            if(fileExist(project_file))
            {
                m_ProjectTable.push_back(loadJson(project_file, true));
            }

            it++;
        }
    }

    const std::vector<nlohmann::json>& ProjectManager::getProjectTable() const
    {
        return m_ProjectTable;
    }

    const std::vector<nlohmann::json> ProjectManager::getWorkspaceProjectTable(const std::string& workspace_name)
    {
        std::vector<nlohmann::json> result;

        nlohmann::json projectWorkpsace = findWorkspace(workspace_name);

        using namespace  std::experimental::filesystem;
        path folder_path(projectWorkpsace["workspace_directory"].get<std::string>());

        directory_iterator it{folder_path};
        while (it != directory_iterator{})
        {
            std::string project_folder = it->path().string();
            std::string project_file = getPath({project_folder, "nero_project"}, StringPool.EXTENSION_JSON);

            if(fileExist(project_file))
            {
                result.push_back(loadJson(project_file, true));
            }

            it++;
        }

        return result;
    }

	void ProjectManager::createWorkspace(const Setting& parameter)
    {
		nero_log("creating new project worksapce");
		nero_log(parameter.toString());

		std::string directory = getPath({parameter.getString("location"), parameter.getString("name")});
		createDirectory(directory);
		createDirectory(getPath({directory, "Project"}));
		createDirectory(getPath({directory, "Factory"}));
		createDirectory(getPath({directory, "Setting"}));
		createDirectory(getPath({directory, "Resource"}));
			createDirectory(getPath({directory, "Resource", "texture"}));
			createDirectory(getPath({directory, "Resource", "animation"}));
			createDirectory(getPath({directory, "Resource", "font"}));
			createDirectory(getPath({directory, "Resource", "sound"}));
			createDirectory(getPath({directory, "Resource", "music"}));
			createDirectory(getPath({directory, "Resource", "luascript"}));
			createDirectory(getPath({directory, "Resource", "shader"}));
			createDirectory(getPath({directory, "Resource", "language"}));
			createDirectory(getPath({directory, "Resource", "lightmap"}));

        //create nero_workspace.json
		saveFile(getPath({directory, ".workspace"}), parameter.toString());

        //update workspace setting
		auto worksapceSetting = m_Setting->getSetting("workspace");

		/*nlohmann::json workspace;
        workspace["workspace_id"]           = formatString(workspaceJson["workspace_name"].get<std::string>());
        workspace["workspace_name"]         = workspaceJson["workspace_name"];
        workspace["workspace_directory"]    = workspace_directory;
        workspace["default_project_lead"]   = workspaceJson["default_project_lead"];
        workspace["default_company_name"]   = workspaceJson["default_company_name"];
        workspace["default_namespace"]       = workspaceJson["default_namespace"];
        workspace["count_workspace"]        = worksapceSetting.size() + 1;

		worksapceSetting.push_back(workspace);*/

		//saveFile(getPath({"workspace", "workspace"}, StringPool.EXTENSION_JSON), worksapceSetting.dump(3), true);
    }

	void ProjectManager::importWorkspace(const std::string& directory)
	{

	}


    const nlohmann::json ProjectManager::getWorkspaceTable() const
    {
        return loadJson(getPath({"workspace", "workspace"}));
    }

    const std::vector<std::string> ProjectManager::getWorkspaceNameTable() const
    {
        auto workspaceTable =  loadJson(getPath({"workspace", "workspace"}));

         std::vector<std::string> result;

         for(auto workspace : workspaceTable)
         {
             result.push_back(workspace["workspace_name"].get<std::string>());
         }

         return result;
    }

    const nlohmann::json ProjectManager::findWorkspace(const std::string& name) const
    {
        auto workspaceTable =  loadJson(getPath({"workspace", "workspace"}));

        nlohmann::json result;

        for(auto workspace : workspaceTable)
        {
            if(workspace["workspace_name"] == name)
            {
                result = workspace;
            }
        }

        return result;
    }



    void ProjectManager::compileProject(const std::string& projectDirectory)
    {
        //remote
        std::string sourcePath  = getPath({projectDirectory, "Source"});
        std::string buildPath   = getPath({projectDirectory, "Build"});

        std::string clean       = "mingw32-make -C \"" + buildPath + "\" -k clean";
        std::string run_cmake    = "cmake -G \"MinGW Makefiles\" -S \"" + sourcePath + "\" -B \"" + buildPath + "\"";
        std::string build       = "mingw32-make -C \"" + buildPath + "\"";

        system(clean.c_str());
        system(run_cmake.c_str());
        system(build.c_str());

        //system("mingw32-make -C \"C:\\Users\\sk-landry\\Desktop\\Bonjour\\startspace\\Build\" -k clean");
        //system("cmake -G \"MinGW Makefiles\" -S \"C:\\Users\\sk-landry\\Desktop\\Bonjour\\startspace\\Source\" -B \"C:\\Users\\sk-landry\\Desktop\\Bonjour\\startspace\\Build\"");
        //system("mingw32-make -C \"C:\\Users\\sk-landry\\Desktop\\Bonjour\\startspace\\Build\"");

        //WinExec("mingw32-make -C \"C:\\Users\\sk-landry\\Desktop\\Bonjour\\startspace\\Build\" -k clean", SW_HIDE);
        //WinExec("cmake -G \"MinGW Makefiles\" -S \"C:\\Users\\sk-landry\\Desktop\\Bonjour\\startspace\\Source\" -B \"C:\\Users\\sk-landry\\Desktop\\Bonjour\\startspace\\Build\"", SW_HIDE);
        //WinExec("mingw32-make -C \"C:\\Users\\sk-landry\\Desktop\\Bonjour\\startspace\\Build\"", SW_HIDE);
    }

    //Replace popen and pclose with _popen and _pclose for Windows
    std::string ProjectManager::exec(const char* cmd)
    {
        std::stringstream string_buffer;
        std::streambuf * cout_buffer = std::cout.rdbuf(string_buffer.rdbuf());

        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);

        if (!pipe)
        {
            throw std::runtime_error("popen() failed!");
        }

        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        {
            result += buffer.data();
        }

        std::cout.rdbuf(cout_buffer);

        return result;
    }

    void ProjectManager::openEditor(std::string cmake_project)
    {
        nero_log("cmake project : " + cmake_project);

        //get the list of qtcreator process
        std::string list_process    = "tasklist \/fo csv\| findstr \/i \"qtcreator\"";
        std::string processCSV      = exec(list_process.c_str());

        nero_log(processCSV);

        //If the Editor has been opened and are still available (has not been closed)
        if(m_EditorPid != StringPool.BLANK && processCSV.find(m_EditorPid) != std::string::npos)
        {
            //open it
            std::string cmd = "START \"\" qtcreator -pid " + m_EditorPid;
            nero_log(cmd);
            system(cmd.c_str());
        }
        else
        {
            //open a new process
            std::string open_editor = "START \"\" qtcreator \"" + cmake_project +"\"";
            system(open_editor.c_str());

            //save the process id
                 processCSV  = exec(list_process.c_str());
            auto processTab  = splitString(processCSV, '\r\n');

            nero_log(processTab.size());


            m_EditorPid = splitString(processTab.back(), ',').at(1);
            nero_log(m_EditorPid);

        }
    }

    void ProjectManager::editProject()
    {

    }

     void ProjectManager::loadLibrary()
     {

         boost::dll::fs::path full_path("C:/Users/sk-landry/Desktop/Bonjour/startspace/Build/liblearn-nero.dll");
         boost::dll::fs::path copy_path("C:/Users/sk-landry/Desktop/Bonjour/startspace/Build/liblearn-nero-copy.dll");

         if(!boost::dll::fs::exists(full_path))
             return;


         if(boost::dll::fs::exists(copy_path))
         {
             m_GameScene = nullptr;
             m_CreateCppSceneFn.clear();

            boost::dll::fs::remove(copy_path);
         }

         system("copy C:\\Users\\sk-landry\\Desktop\\Bonjour\\startspace\\Build\\liblearn-nero.dll C:\\Users\\sk-landry\\Desktop\\Bonjour\\startspace\\Build\\liblearn-nero-copy.dll");


             boost::dll::fs::path library_path("C:/Users/sk-landry/Desktop/Bonjour/startspace/Build");
             library_path /= "liblearn-nero-copy";


             nero_log("loading scene library");

            m_CreateCppSceneFn = boost::dll::import_alias<CreateCppSceneFn>(             // type of imported symbol must be explicitly specified
                 library_path,                                            // path to library
                 "create_scene",                                                // symbol to import
                 boost::dll::load_mode::append_decorations                              // do append extensions and prefixes
             );

            nero_log("loading done");




            m_GameScene = m_CreateCppSceneFn(Scene::Context());


			//nero_log(m_GameScene->getName());
     }

     std::string ProjectManager::formatSceneClassName(std::vector<std::string> wordTable)
     {
         std::string result = StringPool.BLANK;

         for(std::string s : wordTable)
         {
             if(s != StringPool.BLANK)
             {
                 boost::algorithm::to_lower(s);
                 s[0] = std::toupper(s[0]);
                 result += s;
             }
         }

         result += "Scene";

         return result;
     }

     std::string ProjectManager::formatHeaderGard(std::vector<std::string> wordTable)
     {

         std::string result = StringPool.BLANK;

         for(std::string s : wordTable)
         {
             if(s != StringPool.BLANK)
             {
                 boost::algorithm::to_upper(s);
                 result += s + "_";
             }
         }

         result += "H";

         return result;
     }

     std::string ProjectManager::formatCmakeProjectName(std::vector<std::string> wordTable)
     {

         std::string result = StringPool.BLANK;

         for(std::string s : wordTable)
         {
             if(s != StringPool.BLANK)
             {
                 boost::algorithm::to_lower(s);
                 s[0] = std::toupper(s[0]);
                 result += s + " ";
             }
         }

         result.pop_back();
         result = "Project " + result;

         return result;
     }


     std::string ProjectManager::formatCmakeProjectLibrary(std::vector<std::string> wordTable)
     {

         std::string result = StringPool.BLANK;

         for(std::string s : wordTable)
         {
             if(s != StringPool.BLANK)
             {
                 boost::algorithm::to_lower(s);
                 result += s;
             }
         }

         result = "nerogame-" + result;

         return result;
     }

     std::string ProjectManager::getEngineDirectory() const
     {
        //return getCurrentPath();

         return "C:/Program Files (x86)/Nero Game Engine";
     }

     GameProject::Ptr ProjectManager::openProject(const std::string& project_name)
     {
         nero_log("openning project " + project_name);

         //close current project
         nlohmann::json project_workpsace;
         nlohmann::json project;

         auto workspaceTable = getWorkspaceTable();

        //workspace
        for(auto workspace : workspaceTable)
        {
			nero_log("initializing project 1");

            project = findProject(workspace["workspace_name"].get<std::string>(), project_name);
			nero_log("initializing project 2");

            if(!project.empty())
            {
                project_workpsace = workspace;
                break;
            }
        }
        //project

        if(!project_workpsace.empty() && !project.empty())
        {
            m_GameProject = GameProject::Ptr(new GameProject());

			nero_log("initializing project");
            m_GameProject->init(project, project_workpsace);
			nero_log("loading project");
			m_GameProject->loadProject();
			nero_log("loading project library");
			m_GameProject->loadProjectLibrary();
			nero_log("openning editor");
			m_GameProject->openEditor();
        }

        return m_GameProject;
     }

     nlohmann::json ProjectManager::findProject(const std::string& workspace_name, const std::string& project_name)
     {
         nlohmann::json result;

        auto projectTable = getWorkspaceProjectTable(workspace_name);


        for(auto project : projectTable)
        {
            if(project["project_name"] == project_name)
            {
                result =  project;
            }
        }

        return result;
     }

}