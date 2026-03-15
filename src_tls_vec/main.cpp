#include <pybind11/embed.h>
#include <iostream>
#include <cmath>
#include "projection_engine.h"

namespace py = pybind11;

int main() {
    py::scoped_interpreter guard{};

    py::module_::import("sys").attr("path")
        .cast<py::list>().append(".");

    // ── Load each script ────────────────────────────────────────────
    ScriptHooks hooks;

    auto load_hook = [](const char* module_name,
                        const char* func_name) -> py::object {
        try {
            auto mod = py::module_::import(module_name);
            if (py::hasattr(mod, func_name)) {
                std::cout << "Loaded " << module_name << "."
                          << func_name << "()\n";
                return mod.attr(func_name);
            }
            std::cout << "No " << func_name << "() in "
                      << module_name << "\n";
        } catch (const py::error_already_set& e) {
            std::cerr << "Failed to load " << module_name
                      << ": " << e.what() << "\n";
        }
        return py::none();
    };

    auto py_mortality  = load_hook("mortality_script_vec",  "calc");
    auto py_lapse      = load_hook("lapse_script_vec",      "calc");
    auto py_eia_credit = load_hook("eia_credit_script_vec", "calc");

    if (!py_mortality.is_none())
        hooks.mortality  = [&]() { py_mortality(); };
    if (!py_lapse.is_none())
        hooks.lapse      = [&]() { py_lapse(); };
    if (!py_eia_credit.is_none())
        hooks.eia_credit = [&]() { py_eia_credit(); };

    // ── Set up model points ─────────────────────────────────────────
    ProjectionEngine engine;

    engine.add_policy({1, 100000.0, 500.0,  35, 20, 0, 0});
    engine.add_policy({2, 250000.0, 1200.0, 45, 30, 1, 0});

    // Generate fake monthly index returns for 12 months
    int projection_months = 12;
    std::vector<double> idx_returns(projection_months);
    for (int i = 0; i < projection_months; ++i)
        idx_returns[i] = 0.005 + 0.003 * std::sin(i * 0.5);

    engine.add_scenario({1, 0.80, 0.10, 0.00, 0.02, idx_returns});
    engine.add_scenario({2, 0.90, 0.12, 0.01, 0.015, idx_returns});

    // ── Run projection ──────────────────────────────────────────────
    std::cout << "\n=== Running projection — vectorized scripts ("
              << projection_months << " months) ===\n\n";

    engine.run(projection_months, hooks);

    std::cout << "\n=== Projection complete ===\n";
    return 0;
}
