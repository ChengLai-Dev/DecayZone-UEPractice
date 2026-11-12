"""Generate FLUX concept image(s) of a circular industrial extraction platform.

Uses the FLUX graph from ImagePipeline/scripts/run_matrix.py build_flux (16 steps, CFG 1,
euler/simple) against the already-running ComfyUI on 8188.
"""
import argparse
import importlib.util
import json
import time
import urllib.parse
import urllib.request
from pathlib import Path

import requests

SERVER = "http://127.0.0.1:8188"
RUN_MATRIX = r"D:\AIGamePipeline\ImagePipeline\scripts\run_matrix.py"
OUT_DIR = Path(r"D:\Code\temp\extractionplatform\concept")

PROMPT = (
    "a single circular industrial metal platform, a thick round steel deck with radial panel "
    "seams and bolted plating, dark weathered gunmetal, a glowing teal-cyan light strip running "
    "all the way around the outer rim, chamfered edge, slightly elevated three-quarter view, "
    "near orthographic perspective, single game asset, product shot, isolated on plain white "
    "background, even lighting, no shadow, full object in frame with margin around it, high detail"
)


def load_build_flux():
    spec = importlib.util.spec_from_file_location("run_matrix", RUN_MATRIX)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module.build_flux


def wait(prompt_id, timeout=900):
    start = time.time()
    while time.time() - start < timeout:
        hist = requests.get(f"{SERVER}/history/{prompt_id}", timeout=30).json()
        if prompt_id in hist:
            item = hist[prompt_id]
            status = item.get("status", {})
            if status.get("status_str") == "error":
                raise RuntimeError(json.dumps(status, ensure_ascii=False)[:1000])
            for node_out in item.get("outputs", {}).values():
                if node_out.get("images"):
                    return node_out["images"][0]
        time.sleep(1.0)
    raise TimeoutError(prompt_id)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--seeds", type=int, nargs="+", default=[7701])
    parser.add_argument("--size", type=int, default=1024)
    args = parser.parse_args()

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    build_flux = load_build_flux()

    for seed in args.seeds:
        graph = build_flux(PROMPT, args.size, args.size, seed)
        graph["10"]["inputs"]["filename_prefix"] = f"platform_seed{seed}"
        resp = requests.post(f"{SERVER}/prompt", json={"prompt": graph}, timeout=60)
        if resp.status_code != 200:
            raise RuntimeError(f"submit failed {resp.status_code}: {resp.text[:500]}")
        prompt_id = resp.json()["prompt_id"]
        print(f"[seed {seed}] submitted {prompt_id}", flush=True)
        info = wait(prompt_id)
        qs = urllib.parse.urlencode({
            "filename": info["filename"],
            "subfolder": info.get("subfolder", ""),
            "type": info.get("type", "output"),
        })
        dest = OUT_DIR / f"platform_seed{seed}.png"
        with urllib.request.urlopen(f"{SERVER}/view?{qs}", timeout=120) as r:
            dest.write_bytes(r.read())
        print(f"[seed {seed}] saved {dest}", flush=True)


if __name__ == "__main__":
    main()
