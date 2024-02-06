#ifndef IMGUI_TABLE_H
#define IMGUI_TABLE_H

#include "defs.h"

#include <SDL.h>

#include "./imgui/imgui.h"
#include "./imgui/backends/imgui_impl_sdl2.h"
#include "./imgui/backends/imgui_impl_sdlrenderer2.h"

// ImGuiTableFlags Flags = ImGuiTableFlags_Resizable |
// ImGuiTableFlags_Reorderable |
// ImGuiTableFlags_Sortable |
// ImGuiTableFlags_RowBg |
// ImGuiTableFlags_ContextMenuInBody;

#include <string_view>
#include <string>

using std::string_view;
using std::string;

#define MAX_TABLE_HEADER_COUNT 16

enum class imgui_table_model_type
{
	File = 0,
};

class imgui_table
{
	enum class imgui_table_header_property
	{
		Column = 0,
		Text = 1,
		Active = 2,
	};

	struct imgui_table_header
	{
		size_t Column;
		string Text;
		bool Active;
	};

public:
	 imgui_table() = default;
	~imgui_table() = default;

public:
	void SetName(const string_view& TableName, bool Visible);
	void AddHeader(const string_view& Text, bool Active);
	
public:
	void Draw();

private:
	void PopGlobalImGuiStylingRules();
	void PushGlobalImGuiStylingRules();
	void PopImGuiTableHeaderStylingRules();
	void PushImGuiTableHeaderStylingRules();
	void PopImGuiTableBodyStylingRules();
	void PushImGuiTableBodyStylingRules();

private:
	string				   Name;
	bool				   NameVisible;

	imgui_table_header	   Headers[MAX_TABLE_HEADER_COUNT];
	int					   HeaderCount;

	imgui_table_model_type ModelType;
	size_t				   ModelCount;

	geometry			   Geometry;
};

#endif
