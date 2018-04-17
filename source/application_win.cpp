#include "headers/application.h"

#include <Awesomium/WebCore.h>

using namespace Awesomium;

class ApplicationWin : public Application {
    bool is_running_;
    public:
        ApplicationWin()
        {
            is_running_ = true;
            listener_ = NULL;
            web_core_ = NULL;
        }

        virtual ~ApplicationWin()
        {
            if (listener())
                listener()->OnShutdown();

            if (web_core_)
                web_core_->Shutdown();
        }

        virtual void Run()
        {
            Load();
            // Main message loop:
            MSG msg;
            while (GetMessage(&msg, NULL, 0, 0) && is_running_)
            {
                web_core_->Update();
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                if (listener())
                    listener()->OnUpdate();
            }
        }
  
        virtual void Quit()
        {
            is_running_ = false;
        }

        virtual void Load()
        {
            web_core_ = WebCore::Initialize(WebConfig());
            if (listener())
                listener()->OnLoaded();
        }

        virtual View* CreateView(int width, int height)
        {
            return View::Create(width, height);
        }

        virtual void DestroyView(View* view)
        {
            delete view;
        }

        virtual void ShowMessage(const char* message)
        {
            MessageBox(0, message, message, NULL);
        }
};

Application* Application::Create()
{
    return new ApplicationWin();
}