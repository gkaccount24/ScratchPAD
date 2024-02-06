#include "./include/imgui_table.h"

void imgui_table::PopGlobalImGuiStylingRules()
{
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();

	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}

void imgui_table::PushGlobalImGuiStylingRules()
{
	ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(127 / 255.0f, 143 / 255.0f, 166 / 255.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(245 / 255.0f, 246 / 255.0f, 250 / 255.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImVec4(220 / 255.0f, 221 / 255.0f, 225 / 255.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(245 / 255.0f, 246 / 255.0f, 250 / 255.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(113 / 255.0f, 128 / 255.0f, 147 / 255.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(47 / 255.0f, 54 / 255.0f, 64 / 255.0f, 1.0f));

	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_TabBarBorderSize, 0.0f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
}

void imgui_table::PopImGuiTableHeaderStylingRules()
{
	ImGui::PopStyleColor();
}

void imgui_table::PushImGuiTableHeaderStylingRules()
{
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(245 / 255.0f, 246 / 255.0f, 250 / 255.0f, 1.0f));
}

void imgui_table::PopImGuiTableBodyStylingRules()
{
	ImGui::PopStyleColor();
}

void imgui_table::PushImGuiTableBodyStylingRules()
{
	// ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(47 / 255.0f, 54 / 255.0f, 64 / 255.0f, 1.0f));
}

void imgui_table::SetName(const string_view& TableName, bool Visible)
{
	Name = TableName;
	NameVisible = Visible;
}

void imgui_table::AddHeader(const string_view& Text, bool Active)
{
	if(HeaderCount < MAX_TABLE_HEADER_COUNT)
	{
		Headers[HeaderCount].Text = Text;
		Headers[HeaderCount].Active = Active;
		Headers[HeaderCount].Column = HeaderCount;

		HeaderCount++;
	}
}

void imgui_table::Draw()
{
	PushImGuiTableHeaderStylingRules();

	size_t HeaderCount = 0; //Table->ActiveHeaders.size();
	size_t RowCount = ModelCount;

	ImGuiTableFlags Flags = ImGuiTableFlags_Resizable |
		ImGuiTableFlags_Reorderable |
		ImGuiTableFlags_Sortable |
		ImGuiTableFlags_RowBg |
		ImGuiTableFlags_ContextMenuInBody;

	ImGui::SetNextWindowPos(ImVec2(Geometry.X, Geometry.Y));
	ImGui::SetNextWindowSize(ImVec2(Geometry.Width,
		Geometry.Height));

	if(ImGui::BeginTable(Name.c_str(), HeaderCount, Flags))
	{
		for(size_t ActiveHeaderIndex = 0; ActiveHeaderIndex < HeaderCount; ActiveHeaderIndex++)
		{
			// if(Table->ActiveHeaders[ActiveHeaderIndex])
			{
				// string& Text = Table->ActiveHeaders[ActiveHeaderIndex]->Text;

				// ImGuiTableColumnFlags Flags = 0; //ImGuiTableColumnFlags_IsHovered;

				// ImGui::TableSetupColumn(Text.c_str(), Flags);
			}
		}

		// done rendering imgui_table headers
		ImGui::TableHeadersRow();

		ImU32 ColorU32 = ImGui::GetColorU32(ImGuiCol_TableRowBgAlt);
		ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ColorU32);

		PushImGuiTableBodyStylingRules();

		for(size_t RowIndex = 0; RowIndex < ModelCount; RowIndex++)
		{
			ImGui::TableNextRow();

			// if(ImGui::IsAnyItemHovered() && ImGui::IsMouseReleased(1))
			// {
			//     string message = "item is hovered";
			//     cout << message << endl;
			// }

			for(size_t ActiveHeaderIndex = 0; ActiveHeaderIndex < HeaderCount; ActiveHeaderIndex++)
			{
				ImGui::TableNextColumn();

				//  bool HasIsHoveredFlag = (ImGui::TableGetColumnFlags(ActiveHeaderIndex) & 
				 //                          ImGuiTableColumnFlags_IsHovered) > 0;

				  // SetImGuiCellData(Table, ActiveHeaderIndex, RowIndex, Table->ModelType);
			}
		}

		PopImGuiTableBodyStylingRules();

		// close imgui_table
		ImGui::EndTable();
	}

	PopImGuiTableHeaderStylingRules();
}
