#!/usr/bin/env python3
"""
BO2 Elysium Dream Launcher
===========================
Boots the Twilight Elysium dream engine with BO2-themed data and scripts.
Supports casual/legend difficulties and 50-player zombie zLobbies.
"""
from __future__ import annotations

import json
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]
ENGINE_TOOLS = REPO_ROOT / "tools" / "twilight-elysium" / "tools"
BO2_CONTENT = REPO_ROOT / "Content"

if str(ENGINE_TOOLS) not in sys.path:
    sys.path.insert(0, str(ENGINE_TOOLS))

from dream import ContentDreamEngine
from dream_engine import DreamGenerator


class BO2DreamEngine(ContentDreamEngine):
    """Dream engine preset for Black Ops 2 theme."""

    def __init__(self, difficulty: str = "casual"):
        super().__init__()
        self.dream_generator = DreamGenerator()
        # Prefer BO2 data if present
        self.dream_generator.script_state.weapons = []
        self.dream_generator.script_state.enemies = []
        self.dream_generator.script_state.locations = []
        self.dream_generator.load_scripts()
        if not self.dream_generator.script_state.weapons and not self.dream_generator.script_state.enemies:
            print("[BO2] No script data loaded; Content/Data JSONs are optional.")

        self.difficulty = self._load_difficulty(difficulty)
        self.zlobby = self._load_zlobby()

    def _load_difficulty(self, name: str) -> dict:
        f = BO2_CONTENT / "Data" / "Difficulties.json"
        if not f.exists():
            return {"profile": name, "settings": {}}
        data = json.loads(f.read_text())
        profiles = data.get("profiles", {})
        name = name if name in profiles else data.get("default", "casual")
        return {"profile": name, "settings": profiles.get(name, {})}

    def _load_zlobby(self) -> dict:
        f = BO2_CONTENT / "Data" / "BO2_ZLobbies.json"
        if not f.exists():
            return {"enabled": False}
        return json.loads(f.read_text()).get("zlobby", {"enabled": False})


def main() -> int:
    print("[BO2] Starting BO2 Elysium Dream Engine...")
    print(f"[BO2] Repo root : {REPO_ROOT}")
    print(f"[BO2] Twilight  : {ENGINE_TOOLS}")
    print(f"[BO2] Content   : {BO2_CONTENT}")

    difficulty = "casual"
    if len(sys.argv) > 1 and sys.argv[1] in ("casual", "legend"):
        difficulty = sys.argv[1]

    engine = BO2DreamEngine(difficulty=difficulty)
    diff = engine.difficulty
    print(f"[BO2] Difficulty: {diff['profile']}")
    if diff["settings"]:
        print(f"[BO2]   player  -> {diff['settings'].get('player')}")
        print(f"[BO2]   combat  -> {diff['settings'].get('combat')}")

    lobby = engine.zlobby
    if lobby.get("enabled"):
        print(f"[BO2] zLobby   -> 50-player enabled, regions={lobby.get('regions')}")
    else:
        print("[BO2] zLobby   -> disabled")

    scene = engine.dream()
    print(f"[BO2] Generated scene with {len(scene.objects)} objects")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
