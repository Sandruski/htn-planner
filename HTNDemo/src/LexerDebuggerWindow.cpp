
#include "LexerDebuggerWindow.h"

#include "imgui.h"
#include <filesystem>

void LexerDebuggerWindow::Render()
{
	static bool sLexerWindow = false;
	ImGui::Begin("HTN Lexer Debugger", &sLexerWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)




	// TODO JOSE: Print the list of domains in the combo box, so we can select the domain to parse.
	//ImGui::Combo("combo 3 (array)", &item_current_3, items, IM_ARRAYSIZE(items));




	if (ImGui::Button("Parse Domain"))
	{
		// This is not working ATM, needs fix, and we need to move this code up, the parse button is pressed and then the selected domain gets parsed.
		std::filesystem::path current_path = std::filesystem::current_path();
		const std::filesystem::path domains_sub_path{ "../Domains" };
		
		const std::filesystem::path domains_path = current_path.append(domains_sub_path.c_str());
		std::ranges::for_each(std::filesystem::directory_iterator{ domains_path },
			[](const auto& dir_entry) 
			{
				(void)dir_entry;
				//std::cout << dir_entry << '\n';
			});

		// Parse domain!
	}

	ImGui::End();
}
