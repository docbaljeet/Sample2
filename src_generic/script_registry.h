#pragma once
#include <string>
#include <vector>

// ═══════════════════════════════════════════════════════════════════════
// Single source of truth for every variable in the model.
// Adding a new variable = adding ONE line here.
// The bindings, context wiring, and validation all flow from this table.
// ═══════════════════════════════════════════════════════════════════════

struct VarDef {
    enum Type      { INT, DOUBLE, VECTOR_DOUBLE };
    enum Direction { INPUT, OUTPUT };

    std::string name;
    Type        type;
    Direction   direction;
};

struct ScriptDef {
    std::string           name;
    std::vector<VarDef>   vars;
};

inline const std::vector<ScriptDef>& get_script_registry() {
    static const std::vector<ScriptDef> registry = {

        // ── mortality ──────────────────────────────────────────────
        {"mortality", {
            {"projection_months", VarDef::INT,           VarDef::INPUT},
            {"issue_age",         VarDef::INT,           VarDef::INPUT},
            {"gender",            VarDef::INT,           VarDef::INPUT},
            {"smoker_status",     VarDef::INT,           VarDef::INPUT},
            {"face_amount",       VarDef::DOUBLE,        VarDef::INPUT},
            {"qx_monthly",        VarDef::VECTOR_DOUBLE, VarDef::OUTPUT},
        }},

        // ── lapse ──────────────────────────────────────────────────
        {"lapse", {
            {"projection_months",  VarDef::INT,           VarDef::INPUT},
            {"policy_term_years",  VarDef::INT,           VarDef::INPUT},
            {"premium",            VarDef::DOUBLE,        VarDef::INPUT},
            {"lapse_rate_monthly", VarDef::VECTOR_DOUBLE, VarDef::OUTPUT},
        }},

        // ── eia_credit ─────────────────────────────────────────────
        {"eia_credit", {
            {"projection_months",  VarDef::INT,           VarDef::INPUT},
            {"participation_rate", VarDef::DOUBLE,        VarDef::INPUT},
            {"cap_rate",           VarDef::DOUBLE,        VarDef::INPUT},
            {"floor_rate",         VarDef::DOUBLE,        VarDef::INPUT},
            {"spread",             VarDef::DOUBLE,        VarDef::INPUT},
            {"index_returns",      VarDef::VECTOR_DOUBLE, VarDef::INPUT},
            {"credited_rate",      VarDef::VECTOR_DOUBLE, VarDef::OUTPUT},
        }},
    };
    return registry;
}
