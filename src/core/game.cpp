#include "game.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <algorithm>

namespace bo2 {

Game::Game() {}

bool Game::initialize() {
    // Load data or fallback to defaults
    if (!m_weapons.load("Content/Data/weapons.json")) m_weapons.register_defaults();
    if (!m_perks.load("Content/Data/perks.json")) m_perks.register_defaults();
    if (!m_maps.load("Content/Data/maps.json")) m_maps.register_defaults();
    if (!m_diff.load("Content/Data/Difficulties.json")) {
        Difficulty d; d.profile = "casual"; d.starting_points = 500;
        m_diff.set_profile("casual");
    }
    m_npcs.register_defaults();

    m_player.max_hp = 100 * m_diff.current().player_health_mult;
    m_player.hp = m_player.max_hp;
    m_player.points = m_diff.current().starting_points;

    std::printf("[BO2] Initialized.\n");
    return true;
}

void Game::shutdown() {
    std::printf("[BO2] Shutdown.\n");
}

void Game::show_menu() {
    std::printf("\x1b[38;5;214m\x1b[1m");
    std::printf("  BLACK OPS 2 ELYSIUM\n");
    std::printf("  ==================\n");
    std::printf("\x1b[0m");
    std::printf("  \x1b[2m zombies are back. And they want your points.\x1b[0m\n\n");
    std::printf("  1) select map\n");
    std::printf("  2) start zombie wave\n");
    std::printf("  3) visit NPC\n");
    std::printf("  4) shop / upgrades\n");
    std::printf("  d) set difficulty (current: %s)\n", m_state.difficulty.c_str());
    std::printf("  q) quit\n");
    draw_hud();
}

void Game::draw_hud() {
    std::printf("  \x1b[2mHP: %.0f/%.0f | Weapon: %s | Points: %u | Kills: %u | Wave: %u\x1b[0m\n",
                m_player.hp, m_player.max_hp, m_player.weapon.c_str(),
                m_player.points, m_player.kills, m_state.wave);
}

void Game::select_map() {
    std::printf("\n  \x1b[38;5;214mMAPS:\x1b[0m\n");
    for (size_t i = 0; i < m_maps.maps().size(); ++i) {
        const auto& m = m_maps.maps()[i];
        std::printf("  %zu) %-20s [mode=%s players=%u]\n", i + 1, m.name.c_str(), m.mode.c_str(), m.max_players);
    }
    std::printf("  \x1b[2m[map]\x1b[0m ");
    char buf[32];
    if (!std::fgets(buf, sizeof(buf), stdin)) return;
    int n = std::atoi(buf);
    if (n >= 1 && n <= (int)m_maps.maps().size()) {
        m_state.current_map = m_maps.maps()[n - 1].name;
        std::printf("  \x1b[38;5;46mSelected: %s\x1b[0m\n", m_state.current_map.c_str());
    }
}

void Game::run_wave() {
    m_state.wave++;
    WaveManager wm(m_diff);
    wm.start_wave(m_state.wave);
    auto zombies = wm.spawn_wave();

    std::printf("\n  \x1b[38;5;196m\x1b[1mWAVE %u INCOMING\x1b[0m\n", m_state.wave);
    std::printf("  \x1b[2m%zu zombies shambling toward you.\x1b[0m\n", zombies.size());

    for (size_t zi = 0; zi < zombies.size() && !m_state.game_over; ++zi) {
        std::vector<ZombieDef> alive(1, zombies[zi]);
        std::printf("\n  \x1b[38;5;196mZombie %zu/%zu\x1b[0m\n", zi + 1, zombies.size());
        ZombieCombat combat(m_diff);
        combat.engage(zombies[zi], m_player);

        while (!combat.is_over()) {
            std::printf("    [a]ttack [r]eload [g]renade [f]lee: ");
            char cbuf[16];
            if (!std::fgets(cbuf, sizeof(cbuf), stdin)) break;
            std::vector<std::string> log;
            if (cbuf[0] == 'a') log = combat.attack();
            else if (cbuf[0] == 'r') log = combat.reload();
            else if (cbuf[0] == 'g') log = combat.use_equipment();
            else if (cbuf[0] == 'f') log = combat.flee();
            else if (cbuf[0] == 'q') break;
            for (auto& l : log) std::printf("      %s\n", l.c_str());
            if (combat.is_over()) break;
        }

        if (combat.player_won()) {
            m_player.kills++;
            m_state.total_kills++;
            uint32_t reward = zombies[zi].reward;
            m_player.points += reward;
            std::printf("    \x1b[38;5;46mZombie down! +%u points\x1b[0m\n", reward);
        } else if (combat.state() == CombatState::defeat) {
            m_state.game_over = true;
            std::printf("    \x1b[38;5;196mYou were overrun. Wave over.\x1b[0m\n");
            return;
        }
        m_player.hp = std::max(1.0f, (float)combat.player_hp());
    }

    std::printf("\n  \x1b[38;5;220mWAVE %u CLEARED! +%u bonus points\x1b[0m\n", m_state.wave, m_state.wave * 100);
    m_player.points += m_state.wave * 100;
}

void Game::visit_npc() {
    if (m_npcs.npcs().empty()) { std::printf("  No NPCs.\n"); return; }
    auto npc = &m_npcs.npcs()[std::rand() % m_npcs.npcs().size()];
    std::string line;
    if (!npc->lines.empty()) line = npc->lines[std::rand() % npc->lines.size()];

    std::printf("\n  \x1b[38;5;81m\x1b[1m[%s]\x1b[0m \x1b[2m%s\x1b[0m\n",
                npc->name.c_str(), npc->role.c_str());
    std::printf("  \"%s\"\n", line.c_str());
}

void Game::shop() {
    std::printf("\n  \x1b[38;5;220mMYSTERY BOX / SHOP\x1b[0m\n");
    std::printf("  Points: %u\n", m_player.points);
    std::printf("  1) Mystery Box roll (750 pts)\n");
    std::printf("  2) Buy Juggernog (2500 pts)\n");
    std::printf("  3) Upgrade weapon (5000 pts)\n");
    std::printf("  q) back\n");
    std::printf("  \x1b[2m[shop]\x1b[0m ");
    char buf[32];
    if (!std::fgets(buf, sizeof(buf), stdin)) return;
    int n = std::atoi(buf);
    if (n == 1 && m_player.points >= 750) {
        m_player.points -= 750;
        auto w = m_weapons.weapons();
        if (!w.empty()) {
            auto new_w = &w[std::rand() % w.size()];
            m_player.weapon = new_w->name;
            std::printf("  You got the \x1b[1m%s\x1b[0m!\n", new_w->name.c_str());
        }
    } else if (n == 2 && m_player.points >= 2500) {
        m_player.points -= 2500;
        m_player.max_hp *= 1.5f;
        m_player.hp = m_player.max_hp;
        std::printf("  \x1b[38;5;46mJuggernog equipped! +HP\x1b[0m\n");
    } else if (n == 3 && m_player.points >= 5000) {
        m_player.points -= 5000;
        std::printf("  \x1b[38;5;220mWeapon upgraded: PAP'd!\x1b[0m\n");
    }
}

} // namespace bo2

int main() {
    bo2::Game game;
    if (!game.initialize()) return 1;

    while (true) {
        game.show_menu();
        std::printf("  \x1b[2m[menu]\x1b[0m ");
        char buf[32];
        if (!std::fgets(buf, sizeof(buf), stdin)) break;
        std::string cmd;
        for (char* p = buf; *p; ++p)
            if (*p != '\n' && *p != '\r') cmd += (char)std::tolower(*p);
        if (cmd == "q" || cmd == "quit") break;
        if (cmd == "1") game.select_map();
        else if (cmd == "2") game.run_wave();
        else if (cmd == "3") game.visit_npc();
        else if (cmd == "4") game.shop();
        else if (cmd == "d") {
            std::printf("    difficulty (casual/legend): ");
            char dbuf[32];
            std::fgets(dbuf, sizeof(dbuf), stdin);
            std::string dname;
            for (char* p = dbuf; *p; ++p)
                if (*p != '\n' && *p != '\r') dname += (char)std::tolower(*p);
            game.diff().set_profile(dname);
            game.state().difficulty = dname;
        }
    }
    game.shutdown();
    return 0;
}
