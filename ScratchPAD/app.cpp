#include "./include/app.h"

void app::BuildMainMenubar()
{
	MainMenubar.AddMenu("File", true);
    MainMenubar.AddMenu("Edit", true);
    MainMenubar.AddMenu("Selection", true);
    MainMenubar.AddMenu("View", true);
    MainMenubar.AddMenu("Help", true);

}

void app::BuildDirListingTable()
{
    DirListingTable.SetName("DirectoryListingTable", false);

    DirListingTable.AddHeader("#", true);
    DirListingTable.AddHeader("Name", true);
    DirListingTable.AddHeader("Last Write Time", true);
    DirListingTable.AddHeader("Type", true);
    DirListingTable.AddHeader("Size", true);
    DirListingTable.AddHeader("Created", true);
    DirListingTable.AddHeader("Last Accessed", true);

}

void app::BuildUIManifest(const string_view& FilePath)
{
    // UIManifest.AddWindow("##MainContentWindow", );
}

void app::Cleanup()
{
    Graphics.Cleanup();
}

bool app::Init(const string_view& Title, const geometry& WindowGeometry)
{
    if(!Graphics.Init(Title, WindowGeometry))
    {

        return false;
    }

    Exited = false;
    FailCode = 0;

    return true;
}

bool app::Init(const string_view& Title)
{	
    geometry WindowGeometry { };

	WindowGeometry.X = APP_DEFAULT_WINDOW_XPOS;
	WindowGeometry.Y = APP_DEFAULT_WINDOW_YPOS;
	WindowGeometry.Width  = APP_DEFAULT_WINDOW_WIDTH;
	WindowGeometry.Height = APP_DEFAULT_WINDOW_HEIGHT;

    return Init(Title, WindowGeometry);
}

int app::Run()
{
    BuildUIManifest("/path/to/manifest/file");
    // BuildMainMenubar();
    // BuildDirListingTable();

    // DATA SECTION
    // string WorkingDirPath = "C:\\Users\\Nathan";
    // vector<file*> Files;

    while(!Exited)
    {
        SDL_Event Event{ };

        while(SDL_PollEvent(&Event))
        {
            ImGui_ImplSDL2_ProcessEvent(&Event);

            // handle quit event
            if(Event.type == SDL_QUIT)
            {
                Exited = true;
            }

            Graphics.StartNewFrame();
            Graphics.Draw(&UIManifest);
            Graphics.SwapBuffers();
        }
    }

    Cleanup();

    return 0;
}
