#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <regex>
#include <spdlog/spdlog.h>
#include "spdlog/sinks/basic_file_sink.h"
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "MathProcessor.h"

auto static logger = spdlog::basic_logger_mt("basic_logger", "basic-log.txt");
auto static step_logger = spdlog::basic_logger_mt("step_logger", "step-log.txt");

// 3*2^(12-8)+5*(4+1) (testing)

int main() {
    using namespace ftxui;
    auto screen = ScreenInteractive::Fullscreen();

    std::string statement;
    float result;
    std::string result_string;
    std::stringstream result_stream;
    std::string warning_msg;

    bool reveal_answer = false;
    bool show_steps = true;
    bool show_warnings = false;
    bool valid_input = true;
    bool anything_entered = false;

    InputOption _input_statement;
    _input_statement.on_change = [&]{
        anything_entered = true;
        if(statement.size() < 2)
            return;
        try {
            result_stream.str(std::string());
            result = psv::nonRpnEvaluate(statement);
            result_stream << std::fixed << std::setprecision(1) << result;
            result_string = result_stream.str();
            valid_input = true;
            warning_msg.clear();
            spdlog::get("basic_logger")->info(result_string);
        } catch (std::exception& e) {
            valid_input = false;
            reveal_answer = false;
            warning_msg = e.what();
            spdlog::get("basic_logger")->error(e.what());
        }
    };
    _input_statement.on_enter = [&] {
        reveal_answer = valid_input;
    };

Component input_statement = Input(&statement, "Enter a Statement", _input_statement);

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
        warning_msg.clear();
    }, ButtonOption::Ascii());

    auto document_main = Container::Vertical({
                                                input_statement,
                                                button_evaluate | Maybe([&] { return !reveal_answer; }),
                                                button_reset | Maybe(&reveal_answer),
                                        });

    auto Steps = Renderer([&] {
        if(!show_steps || !reveal_answer) {
            return vbox({});
        }
        Elements step_children; // haha
        // Get diffs between steps
        for(int i = 0; i < psv::steps.size()-1; i++) {
            std::string current_step = psv::steps[i];
            std::string next_step = psv::steps[i+1];
            std::string before_diff;
            std::string diff;
            std::string after_diff;

            int dif_start = 0;
            int diff_end = 0;
            for(int j=0; j<next_step.size(); j++) {
                if(current_step[j] != next_step[j]) {
                    dif_start = j;
                    break;
                }
            }
            for(int j=1; j<next_step.size(); j++) {
                if(current_step[current_step.size()-j] != next_step[next_step.size()-j]) {
                    diff_end = current_step.size() - j;
                    break;
                }
            }
            before_diff = current_step.substr(0, dif_start);
            diff = current_step.substr(dif_start, diff_end - dif_start + 1);
            after_diff = current_step.substr(diff_end + 1, current_step.size() - 1);

    step_children.push_back(hbox({
                text(before_diff),
                text(diff) | underlined,
                text(after_diff),
            }) | hcenter);
        }
        return vbox({
            text("Steps:") | dim ,
            vbox(step_children),
        });
    });

    auto Math_Matters = Renderer(document_main, [&] {
        auto result_conditional = reveal_answer ? hbox({
            text("=") | bold,
            text(result_string) | bold,
        }) | vcenter : hbox({});

        auto warnings = show_warnings ? hbox({
            text(warning_msg) | color(Color::Red),
        }) | center : hbox({});


        return vbox({
            filler(),
            hbox({
                input_statement->Render() | flex_shrink | border,
            }) | vcenter | hcenter,
            Steps->Render() | hcenter,
            result_conditional | hcenter,
            warnings,
            hbox({
                reveal_answer ? button_reset->Render()
                : button_evaluate->Render() | (valid_input ? color(Color::Green) : color(Color::Red)),
            }) | vcenter | hcenter ,
            filler(),
        });
    });

    auto toggle_steps = Checkbox("Show Steps?", &show_steps);
    auto toggle_warnings = Checkbox("Show Warnings?", &show_warnings);

    auto document_settings = Container::Vertical({
        toggle_steps,
        toggle_warnings,
    });
    auto Settings_Help = Renderer(document_settings, [&] {
        return vbox({
            filler(),
            vbox({
                 text("Settings") | bold | hcenter,
                 toggle_steps->Render(),
                 toggle_warnings->Render(),
            }) | hcenter,
            filler(),
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
            text("by Peter V.")
        });
    });


    screen.Loop(main_renderer);

    return EXIT_SUCCESS;
}
