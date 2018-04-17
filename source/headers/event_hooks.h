#ifndef APPLICATION_CLASSES_H_
#define APPLICATION_CLASSES_H_

#include <Awesomium/WebCore.h>
#include <Awesomium/STLHelpers.h>

using namespace Awesomium;

namespace Awesomium {
    class WebCore;
}

namespace Awesomium {
    class WebView;
}

// Common class that implements a windowed WebView, handles all input/display,
// and abstracts away all the platform-specific details.
class View {
    public:
        virtual ~View() {}
        // Platform-specific constructor
        static View* Create(int width, int height);
        // Get the associated WebView
        Awesomium::WebView* web_view() { return web_view_; }

    protected:
        View() { }
        Awesomium::WebView* web_view_;
};

// Common class that sets up an application, creates the WebCore, handles
// the Run loop, and abstracts platform-specific details.
class Application {
    public:
        // Listener interface to be used to handle various application events.
        class Listener {
            public:
                virtual ~Listener() {}
                // Event is fired when app (and WebCore) have been loaded.
                virtual void OnLoaded() = 0;
                // Event is fired for each iteration of the Run loop.
                virtual void OnUpdate() = 0;
                // Event is fired when the app is shutting down.
                virtual void OnShutdown() = 0;
        };

        virtual ~Application() {}
        // Platform-specific factory constructor
        static Application* Create();
        // Begin the Run loop.
        virtual void Run() = 0;
        // Ends the Run loop.
        virtual void Quit() = 0;
        // Create a platform-specific, windowed View
        virtual View* CreateView(int width, int height) = 0;
        // Destroy a View
        virtual void DestroyView(View* view) = 0;
        // Show a modal message box
        virtual void ShowMessage(const char* message) = 0;
        // Get the WebCore
        virtual Awesomium::WebCore* web_core() { return web_core_; }
        // Get the Listener.
        Listener* listener() { return listener_; }
        // Set the Listener for various app events.
        void set_listener(Listener* listener) { listener_ = listener; }
    protected:
        Application() { }
        virtual void Load() = 0;
        Listener* listener_;
        Awesomium::WebCore* web_core_;
};

#endif  // APPLICATION_CLASSES_H_
