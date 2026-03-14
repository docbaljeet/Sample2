#include <pybind11/embed.h>
#include <iostream>
#include "projection_engine.h"

namespace py = pybind11;

int main() {
    py::scoped_interpreter guard{};

    py::object user_module;
    try {
        py::module_::import("sys").attr("path")
            .cast<py::list>().append(".");

        user_module = py::module_::import("user_script");
        std::cout << "Loaded user_script.py\n";
    } catch (const py::error_already_set& e) {
        std::cerr << "Failed to load user_script.py: " << e.what() << "\n";
        return 1;
    }

    py::object py_hook;
    if (py::hasattr(user_module, "on_month")) {
        py_hook = user_module.attr("on_month");
        std::cout << "Found on_month hook in user script\n";
    } else {
        std::cout << "No on_month hook found — running without user hook\n";
    }

    ProjectionEngine engine;

    engine.add_policy({1, 100000.0, 500.0, 35, 20});
    engine.add_policy({2, 250000.0, 1200.0, 45, 30});

    engine.add_scenario({1, 0.04});
    engine.add_scenario({2, 0.06});

    int projection_months = 12;

    std::cout << "\n=== Running projection — thread_local approach ("
              << projection_months << " months) ===\n\n";

    ProjectionHook hook = nullptr;
    if (py_hook) {
        hook = [&]() { py_hook(); };
    }

    engine.run(projection_months, hook);

    std::cout << "\n=== Projection complete ===\n";
    return 0;
}
