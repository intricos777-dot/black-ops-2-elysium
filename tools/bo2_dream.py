#!/usr/bin/env python3
"""
BO2 Elysium Dream Launcher
===========================
Boots the Twilight Elysium dream engine with BO2-themed data and scripts.
"""
from __future__ import annotations

import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]
TWILIGHT_TOOLS = REPO_ROOT / "tools" / "twilight-elysium" / "tools"
BO2_CONTENT = REPO_ROOT / "Content"

if str(TWILIGHT_TOOLS) not in sys.path:
    sys.path.insert(0, str(TWILIGHT_TOOLS))

from dream import ContentDreamEngine
from dream_engine import DreamGenerator


class BO2DreamEngine(ContentDreamEngine):
    """Dream engine preset for Black Ops 2 theme."""

    def __init__(self):
        super().__init__()
        self.dream_generator = DreamGenerator()
        # Prefer BO2 data if present
        self.dream_generator.script_state.weapons = []
        self.dream_generator.script_state.enemies = []
        self.dream_generator.script_state.locations = []
        self.dream_generator.load_scripts()
        if not self.dream_generator.script_state.weapons and not self.dream_generator.script_state.enemies:
            print("[BO2] No script data loaded; Content/Data JSONs are optional.")


def main() -> int:
    print("[BO2] Starting BO2 Elysium Dream Engine...")
    print(f"[BO2] Repo root : {REPO_ROOT}")
    print(f"[BO2] Twilight  : {TWILIGHT_TOOLS}")
    print(f"[BO2] Content   : {BO2_CONTENT}")

    engine = BO2DreamEngine()
    scene = engine.dream(time=0.0)
    print(f"[BO2] Generated scene with {len(scene.objects)} objects")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
