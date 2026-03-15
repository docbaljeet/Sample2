#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include "script_registry.h"
#include "runtime_context.h"

namespace py = pybind11;

// ═══════════════════════════════════════════════════════════════════════
// Auto-generated bindings.
//
// Instead of hand-writing get_issue_age(), get_gender(), set_qx_monthly()
// for every variable, we loop over the registry and register them all.
// The Python API is identical: mortality.get_issue_age() still works.
// Adding a new variable = one line in script_registry.h. No changes here.
// ═══════════════════════════════════════════════════════════════════════

PYBIND11_EMBEDDED_MODULE(actuarial, m) {
    m.doc() = "Actuarial projection library — data-driven bindings";

    for (const auto& script : get_script_registry()) {
        auto sub = m.def_submodule(script.name.c_str());

        for (const auto& var : script.vars) {
            // Capture by value so each lambda has its own copy
            std::string sname = script.name;
            std::string vname = var.name;

            if (var.direction == VarDef::INPUT) {
                // ── READ-ONLY inputs: register get_<name>() ──────
                switch (var.type) {
                case VarDef::INT:
                    sub.def(("get_" + vname).c_str(),
                        [sname, vname]() {
                            return active_context(sname).ints.at(vname);
                        });
                    break;

                case VarDef::DOUBLE:
                    sub.def(("get_" + vname).c_str(),
                        [sname, vname]() {
                            return active_context(sname).doubles.at(vname);
                        });
                    break;

                case VarDef::VECTOR_DOUBLE:
                    sub.def(("get_" + vname).c_str(),
                        [sname, vname]() -> const std::vector<double>& {
                            return active_context(sname).vectors.at(vname);
                        },
                        py::return_value_policy::reference);
                    break;
                }
            } else {
                // ── WRITE-ONLY outputs: register set_<name>() ────
                switch (var.type) {
                case VarDef::INT:
                    sub.def(("set_" + vname).c_str(),
                        [sname, vname](int v) {
                            active_context(sname).ints[vname] = v;
                        },
                        py::arg("value"));
                    break;

                case VarDef::DOUBLE:
                    sub.def(("set_" + vname).c_str(),
                        [sname, vname](double v) {
                            active_context(sname).doubles[vname] = v;
                        },
                        py::arg("value"));
                    break;

                case VarDef::VECTOR_DOUBLE:
                    sub.def(("set_" + vname).c_str(),
                        [sname, vname](std::vector<double> v) {
                            active_context(sname).vectors[vname] = std::move(v);
                        },
                        py::arg("rates"));
                    break;
                }
            }
        }
    }
}
