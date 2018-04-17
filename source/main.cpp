#include <fstream>
#include <vector>
#include <string>
#include <windows.h>

#include "headers/structures.h" // for the program_config data structure
#include "headers/stonebase.h" // for database open/close functions
#include "headers/parsing.h"  // for explode()
#include "headers/program.h"
#include "headers/dirent.h"

program_config *pconfig;

BOOL CtrlHandler(DWORD fdwCtrlType)
{
    // ensures that settings are saved when the program is manually closed

    switch (fdwCtrlType)
    {
    case CTRL_C_EVENT:
        pconfig->shutdown();
        break;
    case CTRL_CLOSE_EVENT:
        pconfig->shutdown();
        return 1;
        break;
    case CTRL_BREAK_EVENT:
        pconfig->shutdown();
        break;
    case CTRL_LOGOFF_EVENT:
        pconfig->shutdown();
        break;
    case CTRL_SHUTDOWN_EVENT:
        pconfig->shutdown();
        break;
    }
    return 0;
}

void on_exit()
{
    // call close_database on expected errors
    pconfig->shutdown();
}

int compile_html_source()
{
    std::vector<std::string> jsfiles;
    // order might be important, so these are hardcoded
    std::vector<std::string> cssfiles = { "main.css", "search-panel.css", "result-list.css", "itemview.css", "itemedit.css", "inputs.css", "fileadding.css" };
    std::string line;
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir("./ui/scripts")) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            std::string name = ent->d_name;
            size_t pos = name.find_last_of('.');

            if (pos == std::string::npos) continue;

            if (name.substr(pos) == ".js")
                jsfiles.push_back(name);
        }
        closedir(dir);
    }
    else
    {
        /* could not open directory */
        perror("");
        return EXIT_FAILURE;
    }
    if ((dir = opendir("./ui/style")) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            std::string name = ent->d_name;
            size_t pos = name.find_last_of('.');

            if (pos == std::string::npos) continue;

            if (name.substr(pos) == ".css")
            {
                if(std::find(cssfiles.begin(), cssfiles.end(), name) == cssfiles.end())
                    cssfiles.push_back(name);
            }
        }
        closedir(dir);
    }
    else
    {
        // could not open directory 
        perror("");
        return EXIT_FAILURE;
    }

    std::ifstream readhtmlfile("ui/ui_src.html");
    std::ifstream readfile;
    std::ofstream writefile("ui/ui.html", std::ios::trunc | std::ios::binary);

    while (getline(readhtmlfile, line))
    {
        if (line.find("<script>") != std::string::npos)
        {
            writefile << line << "\n";
            for (std::string f : jsfiles)
            {
                line = "ui/scripts/" + f;
                readfile.open(line);
                while (getline(readfile, line))
                {
                    writefile << "        " << line << "\n";
                }
                readfile.close();
            }
        }
        else if (line.find("<style>") != std::string::npos)
        {
            writefile << line << "\n";
            for (std::string f : cssfiles)
            {
                line = "ui/style/" + f;
                readfile.open(line);
                while (getline(readfile, line))
                {
                    writefile << "        " << line << "\n";
                }
                readfile.close();
            }
        }
        else
        {
            writefile << line << "\n";
        }
    }

    readhtmlfile.close();
    writefile.close();

    return 0;
}

int main()
{
    FreeConsole(); // hides the console window on boot
    // create the program's config object
    program_config config;
    // create the main program object
    ProgramInstance app;
    // make pointers to the config object
    app.config = &config;
    pconfig = &config;

    backup_database(0, config.db_path); // create Stones.db.boot
    open_database(config.db_path); // open database connection
    //compile_html_source();

    // configuration settings for the entire program
    // contains active table, search results and active items
    // start the main program loop
    app.Run();
 
    return config.shutdown();
}
