#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>

#include "input.hpp"

#include <spdlog/spdlog.h>
#include "spdlog/sinks/basic_file_sink.h"

#include "ftxui/dom/elements.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"

auto static logger = spdlog::basic_logger_mt("basic_logger", "basic-log.txt");
auto static step_logger = spdlog::basic_logger_mt("step_logger", "step-log.txt");


int main() {
    using namespace ftxui;
    auto screen = ScreenInteractive::Fullscreen();

    std::string statement;
    float result;
    std::string result_string;
    std::stringstream result_stream;

    bool reveal_answer = false;
    bool show_steps = false;
    bool valid_input = true;
    bool anything_entered = false;

    InputOption handle_change;
    handle_change.on_change = [&]{
        anything_entered = true;
        try {
            result_stream.str(std::string());
            result = psv::nonRpnEvaluate(statement);
            result_stream << std::fixed << std::setprecision(1) << result;
            result_string = result_stream.str();
            valid_input = true;
            spdlog::get("basic_logger")->info(result_string);
        } catch (std::exception& e) {
            valid_input = false;
            reveal_answer = false;
            spdlog::get("basic_logger")->error(e.what());
        }
    };
    handle_change.on_enter = [&] {
        reveal_answer = valid_input;
    };

Component input_statement = Input(&statement, "Enter a Statement", handle_change);

    auto button_evaluate = Button("Evaluate", [&] {
        reveal_answer = valid_input;
        spdlog::get("step_logger")->info("Statement: " + statement);
        for(auto const& step : psv::steps) {
            spdlog::get("step_logger")->info(step);
        }
        spdlog::get("step_logger")->info("\n");
    }, ButtonOption::Ascii());

    auto button_reset = Button("Reset", [&] {
        statement = "";
        result = 0;
        result_string = "";
        reveal_answer = false;
        valid_input = true;
        anything_entered = false;
        psv::steps.clear();
    }, ButtonOption::Ascii());

    auto document_mm = Container::Vertical({
                                                input_statement,
                                                button_evaluate | Maybe([&] { return !reveal_answer; }),
                                                button_reset | Maybe(&reveal_answer),
                                        });

    auto Steps = Renderer([&] {
        if(!show_steps || !reveal_answer) {
            return vbox({});
        }
        Elements step_children; // haha
        for(auto const& step : psv::steps) {
            step_children.push_back(text(step));
        }
        return vbox({
            text("Steps:") | dim ,
            vbox(step_children),
        });
    });

    auto Math_Matters = Renderer(document_mm, [&] {
        auto result_conditional = reveal_answer ? hbox({
            text("=") | bold,
            text(result_string) | bold,
        }) | vcenter : hbox({});


        return vbox({
            filler(),
            hbox({
                input_statement->Render() | flex_shrink | border,
            }) | vcenter | hcenter,
            Steps->Render() | hcenter,
            result_conditional | hcenter,
            hbox({
                reveal_answer ? button_reset->Render()
                : button_evaluate->Render() | (valid_input ? color(Color::Green) : color(Color::Red)),
            }) | vcenter | hcenter ,
            filler(),
        });
    });

    auto toggle_steps = Checkbox("Show Steps?", &show_steps);

    auto document_settings = Container::Vertical({
        toggle_steps,
    });
    auto Settings_Help = Renderer(document_settings, [&] {
        return vbox({
            text("Settings") | hcenter,
            toggle_steps->Render() | center,
            text("by Peter V.")
        });
    });

    int tab_index = 0;
    std::vector<std::string> tabs = {"Statement Solver", "Settings"};
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
            text("Math-Matters") | bold,
            tab_selection->Render(),
            tab_content->Render() | flex,
        });
    });


    screen.Loop(main_renderer);

    return EXIT_SUCCESS;
}
