#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
// next three headers are for Windows AppData directory finding
#include <ShlObj.h>
#include <locale>
#include <codecvt>

#include "headers/structures.h" // for the program_config data structure
#include "headers/stonebase.h" // for checking if the table exists
#include "headers/parsing.h" // for checking if the file exists

int program_config::save(bool force)
{
    /*
    saves the settings in *this to the file config
    they are written in exactly the same order as the members appear in structures.h, with some exceptions:
        config_loaded is not written, the value of this member is specific to each program instance
        only the ROWIDs of the active items are written, the rest of the data is loaded from the database
    */

    // if config has not been loaded, do not overwrite it with empty data
    if((config_loaded == 0) && (!force)) return 0;

    std::ofstream writefile;
    writefile.open(config_path+"config", std::ios::trunc | std::ios::binary); // open file, discarding previous contents with ios::trunc

    // write active table information
    writefile << table << "\n";
    writefile << collection_table << "\n";
    writefile << vec_to_str(collection_table_list) << "\n";
    writefile << active_item_table << "\n";

    // start writing collection_results data
    writefile << collection_results.tablename << "\n";
    writefile << vector_int_to_string(collection_results.results) << "\n";
    writefile << collection_results.active_item << "\n";
    writefile << collection_results.sort_mask << "\n";
    writefile << collection_results.sort_ascending << "\n";
    writefile << collection_results.search_or << "\n";
    writefile << collection_results.datecomp << "\n";
    writefile << collection_results.costcomp << "\n";
    writefile << collection_results.numcomp << "\n";

    // start writing minerals_results data
    writefile << minerals_results.tablename << "\n";
    writefile << vector_int_to_string(minerals_results.results) << "\n";
    writefile << minerals_results.active_item << "\n";
    writefile << minerals_results.sort_mask << "\n";
    writefile << minerals_results.sort_ascending << "\n";
    writefile << minerals_results.search_or << "\n";
    // datecomp, numcomp and costcomp are ignored for minerals

    // write the ROWIDs for the active items
    writefile << active_collection_item.id << "\n";
    writefile << active_mineral_item.id << "\n";

    // write current image numbers
    writefile << collection_current_image << "\n";
    writefile << mineral_current_image << "\n";

    // write visible collection column list
    writefile << vec_to_str(visible_collection_columns) << "\n";
    // write widths of visible collection columns
    writefile << vec_to_str(collection_column_widths) << "\n";

    // write visible mineral column list
    writefile << vec_to_str(visible_mineral_columns) << "\n";
    // write widths of visible mineral columns
    writefile << vec_to_str(mineral_column_widths) << "\n";

    // write each search_result in the alternate_table_info vector
    for(search_result s:alternate_table_info)
    {
        writefile << s.tablename << "\n";
        writefile << vector_int_to_string(s.results) << "\n";
        writefile << s.active_item << "\n";
        writefile << s.sort_mask << "\n";
        writefile << s.sort_ascending << "\n";
        writefile << s.search_or << "\n";
        writefile << s.datecomp << "\n";
        writefile << s.costcomp << "\n";
        writefile << s.numcomp << "\n";
    }

    writefile.close(); // close file

    return 0;
}

int program_config::load()
{
    /*
    this is the constructor function for the class program_config
    this function reads user settings from the config file and applies these settings

    if no config file is found, this function sets default settings
    also checks if the last table active still exists, if not reset to Minerals
    */
    if(!does_file_exist(config_path+"config"))
    {
        // config file not found, set default values
        table = "Minerals";
        active_item_table = "Minerals";

        collection_results.sort_mask = 1; // sort by number
        collection_results.sort_ascending = true;
        collection_results.search_or = false; // use AND logic while searching

        minerals_results.sort_mask = 0; // sort by name
        minerals_results.sort_ascending = true;
        minerals_results.search_or = false; // use AND logic while searching

        visible_collection_columns = {"number", "name", "date", "type", "cost"};
        collection_column_widths   = {"5%",     "30%",  "15%",  "35%",  "15%" };
        visible_mineral_columns = {"name", "varieties"};
        mineral_column_widths   = {"35%",  "65%"      };

        std::vector<std::string> tables;
        get_tables(tables); // get a list of currently existing tables

        for(auto t:tables)
        {
            if(t != "Minerals") collection_table_list.push_back(t);
        }

        if (collection_table_list.size())
        {
            collection_table = collection_table_list[0];
        }

        save(true);
        load();
        return 0;

    }
    else // load config
    {
        std::vector<std::string> tables;
        std::string line;
        search_result sres_buffer;
        unsigned int linenum = 1;
        unsigned int altnum = 0;

        std::ifstream readfile;
        readfile.open(config_path+"config"); // open file

        // read each line of the config file
        while(getline(readfile, line))
        {
            if(linenum == 1) // table name
                table = line;
            else if(linenum == 2) // collection table name
                collection_table = line;
            else if(linenum == 3) // collection table list
                collection_table_list = explode(line, ',');
            else if(linenum == 4) // table of active item
                active_item_table = line;

            else if(linenum == 5) // -----------collection_results begins here-----------
                collection_results.tablename = line;
            else if(linenum == 6)
                collection_results.results = string_to_vector_int(line);
            else if(linenum == 7)
                collection_results.active_item = std::stoi(line);
            else if(linenum == 8)
                collection_results.sort_mask = std::stoi(line);
            else if(linenum == 9)
                collection_results.sort_ascending = to_bool(line);
            else if(linenum == 10)
                collection_results.search_or = to_bool(line);
            else if(linenum == 11)
                collection_results.datecomp = std::stoi(line);
            else if(linenum == 12)
                collection_results.costcomp = std::stoi(line);
            else if(linenum == 13)
                collection_results.numcomp = std::stoi(line);

            else if(linenum == 14) // -----------minerals_results begins here-----------
                minerals_results.tablename = line;
            else if(linenum == 15)
                minerals_results.results = string_to_vector_int(line);
            else if(linenum == 16)
                minerals_results.active_item = std::stoi(line);
            else if(linenum == 17)
                minerals_results.sort_mask = std::stoi(line);
            else if(linenum == 18)
                minerals_results.sort_ascending = to_bool(line);
            else if(linenum == 19)
                minerals_results.search_or = to_bool(line);

            else if(linenum == 20) // get ROWID of active_collection_item
                active_collection_item.id = std::stoi(line);
            else if(linenum == 21) // get ROWID of active_mineral_item
                active_mineral_item.id = std::stoi(line);

            else if(linenum == 22) // get number of collection image
                collection_current_image = std::stoi(line);
            else if(linenum == 23) // get number of mineral image
                mineral_current_image = std::stoi(line);

            else if(linenum == 24) // -----------visible collection column info-----------
                visible_collection_columns = explode(line, ',');
            else if(linenum == 25)
                collection_column_widths = explode(line, ',');

            else if(linenum == 26) // -----------visible mineral column info-----------
                visible_mineral_columns = explode(line, ',');
            else if(linenum == 27)
                mineral_column_widths = explode(line, ',');

            else if((linenum - 28) % 9 == 0) // -----------loading an arbitrary number of alternate table configurations-----------
            {
                sres_buffer.tablename = line;
            }
            else if((linenum - 29) % 9 == 0)
            {
                sres_buffer.results = string_to_vector_int(line);
            }
            else if((linenum - 30) % 9 == 0)
            {
                sres_buffer.active_item = std::stoi(line);
            }
            else if((linenum - 31) % 9 == 0)
            {
                sres_buffer.sort_mask = std::stoi(line);
            }
            else if((linenum - 32) % 9 == 0)
            {
                sres_buffer.sort_ascending = to_bool(line);
            }
            else if((linenum - 33) % 9 == 0)
            {
                sres_buffer.search_or = to_bool(line);
            }
            else if((linenum - 34) % 9 == 0)
            {
                sres_buffer.datecomp = std::stoi(line);
            }
            else if((linenum - 35) % 9 == 0)
            {
                sres_buffer.costcomp = std::stoi(line);
            }
            else if((linenum - 36) % 9 == 0)
            {
                sres_buffer.numcomp = std::stoi(line);
                alternate_table_info.push_back(sres_buffer); // add the buffer to the vector, as this is the last line of an object
                altnum++;
            }

            linenum++; // increment line number
        }

        readfile.close(); // close file
        get_tables(tables); // get a list of currently existing tables

        if ((collection_table == "") && (collection_table_list.size() > 0))
        {
            collection_table = collection_table_list[0];
        }

        // load data for the active items
        get_collection_columns(active_collection_item.id, {}, active_collection_item, collection_results.tablename);
        get_mineral_columns(active_mineral_item.id, {}, active_mineral_item);

        if(std::find(tables.begin(), tables.end(), table) == tables.end())
        {
            // if the table stored in config does not exist, default to Minerals
            change_table(*this, "Minerals");

            collection_results.sort_mask = 1; // sort by number
            collection_results.sort_ascending = true;
            collection_results.search_or = true; // use AND logic while searching

            collection_entry newcentr;
            search_result newsres;

            active_collection_item = newcentr; // reset the active collection item, as that table no longer exists
            collection_results = newsres; // reset results from this table as well
        }

        if(std::find(tables.begin(), tables.end(), collection_table) == tables.end())
        {
            // if the current collection table does not exist, change to a new table
            if (tables.size() > 1)
            {
                for (auto t : tables)
                {
                    if(t != "Minerals") change_table(*this, t);
                }
            }
            else
            {
                // if the collection_table stored in config does not exist, default to Minerals
                collection_table = "";

                collection_results.sort_mask = 1; // sort by number
                collection_results.sort_ascending = true;
                collection_results.search_or = true; // use AND logic while searching

                collection_entry newcentr;
                search_result newsres;

                active_collection_item = newcentr; // reset the active collection item, as that table no longer exists
                collection_results = newsres; // reset results from this table as well
            }
        }

        for (unsigned int i = 0; i < collection_table_list.size(); i++)
        {
            if (std::find(tables.begin(), tables.end(), collection_table_list[i]) == tables.end())
            {
                collection_table_list.erase(collection_table_list.begin() + i);
                i--;
            }
        }

        for(unsigned int i = 0; i < alternate_table_info.size(); i++)
        {
            if(std::find(tables.begin(), tables.end(), alternate_table_info[i].tablename) == tables.end())
            {
                alternate_table_info.erase(alternate_table_info.begin() + i);
                i--;
            }
        }

        for (auto col_tab : tables)
        {
            if (std::find(collection_table_list.begin(), collection_table_list.end(), col_tab) == collection_table_list.end())
            {
                if(col_tab == "Minerals") continue;
                collection_table_list.push_back(col_tab);
            }
        }
    }

    // set config_loaded to 1, now config can be safely saved without danger of overwrite
    config_loaded = 1;

    return 0;
}

int program_config::shutdown()
{
    if (!has_closed)
    {
        int save_value = this->save();
        close_database();
        backup_database(1, db_path);
        has_closed = true;
        return save_value;
    }
    return 0;
}

program_config::program_config()
{
    /*
    sets config_loaded to 0, this is set to 1 when config is loaded to prevent
    config being overwritten if the program is closed before config is loaded
    */
    config_loaded = 0;

    is_new = false;
    is_editing = false;
    has_closed = false;

    collection_current_image = -1;
    mineral_current_image = -1;

    if(!portable_condition())
    {
        wchar_t* path = 0;
        if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path) == S_OK)
        {
            std::wstring_convert<std::codecvt_utf8<wchar_t>,wchar_t> wsc;
            config_path = wsc.to_bytes(std::wstring(path));
            sanitise_path(config_path);
        }
        else
        {
            config_path = "C:/ProgramData";
        }
        CoTaskMemFree(static_cast<void*>(path));
        config_path += "/GeodeDB/";
        CreateDirectory(config_path.c_str(), NULL);
    }
    else
    {
        config_path = "";
    }
    CreateDirectory((config_path+"Images").c_str(), NULL);
    CreateDirectory((config_path+"Database").c_str(), NULL);
    CreateDirectory((config_path+"Config").c_str(), NULL);
    image_path = config_path + "Images/";
    db_path = config_path + "Database/";
    config_path = config_path + "Config/";
}

program_config::~program_config()
{
    /*
    this is the destructor function for the class program_config
    it is called when the class is 'destroyed', which is usually when the function it was declared in is exited

    since the class program_config should be used either globally in main.cpp or in main(), this destructor will therefore be
    called when the program ends.

    it therefore calls save() to make sure settings are saved when the program exits
    */
    save();
}

search_result::search_result()
{
    /*
    initialises ints and bools in search_result objects
    */

    active_item = -1;
    sort_mask = 0; // sort by name
    sort_ascending = true;
    search_or = false; // use AND logic while searching
    datecomp = 0; // equals
    costcomp = 0; // equals
    numcomp  = 0; // equals
}
