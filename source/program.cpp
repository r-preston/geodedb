#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <Awesomium/WebCore.h>
#include <Awesomium/STLHelpers.h>

#include "headers/method_dispatcher.h"
#include "headers/structures.h"
#include "headers/program.h"
#include "headers/stonebase.h"
#include "headers/application.h"
#include "headers/parsing.h"

using namespace Awesomium;

bool javascript_probe(JSValue j)
{
    if (!(j.IsArray()))
    {
        return true;
    }
    if(j.ToArray().size() != 2)
    {
        return true;
    }
    if(!(j.ToArray()[0].IsInteger()))
    {
        return true;
    }
    if(!(j.ToArray()[1].IsString()))
    {
        return true;
    }
    if(j.ToArray()[0].ToInteger() != 1)
    {
        return true;
    }
    if(j.ToArray()[1].ToString() != WSLit("*"))
    {
        return true;
    }
    return false;
}

std::string ProgramInstance::web_str(JSValue JSVal)
{
    return ToString(JSVal.ToString());
}

WebString WideWebString(std::wstring wstr)
{
    return WebString(reinterpret_cast<const wchar16*>(wstr.c_str()));
}

int * window_size(int dimensions[])
{
    // gets the usable screen area and return 90% of it
    RECT s;

    SystemParametersInfo(SPI_GETWORKAREA, NULL, &s, NULL);

    dimensions[0] = (int)(0.9*abs(s.right - s.left));
    dimensions[1] = (int)(0.9*abs(s.top - s.bottom));

    return &dimensions[0];
}

void ProgramInstance::create_javascript_objects()
{
    /*
        Creates Global JavaScript objects that correspond to the program_config class
    */
    // used for initialising properties as lists
    const JSArray t_a;
    JSValue result;

    WebView* web_view = view_->web_view();

    result = JSValue();
    unsigned int max_tries = 0;
    while (result.IsUndefined())
    {
        max_tries++;
        result = web_view->CreateGlobalJavascriptObject(WSLit("program_config"));
        if (max_tries == 100)
        {
            exit(2);
        }
    }
    js_config = result.ToObject();

    /*
        JavaScript version of the following

        function CollectionEntry()
        {
            this.rowid = -1;
            this.number = -1;
            this.order = '';
            this.name = '';
            this.type = [];
            this.description = '';
            this.colours = [];
            this.date = '';
            this.purchaseplace = '';
            this.notes = '';
            this.cost = -1;
            this.buyer = '';
            this.location = '';
            this.images = [];
            this.texture = -1;
        }

        function MineralEntry()
        {
            this.rowid = -1;
            this.name = '';
            this.varieties = [];
            this.description = '';
            this.colours = [];
            this.structure = '';
            this.composition = '';
            this.images = [];
            this.category = '';
            this.pagenumber = -1;
        }

        function SearchResult()
        {
            this.tablename = '';
            this.results = [];
            this.sort_mask = 0; // 0=name, 1=purchase number, 2=date, 3=cost, 4=mineral type. for the Minerals table, only name applies
            this.sort_ascending = true;
            this.search_or = false; // false = AND, true = OR when searching
            this.datecomp = 0; // not applicable to Minerals table
            this.costcomp = 0; // ditto
            this.numcomp  = 0; // ditto
        }

        function ProgramConfig()
        {
            this.table = '';
            this.collection_table = '';
            this.collection_table_list = [];
            this.active_item_table = '';

            this.collection_results = new SearchResult();
            this.minerals_results = new SearchResult();
            this.active_collection_item = new CollectionEntry();
            this.active_mineral_item = new MineralEntry();
            this.alternate_table_info = [];
            this.alternate_table_items = [];

            this.collection_current_image = -1;
            this.mineral_current_image = -1;

            this.visible_collection_columns = [];
            this.collection_column_widths = [];

            this.visible_mineral_columns = [];
            this.mineral_column_widths = [];
        }
    */

    // program_config
    js_config.SetProperty( WSLit("is_new"), JSValue(false));

    js_config.SetProperty( WSLit("image_path"), WSLit(""));

    js_config.SetProperty( WSLit("table")                     , WSLit("")    );
    js_config.SetProperty( WSLit("collection_table")          , WSLit("")    );
    js_config.SetProperty( WSLit("collection_table_list")     , JSValue(t_a) );
    js_config.SetProperty( WSLit("active_item_table")         , WSLit("")    );

    result = web_view->CreateGlobalJavascriptObject( WSLit("program_config.collection_results") ); // SearchResult
        JSObject collection_results = result.ToObject();
    result = web_view->CreateGlobalJavascriptObject( WSLit("program_config.minerals_results") ); // SearchResult
        JSObject minerals_results = result.ToObject();
    result = web_view->CreateGlobalJavascriptObject( WSLit("program_config.active_collection_item") ); // CollectionEntry
        JSObject active_collection_item = result.ToObject();
    result = web_view->CreateGlobalJavascriptObject( WSLit("program_config.active_mineral_item") ); // MineralEntry
        JSObject active_mineral_item = result.ToObject();

    js_config.SetProperty( WSLit("alternate_table_info")      , JSValue(t_a) );

    js_config.SetProperty( WSLit("collection_current_image")  , JSValue(-1)  );
    js_config.SetProperty( WSLit("mineral_current_image")     , JSValue(-1)  );

    js_config.SetProperty( WSLit("visible_collection_columns"), JSValue(t_a) );
    js_config.SetProperty( WSLit("collection_column_widths")  , JSValue(t_a) );

    js_config.SetProperty( WSLit("visible_mineral_columns")   , JSValue(t_a) );
    js_config.SetProperty( WSLit("mineral_column_widths")     , JSValue(t_a) );

    // program_config.collection_results
    collection_results.SetProperty( WSLit("tablename")     , WSLit("")      );
    collection_results.SetProperty( WSLit("results")       , JSValue(t_a)   );
    collection_results.SetProperty( WSLit("active_item")   , JSValue(0)     );
    collection_results.SetProperty( WSLit("sort_mask")     , JSValue(0)     );
    collection_results.SetProperty( WSLit("sort_ascending"), JSValue(true)  );
    collection_results.SetProperty( WSLit("search_or")     , JSValue(false) );
    collection_results.SetProperty( WSLit("datecomp")      , JSValue(0)     );
    collection_results.SetProperty( WSLit("costcomp")      , JSValue(0)     );
    collection_results.SetProperty( WSLit("numcomp")       , JSValue(0)     );

    // program_config.minerals_results
    minerals_results.SetProperty( WSLit("tablename")     , WSLit("")      );
    minerals_results.SetProperty( WSLit("results")       , JSValue(t_a)   );
    minerals_results.SetProperty( WSLit("active_item")   , JSValue(0)     );
    minerals_results.SetProperty( WSLit("sort_mask")     , JSValue(0)     );
    minerals_results.SetProperty( WSLit("sort_ascending"), JSValue(true)  );
    minerals_results.SetProperty( WSLit("search_or")     , JSValue(false) );
    minerals_results.SetProperty( WSLit("datecomp")      , JSValue(0)     );
    minerals_results.SetProperty( WSLit("costcomp")      , JSValue(0)     );
    minerals_results.SetProperty( WSLit("numcomp")       , JSValue(0)     );

    // program_config.active_collection_item
    active_collection_item.SetProperty( WSLit("rowid")        , JSValue(-1)  );
    active_collection_item.SetProperty( WSLit("number")       , JSValue(-1)  );
    active_collection_item.SetProperty( WSLit("order")        , WSLit("")    );
    active_collection_item.SetProperty( WSLit("name")         , WSLit("")    );
    active_collection_item.SetProperty( WSLit("type")         , JSValue(t_a) );
    active_collection_item.SetProperty( WSLit("description")  , WSLit("")    );
    active_collection_item.SetProperty( WSLit("colours")      , JSValue(t_a) );
    active_collection_item.SetProperty( WSLit("date")         , WSLit("")    );
    active_collection_item.SetProperty( WSLit("purchaseplace"), WSLit("")    );
    active_collection_item.SetProperty( WSLit("notes")        , WSLit("")    );
    active_collection_item.SetProperty( WSLit("cost")         , JSValue(-1)  );
    active_collection_item.SetProperty( WSLit("buyer")        , WSLit("")    );
    active_collection_item.SetProperty( WSLit("location")     , WSLit("")    );
    active_collection_item.SetProperty( WSLit("images")       , JSValue(t_a) );
    active_collection_item.SetProperty( WSLit("texture")      , JSValue(-1)  );

    // program_config.active_mineral_item
    active_mineral_item.SetProperty( WSLit("rowid")      , JSValue(-1)  );
    active_mineral_item.SetProperty( WSLit("name")       , WSLit("")    );
    active_mineral_item.SetProperty( WSLit("varieties")  , JSValue(t_a) );
    active_mineral_item.SetProperty( WSLit("description"), WSLit("")    );
    active_mineral_item.SetProperty( WSLit("colours")    , JSValue(t_a) );
    active_mineral_item.SetProperty( WSLit("structure")  , WSLit("")    );
    active_mineral_item.SetProperty( WSLit("composition"), WSLit("")    );
    active_mineral_item.SetProperty( WSLit("images")     , JSValue(t_a) );
    active_mineral_item.SetProperty( WSLit("category")   , WSLit("")    );
    active_mineral_item.SetProperty( WSLit("pagenumber") , JSValue(-1)  );
}

void ProgramInstance::OnLoaded()
{
    // load config from file
    config->load();

    if (!compare_files(config->db_path.c_str(), "Stones.db.boot", "Stones.db.close"))
    {
        // the database has been modified since the program was last closed
        for (search_result c : config->alternate_table_info)
        {
            c.results = {};
        }
        config->minerals_results.results = {};
        mineral_entry mentry;
        config->active_mineral_item = mentry;
    }

    // get screen dimensions
    int screenres[2]; // allocate space for two integers
    int * screen_dimensions = window_size(screenres); // return pointer to first integer

    // create a new view
    view_ = View::Create(screen_dimensions[0], screen_dimensions[1]);

    // create a helpful pointer to the WebView object
    WebView* web_view = view_->web_view();
    web_core = app_->web_core();

    // maximise the window
    HWND hWnd = web_view->parent_window();
    ShowWindow(hWnd, 3);
    // set program icons
    HICON hIcon = (HICON)LoadImage(NULL, "ui/resource/icon.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
    SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

    // initialise the javascript objects corresponding to program_config
    create_javascript_objects();
    web_core->Update();

    // bind C++ JS listeners and event hooks
    BindMethods(web_view);
    web_core->Update();

    // load the GUI
    WebURL url(WSLit("file:///ui/ui.html"));
    web_view->LoadURL(url);
    web_core->Update();

    // get the V8 window object, allows calling of JavaScript functions
    JSValue getWindow = web_view->ExecuteJavascriptWithResult(WSLit("window"), WSLit(""));
    if (getWindow.IsObject())
    {
        window = getWindow.ToObject();
    }
    else
    {
        // this is bad
        std::cout << "Could not retrieve window object" << std::endl;
        app_->ShowMessage("Could not retrieve window object");
        exit(1);
    }

    js_config.SetProperty( WSLit("table"), WSLit("*"));
    web_core->Update();
    JSValue probe_result = window.Invoke(WSLit("startup_probe"), {});
    while (javascript_probe(probe_result))
    {
        js_config.SetProperty( WSLit("table"), WSLit("*"));
        web_core->Update();
        probe_result = window.Invoke(WSLit("startup_probe"), {});
    }

    // port config to JavaScript 
    SyncConfigObjects();
    web_core->Update();

    JSArray args;
    // call the GUI's startup functions
    //   main startup function
    JSValue result = window.Invoke(WSLit("init"), args);
    //   eliminates the mysterious pixel problem, where result columns are offset from labels by 1 pixel
    window.Invoke(WSLit("validate_column_widths"), args);
}

void ProgramInstance::BindMethods(WebView* web_view)
{
    // Create a global js object named 'app'
    JSValue result = web_view->CreateGlobalJavascriptObject(WSLit("ui"));
    if (result.IsObject())
    {
        // Bind our custom method to it.
        JSObject& app_object = result.ToObject();
        ui_id = app_object.remote_id();

        // bind event hooks
        /*method_dispatcher_.BindWithRetval(app_object,
                                          WSLit("confirm"),
                                          JSDelegateWithRetval(this, &ProgramInstance::confirm));*/
        method_dispatcher_.BindWithRetval(app_object,
                                          WSLit("add_collection_items"),
                                          JSDelegateWithRetval(this, &ProgramInstance::add_collection_items));
        method_dispatcher_.BindWithRetval(app_object,
                                          WSLit("add_mineral_items"),
                                          JSDelegateWithRetval(this, &ProgramInstance::add_mineral_items));
        method_dispatcher_.BindWithRetval(app_object,
                                          WSLit("delete_collection_item"),
                                          JSDelegateWithRetval(this, &ProgramInstance::delete_collection_item));
        method_dispatcher_.BindWithRetval(app_object,
                                          WSLit("delete_mineral_item"),
                                          JSDelegateWithRetval(this, &ProgramInstance::delete_mineral_item));
        method_dispatcher_.Bind(app_object,
                                WSLit("remove_collection_image"),
                                JSDelegate(this, &ProgramInstance::remove_collection_image));
        method_dispatcher_.Bind(app_object,
                                WSLit("view_image"),
                                JSDelegate(this, &ProgramInstance::view_image));
        method_dispatcher_.Bind(app_object,
                                WSLit("remove_mineral_image"),
                                JSDelegate(this, &ProgramInstance::remove_mineral_image));
        method_dispatcher_.Bind(app_object,
                                WSLit("choose_images"),
                                JSDelegate(this, &ProgramInstance::choose_images));
        method_dispatcher_.Bind(app_object,
                                WSLit("close_addfile_popups"),
                                JSDelegate(this, &ProgramInstance::close_addfile_popups));
        method_dispatcher_.Bind(app_object,
                                WSLit("sync_item"),
                                JSDelegate(this, &ProgramInstance::sync_item));
        method_dispatcher_.Bind(app_object,
                                WSLit("add_from_file"),
                                JSDelegate(this, &ProgramInstance::add_from_file));
        method_dispatcher_.Bind(app_object,
                                WSLit("set_mask"),
                                JSDelegate(this, &ProgramInstance::set_mask));
        method_dispatcher_.Bind(app_object,
                                WSLit("is_new"),
                                JSDelegate(this, &ProgramInstance::is_new));
        method_dispatcher_.Bind(app_object,
                                WSLit("confirm_callback"),
                                JSDelegate(this, &ProgramInstance::confirm_callback));
        method_dispatcher_.Bind(app_object,
                                WSLit("arbitrary_js"),
                                JSDelegate(this, &ProgramInstance::arbitrary_js));
        method_dispatcher_.Bind(app_object,
                                WSLit("confirm"),
                                JSDelegate(this, &ProgramInstance::confirm));
        method_dispatcher_.Bind(app_object,
                                WSLit("continue_delete_table"),
                                JSDelegate(this, &ProgramInstance::continue_delete_table));
        method_dispatcher_.Bind(app_object,
                                WSLit("confirm_delete_item"),
                                JSDelegate(this, &ProgramInstance::confirm_delete_item));
        method_dispatcher_.Bind(app_object,
                                WSLit("check_table_prompt"),
                                JSDelegate(this, &ProgramInstance::check_table_prompt));
        method_dispatcher_.Bind(app_object,
                                WSLit("get_table_name"),
                                JSDelegate(this, &ProgramInstance::get_table_name));
        method_dispatcher_.Bind(app_object,
                                WSLit("printmsg"),
                                JSDelegate(this, &ProgramInstance::printmsg));
        method_dispatcher_.Bind(app_object,
                                WSLit("toggle_mineral_column"),
                                JSDelegate(this, &ProgramInstance::toggle_mineral_column));
        method_dispatcher_.Bind(app_object,
                                WSLit("toggle_collection_column"),
                                JSDelegate(this, &ProgramInstance::toggle_collection_column));
        method_dispatcher_.Bind(app_object,
                                WSLit("activate_collection_table"),
                                JSDelegate(this, &ProgramInstance::activate_collection_table));
        method_dispatcher_.Bind(app_object,
                                WSLit("choose_new_table"),
                                JSDelegate(this, &ProgramInstance::choose_new_table));
        method_dispatcher_.Bind(app_object,
                                WSLit("dropdown"),
                                JSDelegate(this, &ProgramInstance::dropdown));
        method_dispatcher_.Bind(app_object,
                                WSLit("create_new_table"),
                                JSDelegate(this, &ProgramInstance::create_new_table));
        method_dispatcher_.Bind(app_object,
                                WSLit("activate_mineral_table"),
                                JSDelegate(this, &ProgramInstance::activate_mineral_table));
        method_dispatcher_.Bind(app_object,
                                WSLit("help_popup"),
                                JSDelegate(this, &ProgramInstance::help_popup));
        method_dispatcher_.Bind(app_object,
                                WSLit("clear_collection_form"),
                                JSDelegate(this, &ProgramInstance::clear_collection_form));
        method_dispatcher_.Bind(app_object,
                                WSLit("search_collection_db"),
                                JSDelegate(this, &ProgramInstance::search_collection_db));
        method_dispatcher_.Bind(app_object,
                                WSLit("new_item"),
                                JSDelegate(this, &ProgramInstance::new_item));
        method_dispatcher_.Bind(app_object,
                                WSLit("clear_mineral_form"),
                                JSDelegate(this, &ProgramInstance::clear_mineral_form));
        method_dispatcher_.Bind(app_object,
                                WSLit("search_mineral_db"),
                                JSDelegate(this, &ProgramInstance::search_mineral_db));
        method_dispatcher_.Bind(app_object,
                                WSLit("change_collection_sorting_mask"),
                                JSDelegate(this, &ProgramInstance::change_collection_sorting_mask));
        method_dispatcher_.Bind(app_object,
                                WSLit("change_mineral_sorting_mask"),
                                JSDelegate(this, &ProgramInstance::change_mineral_sorting_mask));
        method_dispatcher_.Bind(app_object,
                                WSLit("decrement_image"),
                                JSDelegate(this, &ProgramInstance::decrement_image));
        method_dispatcher_.Bind(app_object,
                                WSLit("advance_image"),
                                JSDelegate(this, &ProgramInstance::advance_image));
        method_dispatcher_.Bind(app_object,
                                WSLit("edit_collection_item"),
                                JSDelegate(this, &ProgramInstance::edit_collection_item));
        method_dispatcher_.Bind(app_object,
                                WSLit("edit_mineral_item"),
                                JSDelegate(this, &ProgramInstance::edit_mineral_item));
        method_dispatcher_.Bind(app_object,
                                WSLit("cancel_item_edit"),
                                JSDelegate(this, &ProgramInstance::cancel_item_edit));
        method_dispatcher_.Bind(app_object,
                                WSLit("save_collection_item"),
                                JSDelegate(this, &ProgramInstance::save_collection_item));
        method_dispatcher_.Bind(app_object,
                                WSLit("save_mineral_item"),
                                JSDelegate(this, &ProgramInstance::save_mineral_item));
        method_dispatcher_.Bind(app_object,
                                WSLit("remove_table"),
                                JSDelegate(this, &ProgramInstance::remove_table));
        method_dispatcher_.Bind(app_object,
                                WSLit("activate_mineral_item"),
                                JSDelegate(this, &ProgramInstance::activate_mineral_item));
        method_dispatcher_.Bind(app_object,
                                WSLit("activate_collection_item"),
                                JSDelegate(this, &ProgramInstance::activate_collection_item));
    }

    // Bind our method dispatcher to the WebView
    web_view->set_js_method_handler(&method_dispatcher_);
}

void ProgramInstance::SyncConfigObjects(const std::string group)
{
    //SyncConfigObjectsLegacy(group);
    //return;
    char seperator = '\x001e';

    if ((group == "config") || (group == ""))
    {
        std::string data;
        JSArray args;

        data += config->table + seperator;
        data += config->collection_table + seperator;
        data += vec_to_str(config->collection_table_list) + seperator;
        data += config->active_item_table + seperator;
        data += byte_to_string(config->collection_current_image) + seperator;
        data += byte_to_string(config->mineral_current_image) + seperator;
        data += vec_to_str(config->visible_collection_columns) + seperator;
        data += vec_to_str(config->collection_column_widths) + seperator;
        data += vec_to_str(config->visible_mineral_columns) + seperator;
        data += vec_to_str(config->mineral_column_widths) + seperator;
        if(config->image_path[1] != ':') // if path is relative
            data += "../"; // adjust path to get out of ui/ directory
        data += config->image_path;

        args.Push(ToWebString(data));
        args.Push(WSLit("program_config"));
        window.Invoke( WSLit("apply_synced_config"), args);
    }

    if ((group == "config.collection_results") || (group == ""))
    {
        std::string data;
        JSArray args;
        data += config->collection_results.tablename + seperator;
        data += vector_int_to_string(config->collection_results.results) + seperator;
        data += byte_to_string(config->collection_results.active_item) + seperator;
        data += byte_to_string(config->collection_results.sort_mask) + seperator;
        data += byte_to_string(config->collection_results.sort_ascending) + seperator;
        data += byte_to_string(config->collection_results.search_or) + seperator;
        data += byte_to_string(config->collection_results.datecomp) + seperator;
        data += byte_to_string(config->collection_results.costcomp) + seperator;
        data += byte_to_string(config->collection_results.numcomp);

        args.Push(ToWebString(data));
        args.Push(WSLit("program_config.collection_results"));
        window.Invoke( WSLit("apply_synced_config"), args);
    }

    if ((group == "config.minerals_results") || (group == ""))
    {
        std::string data;
        JSArray args;
        data += config->minerals_results.tablename + seperator;
        data += vector_int_to_string(config->minerals_results.results) + seperator;
        data += byte_to_string(config->minerals_results.active_item) + seperator;
        data += byte_to_string(config->minerals_results.sort_mask) + seperator;
        data += byte_to_string(config->minerals_results.sort_ascending) + seperator;
        data += byte_to_string(config->minerals_results.search_or);

        args.Push(ToWebString(data));
        args.Push(WSLit("program_config.minerals_results"));
        window.Invoke( WSLit("apply_synced_config"), args);
    }

    if ((group == "config.active_collection_item") || (group == ""))
    {
        std::string data;
        JSArray args;
        data += byte_to_string(config->active_collection_item.id) + seperator;
        data += byte_to_string(config->active_collection_item.purchase) + seperator;
        data += byte_to_string(config->active_collection_item.cost) + seperator;
        data += byte_to_string(config->active_collection_item.texture) + seperator;
        data += config->active_collection_item.order + seperator;
        data += config->active_collection_item.name + seperator;
        data += config->active_collection_item.description + seperator;
        data += config->active_collection_item.date + seperator;
        data += config->active_collection_item.buyer + seperator;
        data += config->active_collection_item.notes + seperator;
        data += config->active_collection_item.buy_location + seperator;
        data += config->active_collection_item.now_location + seperator;
        data += vec_to_str(config->active_collection_item.mineral) + seperator;
        data += vec_to_sepstring(config->active_collection_item.images) + seperator;
        data += vec_to_str(config->active_collection_item.colours);

        args.Push(ToWebString(data));
        args.Push(WSLit("program_config.active_collection_item"));
        window.Invoke( WSLit("apply_synced_config"), args);
    }

    if ((group == "config.active_mineral_item") || (group == ""))
    {
        std::string data;
        JSArray args;
        data += byte_to_string(config->active_mineral_item.id) + seperator;
        data += byte_to_string(config->active_mineral_item.pagenumber) + seperator;
        data += config->active_mineral_item.category + seperator;
        data += config->active_mineral_item.name + seperator;
        data += config->active_mineral_item.description + seperator;
        data += config->active_mineral_item.structure + seperator;
        data += config->active_mineral_item.composition + seperator;
        data += vec_to_str(config->active_mineral_item.varieties) + seperator;
        data += vec_to_str(config->active_mineral_item.colours) + seperator;
        data += vec_to_sepstring(config->active_mineral_item.images);

        args.Push(ToWebString(data));
        args.Push(WSLit("program_config.active_mineral_item"));
        window.Invoke( WSLit("apply_synced_config"), args);
    }
}

void ProgramInstance::SyncConfigObjectsLegacy(const std::string group)
{
    // This is the legacy version of SyncConfigObjects (it's actually v2.0, see bottom of the function)
    // It has been replaced in favour of an even faster single string based version

    /*
    void SetProperty(const WebString& name, const JSValue& value);
    JSValue GetProperty(const WebString& name) const;
    bool HasProperty(const WebString& name) const;

    Specifiers:

    "config"
    "config.collection_results"
    "config.minerals_results"
    "config.active_collection_item"
    "config.active_mineral_item"
    */

    if ((group == "config") || (group == ""))
    {
        JSArray data, args;
        data.Push( jc.to_js( config->table ) );
        data.Push( jc.to_js( config->collection_table ) );
        data.Push( jc.to_js( config->collection_table_list ) );
        data.Push( jc.to_js( config->active_item_table ) );
        data.Push( jc.to_js( config->collection_current_image ) );
        data.Push( jc.to_js( config->mineral_current_image ) );
        data.Push( jc.to_js( config->visible_collection_columns ) );
        data.Push( jc.to_js( config->collection_column_widths ) );
        data.Push( jc.to_js( config->visible_mineral_columns ) );
        data.Push( jc.to_js( config->mineral_column_widths ) );
        data.Push( jc.to_js( config->image_path ) );

        args.Push(data);
        args.Push(WSLit("program_config"));
        window.Invoke( WSLit("apply_synced_config_legacy"), args);
    }
        
    if ((group == "config.collection_results") || (group == ""))
    {    
        JSArray data, args;
        data.Push( jc.to_js( config->collection_results.tablename ) );
        data.Push( jc.to_js( config->collection_results.results ) );
        data.Push( jc.to_js( config->collection_results.active_item ) );
        data.Push( jc.to_js( config->collection_results.sort_mask ) );
        data.Push( jc.to_js( config->collection_results.sort_ascending ) );
        data.Push( jc.to_js( config->collection_results.search_or ) );
        data.Push( jc.to_js( config->collection_results.datecomp ) );
        data.Push( jc.to_js( config->collection_results.costcomp ) );
        data.Push( jc.to_js( config->collection_results.numcomp ) );

        args.Push(data);
        args.Push(WSLit("program_config.collection_results"));
        window.Invoke( WSLit("apply_synced_config_legacy"), args);
    }

    if ((group == "config.minerals_results") || (group == ""))
    {
        JSArray data, args;
        data.Push( jc.to_js( config->minerals_results.tablename ) );
        data.Push( jc.to_js( config->minerals_results.results ) );
        data.Push( jc.to_js( config->minerals_results.active_item ) );
        data.Push( jc.to_js( config->minerals_results.sort_mask ) );
        data.Push( jc.to_js( config->minerals_results.sort_ascending ) );
        data.Push( jc.to_js( config->minerals_results.search_or ) );

        args.Push(data);
        args.Push(WSLit("program_config.minerals_results"));
        window.Invoke( WSLit("apply_synced_config_legacy"), args);
    }

    if ((group == "config.active_collection_item") || (group == ""))
    {
        JSArray data, args;
        data.Push( jc.to_js( config->active_collection_item.id ) );
        data.Push( jc.to_js( config->active_collection_item.purchase ) );
        data.Push( jc.to_js( config->active_collection_item.cost ) );
        data.Push( jc.to_js( config->active_collection_item.texture ) );
        data.Push( jc.to_js( config->active_collection_item.order ) );
        data.Push( jc.to_js( config->active_collection_item.name ) );
        data.Push( jc.to_js( config->active_collection_item.description ) );
        data.Push( jc.to_js( config->active_collection_item.date ) );
        data.Push( jc.to_js( config->active_collection_item.buyer ) );
        data.Push( jc.to_js( config->active_collection_item.notes ) );
        data.Push( jc.to_js( config->active_collection_item.buy_location ) );
        data.Push( jc.to_js( config->active_collection_item.now_location ) );
        data.Push( jc.to_js( config->active_collection_item.mineral ) );
        data.Push( jc.to_js( config->active_collection_item.images ) );
        data.Push( jc.to_js( config->active_collection_item.colours ) );

        args.Push(data);
        args.Push(WSLit("program_config.active_collection_item"));
        window.Invoke( WSLit("apply_synced_config_legacy"), args);
    }

    if ((group == "config.active_mineral_item") || (group == ""))
    {
        JSArray data, args;
        data.Push( jc.to_js( config->active_mineral_item.id ) );
        data.Push( jc.to_js( config->active_mineral_item.pagenumber ) );
        data.Push( jc.to_js( config->active_mineral_item.category ) );
        data.Push( jc.to_js( config->active_mineral_item.name ) );
        data.Push( jc.to_js( config->active_mineral_item.description ) );
        data.Push( jc.to_js( config->active_mineral_item.structure ) );
        data.Push( jc.to_js( config->active_mineral_item.composition ) );
        data.Push( jc.to_js( config->active_mineral_item.varieties ) );
        data.Push( jc.to_js( config->active_mineral_item.colours ) );
        data.Push( jc.to_js( config->active_mineral_item.images ) );

        args.Push(data);
        args.Push(WSLit("program_config.active_mineral_item"));
        window.Invoke( WSLit("apply_synced_config_legacy"), args);
    }

    // This is the legacy legacy version! It is notable for its glacial slowness
   
    /*config->table = jc.to_string( js_config.GetProperty( WSLit("table") ) );
    config->collection_table = jc.to_string( js_config.GetProperty( WSLit("collection_table") ) );
    config->collection_table_list = jc.to_vectorstring( js_config.GetProperty( WSLit("collection_table_list") ) );
    config->active_item_table = jc.to_string( js_config.GetProperty( WSLit("active_item_table") ) );

    config->alternate_table_info = jc.to_vectorsearchresult( js_config.GetProperty( WSLit("alternate_table_info") ) );

    config->collection_current_image = jc.to_int( js_config.GetProperty( WSLit("collection_current_image") ) );
    config->mineral_current_image = jc.to_int( js_config.GetProperty( WSLit("mineral_current_image") ) );

    config->visible_collection_columns = jc.to_vectorstring( js_config.GetProperty( WSLit("visible_collection_columns") ) );
    config->collection_column_widths = jc.to_vectorstring( js_config.GetProperty( WSLit("collection_column_widths") ) );

    config->visible_mineral_columns = jc.to_vectorstring( js_config.GetProperty( WSLit("visible_mineral_columns") ) );
    config->mineral_column_widths = jc.to_vectorstring( js_config.GetProperty( WSLit("mineral_column_widths") ) );


    // then the children
    config->collection_results.tablename = jc.to_string( collection_results.GetProperty( WSLit("tablename") ) );
    config->collection_results.results = jc.to_vectorint( collection_results.GetProperty( WSLit("results") ) );
    config->collection_results.active_item = jc.to_int( collection_results.GetProperty( WSLit("active_item") ) );
    config->collection_results.sort_mask = jc.to_int( collection_results.GetProperty( WSLit("sort_mask") ) );
    config->collection_results.sort_ascending = jc.to_bool( collection_results.GetProperty( WSLit("sort_ascending") ) );
    config->collection_results.search_or = jc.to_bool( collection_results.GetProperty( WSLit("search_or") ) );
    config->collection_results.datecomp = jc.to_int( collection_results.GetProperty( WSLit("datecomp") ) );
    config->collection_results.costcomp = jc.to_int( collection_results.GetProperty( WSLit("costcomp") ) );
    config->collection_results.numcomp = jc.to_int( collection_results.GetProperty( WSLit("numcomp") ) );


    config->minerals_results.tablename = jc.to_string( minerals_results.GetProperty( WSLit("tablename") ) );
    config->minerals_results.results = jc.to_vectorint( minerals_results.GetProperty( WSLit("results") ) );
    config->minerals_results.active_item = jc.to_int( minerals_results.GetProperty( WSLit("active_item") ) );
    config->minerals_results.sort_mask = jc.to_int( minerals_results.GetProperty( WSLit("sort_mask") ) );
    config->minerals_results.sort_ascending = jc.to_bool( minerals_results.GetProperty( WSLit("sort_ascending") ) );
    config->minerals_results.search_or = jc.to_bool( minerals_results.GetProperty( WSLit("search_or") ) );
    config->minerals_results.datecomp = jc.to_int( minerals_results.GetProperty( WSLit("datecomp") ) );
    config->minerals_results.costcomp = jc.to_int( minerals_results.GetProperty( WSLit("costcomp") ) );
    config->minerals_results.numcomp = jc.to_int( minerals_results.GetProperty( WSLit("numcomp") ) );


    config->active_collection_item.id = jc.to_int( active_collection_item.GetProperty( WSLit("rowid") ) );
    config->active_collection_item.purchase = jc.to_int( active_collection_item.GetProperty( WSLit("number") ) );
    config->active_collection_item.cost = jc.to_int( active_collection_item.GetProperty( WSLit("cost") ) );
    config->active_collection_item.texture = jc.to_int( active_collection_item.GetProperty( WSLit("texture") ) );
    config->active_collection_item.order = jc.to_string( active_collection_item.GetProperty( WSLit("order") ) );
    config->active_collection_item.name = jc.to_string( active_collection_item.GetProperty( WSLit("name") ) );
    config->active_collection_item.description = jc.to_string( active_collection_item.GetProperty( WSLit("description") ) );
    config->active_collection_item.date = jc.to_string( active_collection_item.GetProperty( WSLit("date") ) );
    config->active_collection_item.buyer = jc.to_string( active_collection_item.GetProperty( WSLit("buyer") ) );
    config->active_collection_item.notes = jc.to_string( active_collection_item.GetProperty( WSLit("notes") ) );
    config->active_collection_item.buy_location = jc.to_string( active_collection_item.GetProperty( WSLit("purchaseplace") ) );
    config->active_collection_item.now_location = jc.to_string( active_collection_item.GetProperty( WSLit("location") ) );
    config->active_collection_item.mineral = jc.to_vectorstring( active_collection_item.GetProperty( WSLit("type") ) );
    config->active_collection_item.images = jc.to_vectorstring( active_collection_item.GetProperty( WSLit("images") ) );
    config->active_collection_item.colours = jc.to_vectorstring( active_collection_item.GetProperty( WSLit("colours") ) );


    config->active_mineral_item.id = jc.to_int( active_mineral_item.GetProperty( WSLit("rowid") ) );
    config->active_mineral_item.pagenumber = jc.to_int( active_mineral_item.GetProperty( WSLit("pagenumber") ) );
    config->active_mineral_item.category = jc.to_string( active_mineral_item.GetProperty( WSLit("category") ) );
    config->active_mineral_item.name = jc.to_string( active_mineral_item.GetProperty( WSLit("name") ) );
    config->active_mineral_item.description = jc.to_string( active_mineral_item.GetProperty( WSLit("description") ) );
    config->active_mineral_item.structure = jc.to_string( active_mineral_item.GetProperty( WSLit("structure") ) );
    config->active_mineral_item.composition = jc.to_string( active_mineral_item.GetProperty( WSLit("composition") ) );
    config->active_mineral_item.varieties = jc.to_vectorstring( active_mineral_item.GetProperty( WSLit("varieties") ) );
    config->active_mineral_item.colours = jc.to_vectorstring( active_mineral_item.GetProperty( WSLit("colours") ) );
    config->active_mineral_item.images = jc.to_vectorstring( active_mineral_item.GetProperty( WSLit("images") ) );*/
}

/*
 =========================================================================================
 =========================================================================================
*/

ProgramInstance::ProgramInstance() : app_(Application::Create()), view_(0)
{
    app_->set_listener(this);
}

ProgramInstance::~ProgramInstance()
{
    if (view_)
        app_->DestroyView(view_);
    if (app_)
        delete app_;
}

void ProgramInstance::Run()
{
    app_->Run();
}

// Inherited from Application::Listener
void ProgramInstance::OnUpdate()
{
    if (queue)
    {
        // dispatch any waiting processes
        queue = false;
    }
}

// Inherited from Application::Listener
void ProgramInstance::OnShutdown()
{
    // save program configuration
    config->save();
}
