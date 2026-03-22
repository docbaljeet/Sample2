#include <pybind11/embed.h>
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#ifdef _WIN32
#  include <cstdlib>
#endif
#include "projection_engine.h"

namespace py = pybind11;

int main() {
    // Set PYTHONHOME so the embedded interpreter can locate the stdlib.
    // PYTHON_HOME_PATH is injected by CMake from Python3_EXECUTABLE's directory.
#if defined(_WIN32) && defined(PYTHON_HOME_PATH)
    _putenv_s("PYTHONHOME", PYTHON_HOME_PATH);
#endif

    py::scoped_interpreter guard{};

    py::module_::import("sys").attr("path")
        .cast<py::list>().append(".");

    // Import actuarial module — registers context types + hook decorators
    auto actuarial = py::module_::import("actuarial");

    // ── Load client script name from JSON config ────────────────────
    const char* config_path = "hooks_config.json";
    std::ifstream ifs(config_path);
    if (!ifs) {
        std::cerr << "Cannot open " << config_path << "\n";
        return 1;
    }
    std::ostringstream buf;
    buf << ifs.rdbuf();

    auto json_mod  = py::module_::import("json");
    py::dict config = json_mod.attr("loads")(buf.str());
    std::string script = config["script"].cast<std::string>();

    // ── Import client script — decorators self-register ─────────────
    try {
        py::module_::import(script.c_str());
        std::cout << "Loaded script: " << script << "\n";
    } catch (const py::error_already_set& e) {
        std::cerr << "Failed to load " << script << ": " << e.what() << "\n";
        return 1;
    }

    // ── Load all named functions into registry ─────────────────────
    HookRegistry hooks;
    for (const auto& meta : HOOK_TABLE) {
        auto decorator = actuarial.attr(meta.name.data());
        py::dict funcs = decorator.attr("funcs");
        for (auto& [key, fn] : funcs) {
            std::string name = key.cast<std::string>();
            hooks.add(meta.id, name, fn.cast<py::object>());
            std::cout << "  " << meta.name << " -> " << name << "\n";
        }
    }

    // ── Set up model points ─────────────────────────────────────────
    ProjectionEngine engine;

    engine.add_policy({1, 100000.0, 500.0,  35, 20, 0, 0, "Table1", "Standard", "ScenarioA"});
    engine.add_policy({2, 250000.0, 1200.0, 45, 30, 1, 0, "Table1", "Standard", "ScenarioA"});

    int projection_months = 12;
    std::vector<double> idx_returns(projection_months);
    for (int i = 0; i < projection_months; ++i)
        idx_returns[i] = 0.005 + 0.003 * std::sin(i * 0.5);

    engine.add_scenario({1, 0.80, 0.10, 0.00, 0.02, idx_returns});
    engine.add_scenario({2, 0.90, 0.12, 0.01, 0.015, idx_returns});

    // ── Run projection ──────────────────────────────────────────────
    std::cout << "\n=== Running projection — struct passing ("
              << projection_months << " months) ===\n\n";

    engine.run(projection_months, hooks);

    std::cout << "\n=== Projection complete ===\n";
    return 0;
}
