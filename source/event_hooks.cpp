#include <iostream>
#include <windows.h>
#include <shlobj.h>
#include <Shobjidl.h>

#include "headers/program.h"
#include "headers/stonebase.h"
#include "headers/parsing.h"

// defines C++ event hooks for JS events.
// These can be triggered from JavaScript as methods of the global 'ui' object
void ProgramInstance::view_image(WebView* web_view, const JSArray& args)
{
    std::string error = "";
    bool is_collection = (args[0].ToString() == WSLit("collection-image"));

    TCHAR NPath[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, NPath);
    std::string abs_path = (std::string)NPath;
    sanitise_path(abs_path);
    abs_path += "/";

    std::string chosen_image = "";
    if (is_collection)
    {
        if(config->collection_current_image == -1) return;
        if(config->collection_current_image + 1 > (int)config->active_collection_item.images.size()) return;
        if(config->active_collection_item.images.size() == 0) return;
        chosen_image = config->active_collection_item.images[config->collection_current_image];
    }
    else
    {
        if(config->mineral_current_image == -1) return;
        if(config->mineral_current_image + 1 > (int)config->active_mineral_item.images.size()) return;
        if(config->active_mineral_item.images.size() == 0) return;
        chosen_image = config->active_mineral_item.images[config->mineral_current_image];
    }

    HINSTANCE hInstance = ShellExecute(0, 0, (abs_path + config->image_path + chosen_image).c_str(), 0, 0 , SW_SHOW );

    intptr_t err = (intptr_t)hInstance;

    if(err > 32)
        return;

    if(err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND || err == SE_ERR_FNF || err == SE_ERR_PNF)
    {
        error = "No such file '" + chosen_image + "'";
    }
    else if(err == SE_ERR_NOASSOC)
    {
        size_t p = chosen_image.find('.');
        error = "No file association for extension " + chosen_image.substr(p) + " - cannot open file";
    }
    else if(err == SE_ERR_ACCESSDENIED)
        error = "Access denied by OS";
    else
        error = "Operating system error";

    if (error != "")
    {
        JSArray errmsg;
        errmsg.Push(ToWebString(error));
        errmsg.Push(JSValue(false));
        window.InvokeAsync( WSLit("status"), errmsg);
    }
}

void ProgramInstance::remove_mineral_image(WebView* web_view, const JSArray& args)
{
    if (args.size())
    {
        if (args[0].IsInteger())
        {
            window.InvokeAsync(WSLit("remove_mineral_image"), args);
        }
        else
        {
            remove((config->image_path+namepart(jc.to_string(args[0]))).c_str());
        }
    }
}

void ProgramInstance::remove_collection_image(WebView* web_view, const JSArray& args)
{
    if (args.size())
    {
        if (args[0].IsInteger())
        {
            window.InvokeAsync(WSLit("remove_collection_image"), args);
        }
        else
        {
            remove((config->image_path+namepart(jc.to_string(args[0]))).c_str());
        }
    }
}

void ProgramInstance::choose_images(WebView* web_view, const JSArray& args)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog *pFileOpen = NULL;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        COMDLG_FILTERSPEC rgSpec[] =
        { 
            { L"Image files", L"*.jpg;*.jpeg;*.png;*.tif;*.tiff;*.bmp;*.gif" },
            { L"All files", L"*.*" }
        };
        pFileOpen->SetFileTypes(2, rgSpec);
        pFileOpen->SetFileTypeIndex(1);
        pFileOpen->SetTitle(L"Choose images");
        pFileOpen->SetOptions(FOS_STRICTFILETYPES | FOS_NOCHANGEDIR | FOS_ALLOWMULTISELECT | FOS_FILEMUSTEXIST | FOS_DONTADDTORECENT);

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                //IShellItem *pItem;
                //hr = pFileOpen->GetResult(&pItem);

                IShellItemArray *psiaResult;

                hr = pFileOpen->GetResults(&psiaResult);
                if (SUCCEEDED(hr))
                {
                    DWORD itemCount;
                    psiaResult->GetCount(&itemCount);

                    for (int i = 0; i < (int)itemCount; i++)
                    {
                        IShellItem *pItem;
                        PWSTR pszFilePath;
                        psiaResult->GetItemAt(i, &pItem);

                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                        JSArray ja;
                        std::wstring filepath = (LPWSTR)pszFilePath;
                        sanitise_path(filepath);
                        ja.Push(WideWebString(filepath));

                        if (args[0].ToString() == WSLit("c"))
                        {
                            window.Invoke(WSLit("add_collection_image_path"), ja);
                        }
                        else
                        {
                            window.Invoke(WSLit("add_mineral_image_path"), ja);
                        }
                    }
                    psiaResult->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }

    /*char filename[1024];

    OPENFILENAME ofn;
    ZeroMemory( &filename, sizeof( filename ) );
    ZeroMemory( &ofn,      sizeof( ofn ) );
    ofn.lStructSize  = sizeof( ofn );
    ofn.hwndOwner    = web_view->parent_window();  // If you have a window to center over, put its HANDLE here
    ofn.lpstrFilter  = "Image Files\0*.jpg;*.jpeg;*.png;*.tif;*.tiff;*.gif\0Any File\0*.*\0";
    ofn.lpstrFile    = filename;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile     = MAX_PATH;
    ofn.lpstrTitle   = "Select images";
    ofn.Flags        = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_NO­CHANGE­DIR;

    if (GetOpenFileNameA( &ofn )) // if the user selects a file
    {
        // below is easily the most elegant code in this entire program
        const char * p = filename;
        std::vector<std::string> splitstr;
        do {
            splitstr.push_back(std::string(p));
            p += splitstr.back().size() + 1;
        } while (( p - filename < 1024 ) && (std::string(p) != ""));

        if(splitstr.size())
            sanitise_path(splitstr[0]);

        for (unsigned int i = 1; i < splitstr.size(); i++)
        {
            splitstr[i] = splitstr[0] + "/" + splitstr[i];
        }

        if(splitstr.size() > 1)
            splitstr.erase(splitstr.begin());
    }*/
}

void ProgramInstance::add_from_file(WebView* web_view, const JSArray& args)
{
    int duplicate_method = 0;
    if (args.size())
    {
        if (args[0].IsInteger())
        {
            duplicate_method = args[0].ToInteger();
        }
    }

    if (duplicate_method == 0)
    {
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (SUCCEEDED(hr))
        {
            IFileOpenDialog *pFileOpen = NULL;

            // Create the FileOpenDialog object.
            hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

            COMDLG_FILTERSPEC rgSpec[] =
            { 
                { L"CSV files", L"*.csv" },
                { L"All files", L"*.*" }
            };
            pFileOpen->SetFileTypes(2, rgSpec);
            pFileOpen->SetFileTypeIndex(1);
            pFileOpen->SetTitle(L"Choose CSV file");
            pFileOpen->SetOptions(FOS_STRICTFILETYPES | FOS_NOCHANGEDIR | FOS_FILEMUSTEXIST | FOS_DONTADDTORECENT);

            if (SUCCEEDED(hr))
            {
                // Show the Open dialog box.
                hr = pFileOpen->Show(NULL);

                // Get the file name from the dialog box.
                if (SUCCEEDED(hr))
                {
                    IShellItem *pItem;
                    hr = pFileOpen->GetResult(&pItem);

                    if (SUCCEEDED(hr))
                    {
                        PWSTR pszFilePath;

                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                        std::wstring file = std::wstring((LPWSTR)pszFilePath);

                        std::vector<std::string> mistakes;

                        mistakes = parse_input_file(file, config->table); // check the file given for mistakes

                        if(mistakes.size() == 0) // no mistakes made
                        {
                            std::ifstream src(file.c_str(), std::ios::binary);
                            std::ofstream dst( (config->config_path+"bulk_add_tmp"), std::ios::binary);
                            dst << src.rdbuf();

                            JSArray ja;
                            ja.Push(WideWebString(file));
                            window.Invoke(WSLit("show_continue_box"), ja);
                        }
                        else
                        {
                            // show error box
                            JSArray ja, ja_inner;
                            for (unsigned int i = 0; i < mistakes.size(); i++)
                            {
                                ja_inner.Push(ToWebString(mistakes[i]));
                            }
                            ja.Push(ja_inner);
                            ja.Push(WideWebString(file));
                            window.Invoke(WSLit("show_error_box"), ja);
                        }

                        pItem->Release();
                    }
                }
                pFileOpen->Release();
            }
            CoUninitialize();
        }
    }
    else
    {
        JSValue result = window.Invoke(WSLit("get_add_radio_value"), args);
        duplicate_method = result.ToInteger();

        std::vector<std::string> messages = parse_input_file((config->config_path+"bulk_add_tmp"), config->table, duplicate_method);

        window.Invoke(WSLit("close_addfile_popups"), args);
        JSArray stat_args;

        int good_count = 0;
        int bad_count = 0;
        for (auto message : messages)
        {
            if(message == "fail") bad_count++;
            else good_count++;
        }
        std::string s = std::to_string(good_count)+" items added successfully, "+std::to_string(bad_count)+" failed item additions";

        stat_args.Push(JSValue(ToWebString(s)));
        window.Invoke(WSLit("status"), stat_args);
    }
}

void ProgramInstance::close_addfile_popups(WebView* web_view, const JSArray& args)
{
    window.InvokeAsync(WSLit("close_addfile_popups"), args);
}

void ProgramInstance::set_mask(WebView* web_view, const JSArray& args)
{
    std::string identifier = ToString(args[0].ToString());

    JSValue result;

    if (identifier[0] == 'm')
    {
        result = web_view->ExecuteJavascriptWithResult(WSLit("program_config.minerals_results"), WSLit(""));
    }
    else
    {
        result = web_view->ExecuteJavascriptWithResult(WSLit("program_config.collection_results"), WSLit(""));
    }
    JSObject table_info_struct = result.ToObject();
    
    if (identifier.substr(1) == "-and")
    {
        config->collection_results.search_or = false;
        table_info_struct.SetProperty( WSLit("search_or"), JSValue(false) );
        return;
    }
    else if (identifier.substr(1) == "-or")
    {
        config->collection_results.search_or = true;
        table_info_struct.SetProperty( WSLit("search_or"), JSValue(true) );
        return;
    }

    int value = args[1].ToInteger();

    if (identifier == "date")
    {
        config->collection_results.datecomp = value;
        table_info_struct.SetProperty( WSLit("datecomp"), JSValue(value) );
    }
    else if (identifier == "cost")
    {
        config->collection_results.costcomp = value;
        table_info_struct.SetProperty( WSLit("costcomp"), JSValue(value) );
    }
    else if (identifier == "num")
    {
        config->collection_results.numcomp = value;
        table_info_struct.SetProperty( WSLit("numcomp"), JSValue(value) );
    }
}

void ProgramInstance::sync_item(WebView* web_view, const JSArray& args)
{
    if (ToString(args[0].ToString()) == "visible_collection_columns")
    {
        config->visible_collection_columns = jc.to_vectorstring(args[1]);
        //js_config.SetProperty( WSLit("visible_collection_columns"), jc.to_js( config->visible_collection_columns ) );
    }
    else if (ToString(args[0].ToString()) == "collection_column_widths")
    {
        config->collection_column_widths = jc.to_vectorstring(args[1]);
    }
    else if (ToString(args[0].ToString()) == "visible_mineral_columns")
    {
        config->visible_mineral_columns = jc.to_vectorstring(args[1]);
    }
    else if (ToString(args[0].ToString()) == "mineral_column_widths")
    {
        config->mineral_column_widths = jc.to_vectorstring(args[1]);
    }
    else if (ToString(args[0].ToString()) == "mineral_current_image")
    {
        config->mineral_current_image = jc.to_int(args[1]);
    }
    else if (ToString(args[0].ToString()) == "collection_current_image")
    {
        config->collection_current_image = jc.to_int(args[1]);
    }
}

void ProgramInstance::is_new(WebView* web_view, const JSArray& args)
{
    config->is_editing = args[0].ToBoolean();
    config->is_new = args[0].ToBoolean();
}

void ProgramInstance::confirm(WebView* web_view, const JSArray& args)
{
    JSArray ja;
    ja.Push(args[0]); // message to display
    ja.Push(WSLit("confirm_callback")); // C++ callback
    ja.Push(args[1]); // JS function to call if user confirms
    ja.Push(args[2]); // any arguments to pass to JS callback
    window.Invoke(WSLit("show_confirm_box"), ja);
}

void ProgramInstance::confirm_callback(WebView* web_view, const JSArray& args)
{
    JSArray ja;
    window.Invoke(WSLit("hide_message_boxes"), ja);
    if (args[0].ToBoolean())
    {
        window.Invoke(args[1].ToString(), args[2].ToArray());
    }
}

JSValue ProgramInstance::add_mineral_items(WebView* web_view, const JSArray& args)
{
    //JSConvert jc;
    window.Invoke(WSLit("remove_mineral_results"), args);
    std::vector<int> rowids = config->minerals_results.results;

    for (unsigned int i = 0; i < rowids.size(); i++)
    {
        JSArray create_args;

        //JSArray item_data;
        char seperator = '\x001e';
        std::string data;
        mineral_entry db_data;

        const char * errmsg = get_mineral_columns(rowids[i], {}, db_data);
        if (errmsg)
        {
            std::cout << errmsg << std::endl;
            continue;
        }

        data += byte_to_string(db_data.id) + seperator;
        data += db_data.name + seperator;
        data += vec_to_str(db_data.varieties) + seperator;
        data += prune(db_data.description) + seperator;
        data += vec_to_str(db_data.colours) + seperator;
        data += db_data.structure + seperator;
        data += db_data.composition + seperator;
        data += vec_to_sepstring(db_data.images) + seperator;
        data += db_data.category + seperator;
        data += byte_to_string(db_data.pagenumber);

        /*item_data.Push( jc.to_js(db_data.id));
        item_data.Push( jc.to_js(db_data.name));
        item_data.Push( jc.to_js(db_data.varieties));
        item_data.Push( jc.to_js(prune(db_data.description)));
        item_data.Push( jc.to_js(db_data.colours));
        item_data.Push( jc.to_js(db_data.structure));
        item_data.Push( jc.to_js(db_data.composition));
        item_data.Push( jc.to_js(db_data.images));
        item_data.Push( jc.to_js(db_data.category));
        item_data.Push( jc.to_js(db_data.pagenumber));*/

        //create_args.Push(item_data);
        create_args.Push(ToWebString(data));
        create_args.Push(JSValue((int)i));

        window.InvokeAsync(WSLit("add_mineral_result"), create_args);
    }
    return JSValue(0);
}

JSValue ProgramInstance::add_collection_items(WebView* web_view, const JSArray& args)
{
    //JSConvert jc;

    window.Invoke(WSLit("remove_collection_results"), args);

    std::vector<int> rowids = config->collection_results.results;

    for (unsigned int i = 0; i < rowids.size(); i++)
    {
        JSArray create_args;

        //JSArray item_data;
        char seperator = '\x001e';
        std::string data;

        collection_entry db_data;

        const char * errmsg = get_collection_columns(rowids[i], {}, db_data, config->collection_table);
        if (errmsg)
        {
            std::cout << errmsg << std::endl;
            continue;
        }

        data += byte_to_string(db_data.id) + seperator;
        data += byte_to_string(db_data.purchase) + seperator;
        data += db_data.order + seperator;
        data += db_data.name + seperator;
        data += vec_to_str(db_data.mineral) + seperator;
        data += prune(db_data.description) + seperator;
        data += vec_to_str(db_data.colours) + seperator;
        data += db_data.date + seperator;
        data += db_data.buy_location + seperator;
        data += prune(db_data.notes) + seperator;
        data += byte_to_string(db_data.cost) + seperator;
        data += db_data.buyer + seperator;
        data += db_data.now_location + seperator;
        data += vec_to_sepstring(db_data.images) + seperator;
        data += byte_to_string(db_data.texture);

        /*item_data.Push( jc.to_js(db_data.id));
        item_data.Push( jc.to_js(db_data.purchase));
        item_data.Push( jc.to_js(db_data.order));
        item_data.Push( jc.to_js(db_data.name));
        item_data.Push( jc.to_js(db_data.mineral));
        item_data.Push( jc.to_js(prune(db_data.description)));
        item_data.Push( jc.to_js(db_data.colours));
        item_data.Push( jc.to_js(db_data.date));
        item_data.Push( jc.to_js(db_data.buy_location));
        item_data.Push( jc.to_js(prune(db_data.notes)));
        item_data.Push( jc.to_js(db_data.cost));
        item_data.Push( jc.to_js(db_data.buyer));
        item_data.Push( jc.to_js(db_data.now_location));
        item_data.Push( jc.to_js(db_data.images));
        item_data.Push( jc.to_js(db_data.texture));*/

        //create_args.Push(item_data);
        create_args.Push(ToWebString(data));
        create_args.Push(JSValue((int)i));

        window.InvokeAsync(WSLit("add_collection_result"), create_args);
    }
    return JSValue(0);
}

void ProgramInstance::toggle_mineral_column(WebView* web_view, const JSArray& args)
{
    window.Invoke(WSLit("toggle_mineral_column"), args);
}

void ProgramInstance::toggle_collection_column(WebView* web_view, const JSArray& args)
{
    window.Invoke(WSLit("toggle_collection_column"), args);
}

void ProgramInstance::activate_collection_table(WebView* web_view, const JSArray& args)
{
    if(args.size() > 0)
    {
        if (args[0].ToBoolean() == false)
        {
            window.Invoke(WSLit("hide_message_boxes"), args);
            return;
        }
        JSArray new_ja;
        new_ja.Push(JSValue(false));
        window.Invoke(WSLit("cancel_item_edit"), new_ja);
        window.Invoke(WSLit("hide_message_boxes"), args);
    }
    else if (config->is_editing)
    {
        JSArray ja, jar;
        ja.Push(WSLit("Unsaved changes to this item will be lost if you change tables. Do you wish to continue?")); // message to display
        ja.Push(WSLit("activate_collection_table")); // C++ callback
        ja.Push(WSLit("cancel_item_edit_2")); // JS function to call if user confirms
        ja.Push(jar); // any arguments to pass to JS callback
        window.Invoke(WSLit("show_confirm_box"), ja);
        return;
    }

    JSArray ja_two;
    JSValue result = window.Invoke(WSLit("activate_collection_table"), ja_two);
    config->table = config->collection_table;
    config->active_item_table = config->table;
}

void ProgramInstance::dropdown(WebView* web_view, const JSArray& args)
{
    window.Invoke(WSLit("dropdown"), args);
}

void ProgramInstance::choose_new_table(WebView* web_view, const JSArray& args)
{
    if(args.size() > 1)
    {
        if (args[1].ToBoolean() == false)
        {
            window.Invoke(WSLit("hide_message_boxes"), args);
            return;
        }
        JSArray new_ja;
        new_ja.Push(JSValue(false));
        window.Invoke(WSLit("cancel_item_edit"), new_ja);
        window.Invoke(WSLit("hide_message_boxes"), args);
    }
    else if (config->is_editing)
    {
        JSArray ja, jar;
        ja.Push(WSLit("Unsaved changes to this item will be lost if you change tables. Do you wish to continue?")); // message to display
        ja.Push(WSLit("choose_new_table")); // C++ callback
        ja.Push(WSLit("cancel_item_edit_2")); // JS function to call if user confirms
        ja.Push(jar); // any arguments to pass to JS callback
        ja.Push(args[0]); // argument to pass to C++ callback
        window.Invoke(WSLit("show_confirm_box"), ja);
        return;
    }


    std::string new_table = ToString(args[0].ToString());
    JSArray empty;

    change_table(*config, new_table);
    if (config->table == "Minerals")
    {
        add_mineral_items(web_view, empty);
    }
    else
    {
        add_collection_items(web_view, empty);
        SyncConfigObjects("config.active_collection_item");
        SyncConfigObjects("config.collection_results");
    }

    SyncConfigObjects("config");

    JSArray j_args;
    window.Invoke(WSLit("choose_new_table"), j_args);
}

void ProgramInstance::remove_table(WebView* web_view, const JSArray& args)
{
    window.Invoke(WSLit("confirm_delete_table"), args);
}

void ProgramInstance::continue_delete_table(WebView* web_view, const JSArray& args)
{
    WebString previous_table = ToWebString(config->table);
    const char * result = drop_table(*config, ToString(args[0].ToString()));
    if(result)
    {
        std::cout << result << std::endl;
        JSArray stat_args;
        std::string s = "Table deletion failed with error message \""+std::string(result)+"\"";
        stat_args.Push(JSValue(ToWebString(s)));
        stat_args.Push(JSValue(true));
        window.Invoke(WSLit("status"), stat_args);
        return;
    }

    SyncConfigObjects();

    JSArray ja;
    ja.Push(args[0]);
    ja.Push(previous_table);
    window.Invoke(WSLit("remove_table"), ja);
}

void ProgramInstance::get_table_name(WebView* web_view, const JSArray& args)
{
    if(args.size() > 0)
    {
        if (args[0].ToBoolean() == false)
        {
            window.Invoke(WSLit("hide_message_boxes"), args);
            return;
        }
        JSArray new_ja;
        new_ja.Push(JSValue(false));
        window.Invoke(WSLit("cancel_item_edit"), new_ja);
        window.Invoke(WSLit("hide_message_boxes"), args);
    }
    else if (config->is_editing)
    {
        JSArray ja, jar;
        ja.Push(WSLit("Unsaved changes to this item will be lost if you change tables. Do you wish to continue?")); // message to display
        ja.Push(WSLit("get_table_name")); // C++ callback
        ja.Push(WSLit("cancel_item_edit_2")); // JS function to call if user confirms
        ja.Push(jar); // any arguments to pass to JS callback
        window.Invoke(WSLit("show_confirm_box"), ja);
        return;
    }

    JSArray popup;
    popup.Push(WSLit("Enter name of new table (non-ascii or unprintable characters are automatically removed)"));
    popup.Push(WSLit("check_table_prompt"));
    JSValue table_name = window.Invoke(WSLit("show_prompt_box"), popup);
}

void ProgramInstance::create_new_table(WebView* web_view, const JSArray& args)
{
    JSValue table_name = args[0];
    JSArray args_two;

    std::string table = ToString(table_name.ToString());

    while (table.find("<div>") != std::string::npos)
    {
        table.replace(table.find("<div>"), 5, "");
    }
    while (table.find("</div>") != std::string::npos)
    {
        table.replace(table.find("</div>"), 6, "");
    }

    const char * errmsg = make_table(table);
    if (errmsg)
    {
        std::cout << errmsg << std::endl;
        JSArray stat_args;
        std::string s = "Table creation failed with error message \""+std::string(errmsg)+"\"";
        stat_args.Push(JSValue(ToWebString(s)));
        stat_args.Push(JSValue(true));
        window.Invoke(WSLit("status"), stat_args);
        return;
    }
    config->collection_table_list.push_back(table);
    change_table(*config, table);

    SyncConfigObjects();

    args_two.Push(ToWebString(table));
    window.Invoke(WSLit("create_new_table"), args_two);
}

void ProgramInstance::check_table_prompt(WebView* web_view, const JSArray& args)
{
    if(args[0].IsBoolean())
    {
        // user clicked the cancel button
        JSArray ja;
        window.Invoke(WSLit("hide_message_boxes"), ja);
        return;
    }

    std::string name = ToString(args[0].ToString());
    censor(name);

    std::vector<std::string> cl = config->collection_table_list;

    if( ( std::find(cl.begin(), cl.end(), name) != cl.end() ) || ( name == "" ) || (name == "Minerals")  )
    {
        if(name == "")
        {
            JSValue newmsg = WSLit("Please enter a valid table name (alphabetic characters only)");
            JSArray ja;
            ja.Push(newmsg);
            window.Invoke(WSLit("bad_table_name"), ja);
            return;
        }
        else
        {
            JSValue newmsg = ToWebString("A table with name '"+name+"' already exists - enter another name");
            JSArray ja;
            ja.Push(newmsg);
            window.Invoke(WSLit("bad_table_name"), ja);
            return;
        }
    }
    // invoke creation with name
    JSArray ja;
    window.Invoke(WSLit("hide_message_boxes"), ja);

    ja.Push(ToWebString(name));
    create_new_table(view_->web_view(), ja);
}

void ProgramInstance::activate_mineral_table(WebView* web_view, const JSArray& args)
{
    if(args.size() > 0)
    {
        if (args[0].ToBoolean() == false)
        {
            window.Invoke(WSLit("hide_message_boxes"), args);
            return;
        }
        JSArray new_ja;
        new_ja.Push(JSValue(false));
        window.Invoke(WSLit("cancel_item_edit"), new_ja);
        window.Invoke(WSLit("hide_message_boxes"), args);
    }
    else if (config->is_editing)
    {
        JSArray ja, jar;
        ja.Push(WSLit("Unsaved changes to this item will be lost if you change tables. Do you wish to continue?")); // message to display
        ja.Push(WSLit("activate_mineral_table")); // C++ callback
        ja.Push(WSLit("cancel_item_edit_2")); // JS function to call if user confirms
        ja.Push(jar); // any arguments to pass to JS callback
        window.Invoke(WSLit("show_confirm_box"), ja);
        return;
    }


    window.Invoke(WSLit("activate_mineral_table"), args);
    change_table(*config, "Minerals");
}

void ProgramInstance::help_popup(WebView* web_view, const JSArray& args)
{
    window.Invoke(WSLit("help_popup"), args);
}

void ProgramInstance::clear_collection_form(WebView* web_view, const JSArray& args)
{
    window.Invoke(WSLit("clear_collection_form"), args);
}

void ProgramInstance::search_collection_db(WebView* web_view, const JSArray& args)
{
    JSValue sres = window.Invoke(WSLit("search_collection_db"), args);
    if(sres.IsBoolean()) return;
    JSArray res = sres.ToArray();
    JSObject search_terms = res[0].ToObject();
    std::string main_search = ToString(res[1].ToString());
    collection_entry entry;
    int argnum = 0;

    entry.purchase     = jc.to_int( search_terms.GetProperty( WSLit("number") ) );
    entry.cost         = jc.to_int( search_terms.GetProperty( WSLit("cost") ) );
    entry.name         = jc.to_string( search_terms.GetProperty( WSLit("name") ) );
    entry.description  = jc.to_string( search_terms.GetProperty( WSLit("description") ) );
    entry.date         = jc.to_string( search_terms.GetProperty( WSLit("date") ) );
    entry.buyer        = jc.to_string( search_terms.GetProperty( WSLit("buyer") ) );
    entry.notes        = jc.to_string( search_terms.GetProperty( WSLit("notes") ) );
    entry.buy_location = jc.to_string( search_terms.GetProperty( WSLit("purchaseplace") ) );
    entry.now_location = jc.to_string( search_terms.GetProperty( WSLit("location") ) );
    entry.mineral      = jc.to_vectorstring( search_terms.GetProperty( WSLit("type") ) );
    entry.colours      = jc.to_vectorstring( search_terms.GetProperty( WSLit("colours") ) );

    if (entry.purchase != -1) argnum++;
    if (entry.cost != -1) argnum++;
    if (entry.name != "") argnum++;
    if (entry.description != "") argnum++;
    if (entry.buyer != "") argnum++;
    if (entry.notes != "") argnum++;
    if (entry.buy_location != "") argnum++;
    if (entry.now_location != "") argnum++;
    if (entry.mineral.size() > 0) argnum++;
    if (entry.colours.size() > 0) argnum++;

    search_result * c = &(config->collection_results);

    std::vector<int> comps;
    comps.push_back(c->numcomp);
    comps.push_back(c->datecomp);
    comps.push_back(c->costcomp);

    search_collection(entry, main_search, argnum, config->table, *c, comps, c->search_or);

    JSValue result = web_view->ExecuteJavascriptWithResult(WSLit("program_config.collection_results"), WSLit(""));
    JSObject collection_results = result.ToObject();
    collection_results.SetProperty( WSLit("results"), jc.to_js( config->collection_results.results ) );

    JSArray stat_args;
    std::string s = "Search complete, "+std::to_string(c->results.size())+" result"+plural_correction(c->results.size())+" matched";
    stat_args.Push(JSValue(ToWebString(s)));
    window.Invoke(WSLit("status"), stat_args);

    add_collection_items(web_view, args);
}

void ProgramInstance::new_item(WebView* web_view, const JSArray& args)
{
    config->is_editing = true;
    window.Invoke(WSLit("new_item"), args);
}

void ProgramInstance::clear_mineral_form(WebView* web_view, const JSArray& args)
{
    window.Invoke(WSLit("clear_mineral_form"), args);
}

void ProgramInstance::search_mineral_db(WebView* web_view, const JSArray& args)
{
    JSArray res = window.Invoke(WSLit("search_mineral_db"), args).ToArray();
    JSObject search_terms = res[0].ToObject();
    std::string main_search = ToString(res[1].ToString());
    mineral_entry entry;
    int argnum = 0;

    entry.name        = jc.to_string( search_terms.GetProperty( WSLit("name") ) );
    entry.description = jc.to_string( search_terms.GetProperty( WSLit("description") ) );
    entry.structure   = jc.to_string( search_terms.GetProperty( WSLit("structure") ) );
    entry.composition = jc.to_string( search_terms.GetProperty( WSLit("composition") ) );
    entry.category    = jc.to_string( search_terms.GetProperty( WSLit("composition") ) );
    entry.varieties   = jc.to_vectorstring( search_terms.GetProperty( WSLit("varieties") ) );
    entry.colours     = jc.to_vectorstring( search_terms.GetProperty( WSLit("colours") ) );

    if (entry.name != "") argnum++;
    if (entry.description != "") argnum++;
    if (entry.structure != "") argnum++;
    if (entry.category != "") argnum++;
    if (entry.composition != "") argnum++;
    if (entry.varieties.size() > 0) argnum++;
    if (entry.colours.size() > 0) argnum++;

    search_result * m = &(config->minerals_results);

    search_minerals(entry, main_search, argnum, *m, m->search_or);

    JSValue result = web_view->ExecuteJavascriptWithResult(WSLit("program_config.minerals_results"), WSLit(""));
    JSObject minerals_results = result.ToObject();
    minerals_results.SetProperty( WSLit("results"), jc.to_js( m->results ) );

    JSArray stat_args;

    std::string s = "Search complete, "+std::to_string(m->results.size())+" result"+plural_correction(m->results.size())+" matched";
    stat_args.Push(JSValue(ToWebString(s)));
    window.Invoke(WSLit("status"), stat_args);
    add_mineral_items(web_view, args);
}

void ProgramInstance::change_collection_sorting_mask(WebView* web_view, const JSArray& args)
{
    int mask = args[0].ToInteger();
    search_result * cres = &(config->collection_results);

    if(mask == cres->sort_mask)
    {
        cres->sort_ascending = !(cres->sort_ascending);
    }
    else
    {
        cres->sort_ascending = true;
        cres->sort_mask = mask;
    }

    sort_items(cres->results, config->table, cres->sort_ascending, cres->sort_mask);

    JSValue result;

    result = web_view->ExecuteJavascriptWithResult(WSLit("program_config.collection_results"), WSLit(""));
    JSObject collection_results = result.ToObject();

    collection_results.SetProperty( WSLit("results"), jc.to_js( config->collection_results.results ) );

    JSArray jar;
    window.Invoke(WSLit("sort_collection_items"), jar);
    window.InvokeAsync(WSLit("change_collection_sorting_mask"), args);
}

void ProgramInstance::change_mineral_sorting_mask(WebView* web_view, const JSArray& args)
{
    config->minerals_results.sort_ascending = !(config->minerals_results.sort_ascending);

    sort_items(config->minerals_results.results, "Minerals", config->minerals_results.sort_ascending, config->minerals_results.sort_mask);

    JSValue result;

    result = web_view->ExecuteJavascriptWithResult(WSLit("program_config.minerals_results"), WSLit(""));
    JSObject minerals_results = result.ToObject();

    minerals_results.SetProperty( WSLit("results"), jc.to_js( config->minerals_results.results ) );

    JSArray jar;
    window.Invoke(WSLit("sort_mineral_items"), jar);
    window.InvokeAsync(WSLit("change_mineral_sorting_mask"), args);
}

void ProgramInstance::decrement_image(WebView* web_view, const JSArray& args)
{
    JSValue image_number = window.Invoke(WSLit("decrement_image"), args);
    if (ToString(args[0].ToString()) == "collection")
    {
        config->collection_current_image = image_number.ToInteger();
    }
    else
    {
        config->mineral_current_image = image_number.ToInteger();
    }
}

void ProgramInstance::advance_image(WebView* web_view, const JSArray& args)
{
    JSValue image_number = window.Invoke(WSLit("advance_image"), args);
    if (ToString(args[0].ToString()) == "collection")
    {
        config->collection_current_image = image_number.ToInteger();
    }
    else
    {
        config->mineral_current_image = image_number.ToInteger();
    }
}

void ProgramInstance::confirm_delete_item(WebView* web_view, const JSArray& args)
{
    // args[0]: either "collection" or "mineral"
    if ((config->active_collection_item.id != -1) && (args[0].ToString() == WSLit("collection")))
    {
        window.Invoke(WSLit("confirm_delete_item"), args);
    }
    else if ((config->active_mineral_item.id != -1) && (args[0].ToString() == WSLit("mineral")))
    {
        window.Invoke(WSLit("confirm_delete_item"), args);
    }
}

JSValue ProgramInstance::delete_collection_item(WebView* web_view, const JSArray& args)
{
    const char * errmsg = delete_row(config->table, config->active_collection_item.id);
    if (errmsg)
    {
        return JSValue(1);
    }

    std::vector<int> & r = config->collection_results.results;
    r.erase(std::find(r.begin(), r.end(), config->active_collection_item.id));

    config->collection_results.active_item = -1;
    config->collection_current_image = -1;
    collection_entry cres;
    config->active_collection_item = cres;

    SyncConfigObjects("config");
    SyncConfigObjects("config.collection_results");
    return JSValue(0);
}

void ProgramInstance::edit_collection_item(WebView* web_view, const JSArray& args)
{
    if (config->active_collection_item.id != -1)
    {
        config->is_editing = true;
        window.Invoke(WSLit("edit_collection_item"), args);
    }
}

JSValue ProgramInstance::delete_mineral_item(WebView* web_view, const JSArray& args)
{
    const char * errmsg = delete_row("Minerals", config->active_mineral_item.id);
    if (errmsg)
    {
        return JSValue(1);
    }

    std::vector<int> & r = config->minerals_results.results;
    r.erase(std::find(r.begin(), r.end(), config->active_mineral_item.id));

    config->minerals_results.active_item = -1;
    config->mineral_current_image = -1;
    mineral_entry mres;
    config->active_mineral_item = mres;

    SyncConfigObjects("config");
    SyncConfigObjects("config.minerals_results");
    return JSValue(0);
}

void ProgramInstance::edit_mineral_item(WebView* web_view, const JSArray& args)
{
    if (config->active_mineral_item.id != -1)
    {
        config->is_editing = true;
        window.Invoke(WSLit("edit_mineral_item"), args);
    }
}

void ProgramInstance::cancel_item_edit(WebView* web_view, const JSArray& args)
{
    window.Invoke(WSLit("cancel_item_edit"), args);
}

void ProgramInstance::save_collection_item(WebView* web_view, const JSArray& args)
{
    JSObject data;
    JSValue jdata = window.Invoke(WSLit("save_collection_item"), args);
    if (jdata.IsObject())
    {
        config->is_editing = false;
        data = jdata.ToObject();
    }
    else return;

    collection_entry entry;

    entry.purchase      = jc.to_int( data.GetProperty( WSLit("number") ) );
    entry.cost          = jc.to_int( data.GetProperty( WSLit("cost") ) );
    entry.texture       = jc.to_int( data.GetProperty( WSLit("texture") ) );
    entry.order         = jc.to_string( data.GetProperty( WSLit("order") ) );
    entry.name          = jc.to_string( data.GetProperty( WSLit("name") ) );
    entry.description   = jc.to_string( data.GetProperty( WSLit("description") ) );
    entry.date          = jc.to_string( data.GetProperty( WSLit("date") ) );
    entry.buyer         = jc.to_string( data.GetProperty( WSLit("buyer") ) );
    entry.notes         = jc.to_string( data.GetProperty( WSLit("notes") ) );
    entry.buy_location  = jc.to_string( data.GetProperty( WSLit("purchaseplace") ) );
    entry.now_location  = jc.to_string( data.GetProperty( WSLit("location") ) );
    entry.mineral       = jc.to_vectorstring( data.GetProperty( WSLit("type") ) );
    entry.colours       = jc.to_vectorstring( data.GetProperty( WSLit("colours") ) );
    entry.source_images = jc.to_vectorwstring( data.GetProperty( WSLit("images") ) );

    size_t pos;
    for (unsigned int i = 0; i < entry.colours.size(); i++)
    {
        pos = entry.colours[i].find('\n');
        while (pos != std::string::npos)
        {
            entry.colours[i].erase(pos, 1);
            pos = entry.colours[i].find('\n');
        }
    }
    for (unsigned int i = 0; i < entry.source_images.size(); i++)
    {
        size_t pos = entry.source_images[i].find('\n');
        while (pos != std::string::npos)
        {
            entry.source_images[i].erase(pos, 1);
            pos = entry.source_images[i].find('\n');
        }
    }
    for (unsigned int i = 0; i < entry.mineral.size(); i++)
    {
        size_t pos = entry.mineral[i].find('\n');
        while (pos != std::string::npos)
        {
            entry.mineral[i].erase(pos, 1);
            pos = entry.mineral[i].find('\n');
        }
    }
    pos = entry.order.find('\n');
    while (pos != std::string::npos)
    {
        entry.order.erase(pos, 1);
        pos = entry.order.find('\n');
    }
    pos = entry.name.find('\n');
    while (pos != std::string::npos)
    {
        entry.name.erase(pos, 1);
        pos = entry.name.find('\n');
    }
    pos = entry.date.find('\n');
    while (pos != std::string::npos)
    {
        entry.date.erase(pos, 1);
        pos = entry.date.find('\n');
    }
    pos = entry.buy_location.find('\n');
    while (pos != std::string::npos)
    {
        entry.buy_location.erase(pos, 1);
        pos = entry.buy_location.find('\n');
    }
    pos = entry.now_location.find('\n');
    while (pos != std::string::npos)
    {
        entry.now_location.erase(pos, 1);
        pos = entry.now_location.find('\n');
    }
    pos = entry.buyer.find('\n');
    while (pos != std::string::npos)
    {
        entry.buyer.erase(pos, 1);
        pos = entry.buyer.find('\n');
    }

    if (config->is_new)
    {
        const char * msg = new_collection_row(entry, config->table, config->image_path);
        if(msg) 
        {
            JSArray errmsg;
            errmsg.Push(ToWebString("Item creation failed with error message '"+std::string(msg)+"'"));
            errmsg.Push(JSValue(false));
            window.InvokeAsync( WSLit("status"), errmsg);
            return;
        }
        JSArray success;
        success.Push(WSLit("Item saved"));
        window.InvokeAsync( WSLit("status"), success);
    }
    else
    {
        std::vector<std::string> to_update;
        if (config->active_collection_item.purchase != entry.purchase)
        {
            to_update.push_back("num");
        }
        if (config->active_collection_item.cost != entry.cost)
        {
            to_update.push_back("cost");
        }
        if (config->active_collection_item.texture != entry.texture)
        {
            to_update.push_back("texture");
        }
        if (config->active_collection_item.order != entry.order)
        {
            to_update.push_back("order");
        }
        if (config->active_collection_item.name != entry.name)
        {
            to_update.push_back("name");
        }
        if (config->active_collection_item.description != entry.description)
        {
            to_update.push_back("desc");
        }
        if (config->active_collection_item.date != entry.date)
        {
            to_update.push_back("date");
        }
        if (config->active_collection_item.buyer != entry.buyer)
        {
            to_update.push_back("buyer");
        }
        if (config->active_collection_item.notes != entry.notes)
        {
            to_update.push_back("notes");
        }
        if (config->active_collection_item.buy_location != entry.buy_location)
        {
            to_update.push_back("from");
        }
        if (config->active_collection_item.now_location != entry.now_location)
        {
            to_update.push_back("location");
        }
        if (config->active_collection_item.mineral != entry.mineral)
        {
            to_update.push_back("type");
        }
        if (config->active_collection_item.colours != entry.colours)
        {
            to_update.push_back("colours");
        }

        if (config->active_collection_item.images.size() != entry.source_images.size())
        {
            to_update.push_back("images");
        }
        else
        {
            for (unsigned int i = 0; i < entry.source_images.size(); i++)
            {
                if (config->active_collection_item.images[i] != wstr_trunc(entry.source_images[i]))
                {
                    to_update.push_back("images");
                    break;
                }
            }
        }

        if (to_update.size())
        {
            const char * msg = update_collection_item(entry, config->active_collection_item, config->table, to_update, config->image_path);
            if(msg) 
            {
                JSArray errmsg;
                errmsg.Push(ToWebString("Item edit failed with error message '"+std::string(msg)+"'"));
                errmsg.Push(JSValue(false));
                window.InvokeAsync( WSLit("status"), errmsg);
                return;
            }
            if (!((unsigned int)config->collection_current_image < config->active_collection_item.images.size()))
            {
                config->collection_current_image = config->active_collection_item.images.size() == 0 ? -1 : 0;
                SyncConfigObjects("config");
            }
            JSArray success;
            success.Push(WSLit("Item successfully updated"));
            window.InvokeAsync( WSLit("status"), success);

            SyncConfigObjects("config.active_collection_item");
            window.InvokeAsync( WSLit("display_collection_item"), args);
            window.InvokeAsync( WSLit("update_collection_results"), args);
        }
        else
        {
            JSArray success;
            success.Push(WSLit("No changes made"));
            window.InvokeAsync( WSLit("status"), success);
        }
    }
}

void ProgramInstance::save_mineral_item(WebView* web_view, const JSArray& args)
{
    JSObject data;
    JSValue jdata = window.Invoke(WSLit("save_mineral_item"), args);
    if (jdata.IsObject())
    {
        config->is_editing = false;
        data = jdata.ToObject();
    }
    else return;
    mineral_entry entry;

    entry.pagenumber    = jc.to_int( data.GetProperty( WSLit("pagenumber") ) );
    entry.category      = jc.to_string( data.GetProperty( WSLit("category") ) );
    entry.name          = jc.to_string( data.GetProperty( WSLit("name") ) );
    entry.description   = jc.to_string( data.GetProperty( WSLit("description") ) );
    entry.structure     = jc.to_string( data.GetProperty( WSLit("structure") ) );
    entry.composition   = jc.to_string( data.GetProperty( WSLit("composition") ) );
    entry.varieties     = jc.to_vectorstring( data.GetProperty( WSLit("varieties") ) );
    entry.colours       = jc.to_vectorstring( data.GetProperty( WSLit("colours") ) );
    entry.source_images = jc.to_vectorwstring( data.GetProperty( WSLit("images") ) );

    size_t pos;
    for (unsigned int i = 0; i < entry.colours.size(); i++)
    {
        pos = entry.colours[i].find('\n');
        while (pos != std::string::npos)
        {
            entry.colours[i].erase(pos, 1);
            pos = entry.colours[i].find('\n');
        }
    }
    for (unsigned int i = 0; i < entry.source_images.size(); i++)
    {
        size_t pos = entry.source_images[i].find('\n');
        while (pos != std::wstring::npos)
        {
            entry.source_images[i].erase(pos, 1);
            pos = entry.source_images[i].find('\n');
        }
    }
    for (unsigned int i = 0; i < entry.varieties.size(); i++)
    {
        size_t pos = entry.varieties[i].find('\n');
        while (pos != std::string::npos)
        {
            entry.varieties[i].erase(pos, 1);
            pos = entry.varieties[i].find('\n');
        }
    }
    pos = entry.category.find('\n');
    while (pos != std::string::npos)
    {
        entry.category.erase(pos, 1);
        pos = entry.category.find('\n');
    }
    pos = entry.name.find('\n');
    while (pos != std::string::npos)
    {
        entry.name.erase(pos, 1);
        pos = entry.name.find('\n');
    }
    pos = entry.structure.find('\n');
    while (pos != std::string::npos)
    {
        entry.structure.erase(pos, 1);
        pos = entry.structure.find('\n');
    }
    pos = entry.composition.find('\n');
    while (pos != std::string::npos)
    {
        entry.composition.erase(pos, 1);
        pos = entry.composition.find('\n');
    }

    if (config->is_new)
    {
        const char * msg = new_mineral_row(entry, config->image_path);
        if(msg) 
        {
            JSArray errmsg;
            errmsg.Push(ToWebString("Item creation failed with error message '"+std::string(msg)+"'"));
            errmsg.Push(JSValue(false));
            window.InvokeAsync( WSLit("status"), errmsg);
            return;
        }
        JSArray success;
        success.Push(WSLit("Item saved"));
        window.InvokeAsync( WSLit("status"), success);
    }
    else
    {
        std::vector<std::string> to_update;
        if (config->active_mineral_item.pagenumber != entry.pagenumber)
        {
            to_update.push_back("pagenum");
        }
        if (config->active_mineral_item.category != entry.category)
        {
            to_update.push_back("category");
        }
        if (config->active_mineral_item.name != entry.name)
        {
            to_update.push_back("name");
        }
        if (config->active_mineral_item.description != entry.description)
        {
            to_update.push_back("desc");
        }
        if (config->active_mineral_item.structure != entry.structure)
        {
            to_update.push_back("structure");
        }
        if (config->active_mineral_item.composition != entry.composition)
        {
            to_update.push_back("formula");
        }
        if (config->active_mineral_item.varieties != entry.varieties)
        {
            to_update.push_back("varieties");
        }
        if (config->active_mineral_item.colours != entry.colours)
        {
            to_update.push_back("colours");
        }

        if (config->active_mineral_item.images.size() != entry.source_images.size())
        {
            to_update.push_back("images");
        }
        else
        {
            for (unsigned int i = 0; i < entry.source_images.size(); i++)
            {
                if (config->active_mineral_item.images[i] != wstr_trunc(entry.source_images[i]))
                {
                    to_update.push_back("images");
                    break;
                }
            }
        }

        if(to_update.size())
        {
            const char * msg = update_mineral_item(entry, config->active_mineral_item, to_update, config->image_path);
            if(msg) 
            {
                JSArray errmsg;
                errmsg.Push(ToWebString("Item edit failed with error message '"+std::string(msg)+"'"));
                errmsg.Push(JSValue(false));
                window.InvokeAsync( WSLit("status"), errmsg);
                return;
            }
            if (!((unsigned int)config->mineral_current_image < config->active_mineral_item.images.size()))
            {
                config->mineral_current_image = config->active_mineral_item.images.size() == 0 ? -1 : 0;
                SyncConfigObjects("config");
            }
            JSArray success;
            success.Push(WSLit("Item successfully updated"));
            window.InvokeAsync( WSLit("status"), success);

            SyncConfigObjects("config.active_mineral_item");
            window.InvokeAsync( WSLit("display_mineral_item"), args);
            window.InvokeAsync( WSLit("update_mineral_results"), args);
        }
        else
        {
            JSArray success;
            success.Push(WSLit("No changes made"));
            window.InvokeAsync( WSLit("status"), success);
        }
    }
}

void ProgramInstance::activate_mineral_item(WebView* web_view, const JSArray& args)
{
    config->minerals_results.active_item = args[0].ToInteger();
    get_mineral_columns(config->minerals_results.active_item, {}, config->active_mineral_item);
    SyncConfigObjects("config.active_mineral_item");
    JSArray ja;
    window.InvokeAsync(WSLit("display_mineral_item"), ja);
}

void ProgramInstance::activate_collection_item(WebView* web_view, const JSArray& args)
{
    config->collection_results.active_item = args[0].ToInteger();
    get_collection_columns(config->collection_results.active_item, {}, config->active_collection_item, config->table);
    SyncConfigObjects("config.active_collection_item");
    JSArray ja;
    window.InvokeAsync(WSLit("display_collection_item"), ja);
}

void ProgramInstance::arbitrary_js(WebView* web_view, const JSArray& args)
{
    JSArray ja;
    for (unsigned int i = 1; i < args.size(); i++)
    {
        ja.Push(args[i]);
    }
    window.Invoke(args[0].ToString(), ja);
}

void ProgramInstance::printmsg(WebView* caller, const JSArray& args)
{
    if (args.size())
    {
        //app_->ShowMessage(web_str(args[0]).c_str());
        std::cout << web_str(args[0]) << std::endl;
    }
}
