#ifndef UI_MANIFEST_H
#define UI_MANIFEST_H

#include <SDL.h>

#include "./imgui/imgui.h"
#include "./imgui/backends/imgui_impl_sdl2.h"
#include "./imgui/backends/imgui_impl_sdlrenderer2.h"

#include <memory>
#include <unordered_map>
#include <vector>
#include <string_view>
#include <string>

using std::shared_ptr;
using std::make_shared;
using std::unordered_map;
using std::vector;
using std::string_view;
using std::string;

#define IMGUI_DEFAULT_TABLE_FLAGS ImGuiTableFlags_Resizable   |\
								  ImGuiTableFlags_Reorderable |\
								  ImGuiTableFlags_Sortable    |\
								  ImGuiTableFlags_RowBg       |\
								  ImGuiTableFlags_ContextMenuInBody

#define IMGUI_DEFAULT_TABLE_COLUMN_FLAGS ImGuiTableColumnFlags_IndentEnable
#define IMGUI_DEFAULT_TABLE_HEADER_FLAGS ImGuiTableRowFlags_Headers

class ui_manifest
{
	struct													   ui_element;
	typedef unordered_map<string_view, shared_ptr<ui_element>> uie_assoc_container;
	typedef vector<shared_ptr<ui_element>>					   uie_list_container;

	struct ui_element 
	{
		string_view		    Name;
		string			    DisplayName;
		int				    ImGuiAppFlags;
		uie_assoc_container Children;
	};

public:
	// Begin();

public:
	void AddWindow(const string_view& Name, ImGuiWindowFlags Flags);
	void AddButton();

private:
	shared_ptr<ui_element> Root;
	uie_list_container     Elements;

};

#endif
