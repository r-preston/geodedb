#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <algorithm>

int compile_html_source()
{
    std::vector<std::string> jsfiles;
    // order might be important, so these are hardcoded. more can be added
    std::vector<std::string> cssfiles = { "main.css", "search-panel.css", "result-list.css", "itemview.css", "itemedit.css", "inputs.css", "fileadding.css" };
    std::string line;
    DIR *dir;
    struct dirent *ent;

    // find all files in the 'scripts' directory with the extension .js
    if ((dir = opendir("scripts")) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            std::string name = ent->d_name;
            size_t pos = name.find_last_of('.');

            if (pos == std::string::npos) continue;

            if (name.substr(pos) == ".js")
            {
                // only add file if it isn't already in the list
                if(std::find(jsfiles.begin(), jsfiles.end(), name) == jsfiles.end())
                    jsfiles.push_back(name);
            }
        }
        closedir(dir);
    }
    else
    {
        /* could not open directory */
        perror("");
        return EXIT_FAILURE;
    }
    std::cout << "Found " << jsfiles.size() << " JavaScript files:" << std::endl;
    for(std::string jsfile: jsfiles)
    {
        std::cout << "    " << jsfile << std::endl;
    }


    // find all files in the 'scripts' directory with the extension .css
    if ((dir = opendir("style")) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            std::string name = ent->d_name;
            size_t pos = name.find_last_of('.');

            if (pos == std::string::npos) continue;

            if (name.substr(pos) == ".css")
            {
                // only add file if it isn't already in the list
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
    std::cout << "Found " << cssfiles.size() << " CSS files:" << std::endl;
    for(std::string cssfile: cssfiles)
    {
        std::cout << "    " << cssfile << std::endl;
    }

    // ui_src.html is the base html file
    std::ifstream readhtmlfile("ui_src.html");
    std::ifstream readfile;
    // write all JS, CSS and HTML into the grand ui.html file
    // ui_src.html has empty <style> and <script> tags to indicate where the CSS and JS should be inserted
    std::ofstream writefile("ui.html", std::ios::trunc | std::ios::binary);

    while (getline(readhtmlfile, line))
    {
        if (line.find("<script>") != std::string::npos)
        {
            writefile << line << "\n";
            for (std::string f : jsfiles)
            {
                line = "scripts/" + f;
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
                line = "style/" + f;
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

    std::cout << "Compilation complete, all data written to ui.html" << std::endl;

    return 0;
}

int main()
{
    int err = compile_html_source();
    if(err)
    {
        std::cout << "One or more directories could not be opened - check that you are in the ui/ folder, that both the style/"
                     "and scripts/ folders exist, and that no files are open in another program and try again." << std::endl;
    }
    std::cout << "\nPress any key to exit";
    std::cin.get();
    return 0;
}
