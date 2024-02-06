#include "./include/gfx.h"

bool gfx::InitSDL(int Flags)
{
    bool Success = true;

    if(SDL_Init(Flags) != 0)
    {
        Success = false;

        // report, and log error
    }

    return Success;
}

void gfx::SwapBuffers()
{
    ImGuiIO& IO = ImGui::GetIO();

    ImGui::Render();

    SDL_RenderSetScale(RenderHandle, IO.DisplayFramebufferScale.x, IO.DisplayFramebufferScale.y);

    SDL_SetRenderDrawColor(RenderHandle, 245, 246, 250, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(RenderHandle);

    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(RenderHandle);
}

bool gfx::CreateAppWindow(const string_view& Title, const geometry& Geometry, int GfxSDLWindowMask)
{
    SDL_Window* TempWindowHandle = nullptr;

    TempWindowHandle = SDL_CreateWindow(Title.data(), 
                                        Geometry.X, 
                                        Geometry.Y, 
                                        Geometry.Width, 
                                        Geometry.Height, 
                                        GfxSDLWindowMask);
    if(!TempWindowHandle)
    {
        // log & report error
        return false;
    }

    WindowHandle = TempWindowHandle;

    WindowGeometry.X = Geometry.X;
    WindowGeometry.Y = Geometry.Y;

    WindowGeometry.Width = Geometry.Width;
    WindowGeometry.Height = Geometry.Height;

    SDLWindowMask = GfxSDLWindowMask;
    WindowTitle = Title;

	return true;
}

bool gfx::CreateAppWindow(const string_view& Title, const geometry& Geometry)
{
    return CreateAppWindow(Title, Geometry, GFX_DEFAULT_SDL_WINDOW_FLAGS);
}

bool gfx::CreateAppRenderer(int RenderFlags)
{
    SDL_Renderer* TempRenderHandle = nullptr;

    TempRenderHandle = SDL_CreateRenderer(WindowHandle, -1, RenderFlags);

    if(!TempRenderHandle)
    {
        // log & report error
        return false;
    }

    RenderHandle = TempRenderHandle;
    RenderFlags = RenderFlags;

    return true;
}

bool gfx::CreateAppRenderer()
{
    return CreateAppRenderer(GFX_DEFAULT_SDL_RENDERER_FLAGS);
}

void gfx::Cleanup()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(RenderHandle);
    SDL_DestroyWindow(WindowHandle);

    SDL_Quit();
}

bool gfx::Init(const string_view& Title, const geometry& Geometry, int GfxSDLInitFlags)
{
    if(!InitSDL(GfxSDLInitFlags))
    {

        return false;
    }

    if(!CreateAppWindow(WindowTitle, Geometry))
    {

        return false;
    }

    if(!CreateAppRenderer())
    {

        return false;
    }

    InitImGui();

    return true;
}

bool gfx::Init(const string_view& Title, const geometry& Geometry)
{
    return Init(Title, Geometry, GFX_DEFAULT_SDL_INIT_FLAGS);
}

void gfx::EnableImGuiIOKeyboardNav(ImGuiIO& IO)
{
    IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
}

void gfx::InitImGui()
{
    // init imgui context
    ImGui::CreateContext();

    // grab io ref for configuring flags
    ImGuiIO& IO = ImGui::GetIO();

    // configure flags
    EnableImGuiIOKeyboardNav(IO);

    ImGui::StyleColorsLight();

    // imgui sdl initialization
    ImGui_ImplSDL2_InitForSDLRenderer(WindowHandle, RenderHandle);
    ImGui_ImplSDLRenderer2_Init(RenderHandle);
}

void gfx::StartNewFrame()
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

//void gfx::DrawMainMenuBar(imgui_main_menubar* MainMenuBar)
//{
//    if(ImGui::BeginMainMenuBar())
//    {
//        MainMenuBar->Height = ImGui::GetWindowSize().y;
//
//        for (size_t Index = 0;
//            Index < MainMenuBar->MenuCount;
//            Index++)
//        {
//            imgui_menu* Menu = &MainMenuBar->Menus[Index];
//
//            const char* Text = Menu->Name.c_str();
//
//            if(ImGui::BeginMenu(Text))
//            {
//                for (size_t SubIndex = 0;
//                    SubIndex < Menu->MenuItemCount;
//                    SubIndex++)
//                {
//                    string& Text = Menu->MenuItems[SubIndex].Text;
//
//                    if(!Text.empty())
//                    {
//                        if(!ImGui::MenuItem(Text.c_str()))
//                        {
//                            //
//                        }
//                    }
//                }
//
//                ImGui::EndMenu();
//            }
//        }
//
//        ImGui::EndMainMenuBar();
//    }
//}

//void gfx::DrawTable(imgui_table* Table, geometry& Geometry)
//{
//    PushImGuiTableHeaderStylingRules();
//
//    size_t HeaderCount = Table->ActiveHeaders.size();
//    size_t RowCount = Table->ModelCount;
//
//    ImGuiTableFlags Flags = ImGuiTableFlags_Resizable |
//        ImGuiTableFlags_Reorderable |
//        ImGuiTableFlags_Sortable |
//        ImGuiTableFlags_RowBg |
//        ImGuiTableFlags_ContextMenuInBody;
//
//    ImGui::SetNextWindowPos(ImVec2(Geometry.X, Geometry.Y));
//    ImGui::SetNextWindowSize(ImVec2(Geometry.Width,
//        Geometry.Height));
//
//    if(ImGui::BeginTable(Table->ID.c_str(), HeaderCount, Flags))
//    {
//        for (size_t ActiveHeaderIndex = 0; ActiveHeaderIndex < HeaderCount; ActiveHeaderIndex++)
//        {
//            if(Table->ActiveHeaders[ActiveHeaderIndex])
//            {
//                string& Text = Table->ActiveHeaders[ActiveHeaderIndex]->Text;
//
//                ImGuiTableColumnFlags Flags = 0; //ImGuiTableColumnFlags_IsHovered;
//
//                ImGui::TableSetupColumn(Text.c_str(), Flags);
//            }
//        }
//
//        // done rendering imgui_table headers
//        ImGui::TableHeadersRow();
//
//        ImU32 ColorU32 = ImGui::GetColorU32(ImGuiCol_TableRowBgAlt);
//        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ColorU32);
//
//        PushImGuiTableBodyStylingRules();
//
//        for (size_t RowIndex = 0; RowIndex < Table->ModelCount; RowIndex++)
//        {
//            ImGui::TableNextRow();
//
//            // if(ImGui::IsAnyItemHovered() && ImGui::IsMouseReleased(1))
//            // {
//            //     string message = "item is hovered";
//            //     cout << message << endl;
//            // }
//
//            for (size_t ActiveHeaderIndex = 0; ActiveHeaderIndex < HeaderCount; ActiveHeaderIndex++)
//            {
//                ImGui::TableNextColumn();
//
//                //  bool gfx::HasIsHoveredFlag = (ImGui::TableGetColumnFlags(ActiveHeaderIndex) & 
//                 //                          ImGuiTableColumnFlags_IsHovered) > 0;
//
//                  // SetImGuiCellData(Table, ActiveHeaderIndex, RowIndex, Table->ModelType);
//            }
//        }
//
//        PopImGuiTableBodyStylingRules();
//
//        // close imgui_table
//        ImGui::EndTable();
//    }
//
//    PopImGuiTableHeaderStylingRules();
//}

void gfx::DrawSplitterWindow()
{
    if(ImGui::Begin(WindowTitle.data(), 0, ImGuiWindowMask))
    {


        ImGui::End();
    }
}

void gfx::Draw(ui_manifest* UI)
{
    int MenuBarHeight = 20;

    SDL_GetWindowSize(WindowHandle,
        &WindowGeometry.Width,
        &WindowGeometry.Height);

    ImGui::SetNextWindowPos(ImVec2(0, MenuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(WindowGeometry.Width, 
        WindowGeometry.Height));

    // UI->FirstNode()
}