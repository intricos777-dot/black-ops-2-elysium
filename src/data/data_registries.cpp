#include "data_registries.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <cstdio>
#include <algorithm>

namespace bo2 {

bool WeaponRegistry::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;
    nlohmann::json j;
    try { f >> j; } catch (...) { return false; }
    for (const auto& w : j) {
        WeaponDef d;
        d.name = w.value("name", "");
        std::string cls = w.value("class", "assault");
        if (cls == "smg") d.wclass = WeaponClass::SMG;
        else if (cls == "lmg") d.wclass = WeaponClass::LMG;
        else if (cls == "sniper") d.wclass = WeaponClass::Sniper;
        else if (cls == "shotgun") d.wclass = WeaponClass::Shotgun;
        else if (cls == "pistol") d.wclass = WeaponClass::Pistol;
        else if (cls == "launcher") d.wclass = WeaponClass::Launcher;
        else d.wclass = WeaponClass::Assault;
        d.damage = w.value("damage", 30);
        d.fire_rate = w.value("fireRate", 8.0);
        d.magazine = w.value("magazine", 30);
        d.reserve = w.value("reserve", 90);
        d.range = w.value("range", 50.0);
        m_weapons.push_back(std::move(d));
    }
    return true;
}

const WeaponDef* WeaponRegistry::find(const std::string& name) const {
    for (const auto& w : m_weapons) if (w.name == name) return &w;
    return nullptr;
}

std::vector<const WeaponDef*> WeaponRegistry::by_class(WeaponClass c) const {
    std::vector<const WeaponDef*> out;
    for (const auto& w : m_weapons) if (w.wclass == c) out.push_back(&w);
    return out;
}

void WeaponRegistry::register_defaults() {
    m_weapons = {
        {"M16A1", WeaponClass::Assault, 35, 9.0f, 30, 120, 60.0f, ""},
        {"MP5K", WeaponClass::SMG, 28, 12.0f, 30, 150, 35.0f, ""},
        {"RPK", WeaponClass::LMG, 32, 7.0f, 100, 300, 70.0f, ""},
        {"Dragunov", WeaponClass::Sniper, 90, 1.5f, 10, 40, 120.0f, "scope"},
        {"Olympia", WeaponClass::Shotgun, 80, 2.0f, 8, 32, 15.0f, ""},
        {"M1911", WeaponClass::Pistol, 35, 5.0f, 8, 40, 30.0f, ""},
        {"RPG-7", WeaponClass::Launcher, 200, 0.5f, 1, 4, 100.0f, ""},
        {"Ray Gun", WeaponClass::Special, 100, 4.0f, 20, 200, 80.0f, ""},
    };
    std::printf("[Weapons] %zu registered\n", m_weapons.size());
}

bool PerkRegistry::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;
    nlohmann::json j;
    try { f >> j; } catch (...) { return false; }
    for (const auto& p : j) {
        PerkDef d;
        d.name = p.value("name", "");
        d.tier = p.value("tier", "passive");
        d.effect = p.value("effect", "");
        d.modifier = p.value("modifier", 1.0);
        m_perks.push_back(std::move(d));
    }
    return true;
}

void PerkRegistry::register_defaults() {
    m_perks = {
        {"Juggernog", "passive", "+50% HP", 1.5f},
        {"Speed Cola", "passive", "+50% reload speed", 1.5f},
        {"Double Tap", "passive", "+fire rate", 2.0f},
        {"Quick Revive", "passive", "self-revive (solo)", 1.0f},
        {"Stamin-Up", "passive", "+move speed", 1.3f},
        {"Mule Kick", "passive", "3rd weapon slot", 1.0f},
    };
    std::printf("[Perks] %zu registered\n", m_perks.size());
}

bool MapRegistry::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;
    nlohmann::json j;
    try { f >> j; } catch (...) { return false; }
    for (const auto& m : j) {
        MapDef d;
        d.name = m.value("name", "");
        d.mode = m.value("mode", "mp");
        d.max_players = m.value("maxPlayers", 6);
        m_maps.push_back(std::move(d));
    }
    return true;
}

void MapRegistry::register_defaults() {
    m_maps = {
        {"Nuketown", "mp", 6},
        {"Firing Range", "mp", 6},
        {"Summit", "mp", 6},
        {"Tranzit", "zm", 4},
        {"Town", "zm", 4},
        {"Bus Depot", "zm", 4},
    };
    std::printf("[Maps] %zu registered\n", m_maps.size());
}

bool DifficultyManager::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;
    nlohmann::json j;
    try { f >> j; } catch (...) { return false; }
    for (auto& [name, prof] : j.value("profiles", nlohmann::json::object()).items()) {
        Difficulty d;
        d.profile = name;
        d.player_health_mult = prof.value("player", nlohmann::json::object()).value("health", 100) / 100.0f;
        d.enemy_health_mult = prof.value("combat", nlohmann::json::object()).value("enemyHealthMultiplier", 1.0);
        d.enemy_damage_mult = prof.value("combat", nlohmann::json::object()).value("enemyDamageMultiplier", 1.0);
        d.enemy_accuracy = prof.value("combat", nlohmann::json::object()).value("enemyAccuracy", 0.7);
        d.zombie_speed_mult = prof.value("zombies", nlohmann::json::object()).value("speedMultiplier", 1.0);
        d.starting_points = prof.value("economy", nlohmann::json::object()).value("startingPoints", 500);
        m_profiles.push_back(std::move(d));
    }
    set_profile(j.value("default", "casual"));
    return !m_profiles.empty();
}

const Difficulty* DifficultyManager::get(const std::string& name) const {
    for (const auto& p : m_profiles) if (p.profile == name) return &p;
    return nullptr;
}

void DifficultyManager::set_profile(const std::string& name) {
    const Difficulty* d = get(name);
    if (d) m_current = *d;
    else if (!m_profiles.empty()) m_current = m_profiles[0];
}

} // namespace bo2
