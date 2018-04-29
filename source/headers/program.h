#ifndef STONES_PROGRAM_CLASSES_H_
#define STONES_PROGRAM_CLASSES_H_

#include <string>
#include <vector>

#include "application.h"
#include "method_dispatcher.h" // Awesomium JavaScript method things
#include "structures.h" // for the program_config data structure

#include <Awesomium/WebCore.h>
#include <Awesomium/STLHelpers.h>

using namespace Awesomium;

WebString WideWebString(std::wstring);

class JSConvert
{
public:
    JSValue to_js(std::string s);
    JSValue to_js(bool b);
    JSValue to_js(int i);
    JSValue to_js(std::vector<std::string> v);
    JSValue to_js(std::vector<int> v);

    std::vector<std::wstring> to_vectorwstring(JSValue j);
    std::string to_string(JSValue j);
    bool to_bool(JSValue j);
    int to_int(JSValue j);
    std::vector<std::string> to_vectorstring(JSValue j);
    std::vector<int> to_vectorint(JSValue j);
    std::vector<search_result> to_vectorsearchresult(JSValue j);
};

class ProgramInstance : public Application::Listener
{
    Application* app_;
    View* view_;
    WebCore* web_core;
    MethodDispatcher method_dispatcher_;
    JSObject window;
    JSConvert jc; // wrapper for JSValue <-> C++ type conversions
    unsigned int ui_id;
    bool queue = false;
public:
    JSObject js_config;
    program_config* config;

    ProgramInstance();
    ~ProgramInstance();

    void SyncConfigObjects(const std::string = "");
    void SyncConfigObjectsLegacy(const std::string = "");

    void Run();

    // Inherited from Application::Listener
    void OnLoaded();
    // Inherited from Application::Listener
    void OnUpdate();
    // Inherited from Application::Listener
    void OnShutdown();

    void BindMethods(WebView* web_view);

    void create_javascript_objects();

    // event hooks
    void remove_mineral_image(WebView* web_view, const JSArray& args);
    void remove_collection_image(WebView* web_view, const JSArray& args);
    void choose_images(WebView* web_view, const JSArray& args);
    void set_mask(WebView* web_view, const JSArray& args);
    void sync_item(WebView* web_view, const JSArray& args);
    void confirm(WebView* web_view, const JSArray& args);
    JSValue add_mineral_items(WebView* web_view, const JSArray& args);
    JSValue add_collection_items(WebView* web_view, const JSArray& args);
    void is_new(WebView* web_view, const JSArray& args);
    void confirm_callback(WebView* web_view, const JSArray& args);
    void add_from_file(WebView* web_view, const JSArray& args);
    void close_addfile_popups(WebView* web_view, const JSArray& args);
    void continue_delete_table(WebView* web_view, const JSArray& args);
    void confirm_delete_item(WebView* web_view, const JSArray& args);
    void check_table_prompt(WebView* web_view, const JSArray& args);
    void get_table_name(WebView* web_view, const JSArray& args);
    void printmsg(WebView* web_view, const JSArray& args);
    void toggle_mineral_column(WebView* web_view, const JSArray& args);
    void toggle_collection_column(WebView* web_view, const JSArray& args);
    void activate_collection_table(WebView* web_view, const JSArray& args);
    void dropdown(WebView* web_view, const JSArray& args);
    void create_new_table(WebView* web_view, const JSArray& args);
    void activate_mineral_table(WebView* web_view, const JSArray& args);
    void help_popup(WebView* web_view, const JSArray& args);
    void clear_collection_form(WebView* web_view, const JSArray& args);
    void search_collection_db(WebView* web_view, const JSArray& args);
    void new_item(WebView* web_view, const JSArray& args);
    void clear_mineral_form(WebView* web_view, const JSArray& args);
    void search_mineral_db(WebView* web_view, const JSArray& args);
    void change_collection_sorting_mask(WebView* web_view, const JSArray& args);
    void change_mineral_sorting_mask(WebView* web_view, const JSArray& args);
    void decrement_image(WebView* web_view, const JSArray& args);
    void advance_image(WebView* web_view, const JSArray& args);
    JSValue delete_collection_item(WebView* web_view, const JSArray& args);
    JSValue delete_mineral_item(WebView* web_view, const JSArray& args);
    void edit_collection_item(WebView* web_view, const JSArray& args);
    void edit_mineral_item(WebView* web_view, const JSArray& args);
    void cancel_item_edit(WebView* web_view, const JSArray& args);
    void save_collection_item(WebView* web_view, const JSArray& args);
    void save_mineral_item(WebView* web_view, const JSArray& args);
    void remove_table(WebView* web_view, const JSArray& args);
    void choose_new_table(WebView* web_view, const JSArray& args);
    void activate_mineral_item(WebView* web_view, const JSArray& args);
    void activate_collection_item(WebView* web_view, const JSArray& args);
    void arbitrary_js(WebView* web_view, const JSArray& args);

    std::string web_str(JSValue JSVal);
};

#endif  // STONES_PROGRAM_CLASSES_H_