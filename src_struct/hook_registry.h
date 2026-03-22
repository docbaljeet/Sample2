#pragma once
#include <pybind11/pytypes.h>
#include <array>
#include <unordered_map>
#include <string>
#include <cstddef>
#include <string_view>
#include <stdexcept>

namespace py = pybind11;

// ── 1. Add new hooks here — this is the ONLY place ─────────────
enum class Hook : std::size_t {
    Mortality,
    Lapse,
    EiaCredit,
    // ← new hooks go here
    Count
};

// ── 2. Metadata table — maps enum → Python name ────────────────
struct HookMeta {
    Hook             id;
    std::string_view name;   // Python-visible name
};

inline constexpr std::array<HookMeta, static_cast<std::size_t>(Hook::Count)> HOOK_TABLE {{
    { Hook::Mortality,  "mortality"  },
    { Hook::Lapse,      "lapse"      },
    { Hook::EiaCredit,  "eia_credit" },
}};

// ── 3. Runtime storage — hook → name → callable ────────────────
class HookRegistry {
public:
    void add(Hook h, const std::string& name, py::object fn) {
        slots_[static_cast<std::size_t>(h)][name] = std::move(fn);
    }

    template <typename Ctx>
    void call(Hook h, const std::string& name, Ctx& ctx) const {
        const auto& map = slots_[static_cast<std::size_t>(h)];
        auto it = map.find(name);
        if (it == map.end())
            throw std::runtime_error(
                "No function registered for hook '" + name + "'");
        it->second(py::cast(ctx, py::return_value_policy::reference));
    }

private:
    std::array<std::unordered_map<std::string, py::object>,
               static_cast<std::size_t>(Hook::Count)> slots_;
};
