#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <set>

namespace bo2 {

// ---- Weapons -------------------------------------------------------------

enum class WeaponClass : uint32_t { Assault, SMG, LMG, Sniper, Shotgun, Pistol, Launcher, Special };

struct WeaponDef {
    std::string name;
    WeaponClass wclass;
    uint32_t damage = 30;
    float fire_rate = 8.0f;
    uint32_t magazine = 30;
    uint32_t reserve = 90;
    float range = 50.0f;
    std::string attach;  // attachment name, e.g. "rapid_fire", "scope"
};

class WeaponRegistry {
public:
    bool load(const std::string& path);
    const std::vector<WeaponDef>& weapons() const { return m_weapons; }
    const WeaponDef* find(const std::string& name) const;
    std::vector<const WeaponDef*> by_class(WeaponClass c) const;
    void register_defaults();
private:
    std::vector<WeaponDef> m_weapons;
};

// ---- Perks ----------------------------------------------------------------

struct PerkDef {
    std::string name;
    std::string tier;     // "tactical", "lethal", "passive"
    std::string effect;
    float modifier = 1.0f;
};

class PerkRegistry {
public:
    bool load(const std::string& path);
    const std::vector<PerkDef>& perks() const { return m_perks; }
    void register_defaults();
private:
    std::vector<PerkDef> m_perks;
};

// ---- Maps -----------------------------------------------------------------

struct MapDef {
    std::string name;
    std::string mode;     // "mp", "zm", "campaign"
    uint32_t max_players = 6;
};

class MapRegistry {
public:
    bool load(const std::string& path);
    const std::vector<MapDef>& maps() const { return m_maps; }
    void register_defaults();
private:
    std::vector<MapDef> m_maps;
};

// ---- Difficulty ------------------------------------------------------------

struct Difficulty {
    std::string profile;
    float player_health_mult = 1.0f;
    float enemy_health_mult = 1.0f;
    float enemy_damage_mult = 1.0f;
    float enemy_accuracy = 0.7f;
    float zombie_speed_mult = 1.0f;
    uint32_t starting_points = 500;
};

class DifficultyManager {
public:
    bool load(const std::string& path);
    const Difficulty* get(const std::string& name) const;
    const Difficulty& current() const { return m_current; }
    void set_profile(const std::string& name);
private:
    std::vector<Difficulty> m_profiles;
    Difficulty m_current;
};

} // namespace bo2
