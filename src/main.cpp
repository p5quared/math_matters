#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include "input.hpp"

#include "ftxui/dom/elements.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"

int main() {
    using namespace ftxui;
    auto screen = ScreenInteractive::Fullscreen();

    std::string statement;
    float result;
    std::string result_string;
    std::stringstream result_stream;
    bool evaluated = false;
Component input_statement = Input(&statement, "Enter a Statement");

    auto button_evaluate = Button("Evaluate", [&] {
        try {
            result_stream.str(std::string());
            result = psv::nonRpnEvaluate(statement);
            evaluated = true;
            result_stream << std::fixed << std::setprecision(1) << result;
            result_string = result_stream.str();
        } catch (std::exception& e) {

        }
    }, ButtonOption::Ascii());

    auto document_mm = Container::Vertical({
                                                input_statement,
                                                button_evaluate,
                                        });

    auto Math_Matters = Renderer(document_mm, [&] {
        auto result_conditional = evaluated ? hbox({
            text("=") | bold,
            text(result_string) | bold,
        }) | vcenter : hbox({});


        return vbox({
            filler(),
            hbox({
                input_statement->Render() | flex_shrink | border,
            }) | vcenter | hcenter,
            result_conditional | hcenter,
            hbox({
                button_evaluate->Render(),
            }) | vcenter | hcenter ,
            filler(),
        });
    });


    auto Settings_Help = Renderer([&] {
        return vbox({
            text("Settings/Help") | hcenter,
        });
    });

    int tab_index = 0;
    std::vector<std::string> tabs = {"Statement Solver", "Settings/Help"};
    auto tab_selection = Menu(&tabs, &tab_index, MenuOption::HorizontalAnimated());
    auto tab_content = Container::Tab({
        Math_Matters,
        Settings_Help,
    },
    &tab_index);

    auto document = Container::Vertical({
        tab_selection,
        tab_content,
    });

    auto main_renderer = Renderer(document, [&] {
        return vbox({
            text("Math-Matters") | bold | hcenter,
            tab_selection->Render(),
            tab_content->Render() | flex,
        });
    });


    screen.Loop(main_renderer);

    return EXIT_SUCCESS;
}
