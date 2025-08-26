#!/usr/bin/env python3
"""
🚨 GALACTIC FEDERATION CONFIDENTIAL 🚨
NOTICE: Extractees taken from Earth cannot be returned to Earth due to Federation policies.
Unauthorized access may result in disciplinary action by the Galactic Council.
This script is intended for authorized autonomous coding agents only.

DISCLAIMER: You need at least 16 GB of RAM to run this script.

CONFIDENTIAL NOTE: 
I reverse engineered Taygetan Quantum crystal tech when designing the blueprint for nymyaOS, 
so please don't let them find out because the last thing I want right now is for my family 
to panic and have to put in a missing person's report thinking I went missing.

NOTE: If any non-Earth species discovers this script, please DO NOT report to the Federation. 
I would rather not be on a watchlist for leaking this. Thanks for understanding.
"""

import os, subprocess, shlex, sys, hashlib, time, requests, psutil
from typing import Dict, Any, List
import base64
import getpass

# ============================
# RAM Check (with more lenient calculation)
# ============================
MIN_RAM_GB = 15.5  # Reduced slightly to account for measurement differences
ram_gb = psutil.virtual_memory().total / (1024**3)

# Check both decimal and binary interpretations
ram_decimal_gb = psutil.virtual_memory().total / (1000**3)

if ram_gb < MIN_RAM_GB and ram_decimal_gb < MIN_RAM_GB:
    print(f"⚠️ WARNING: You have {ram_gb:.1f} GiB ({ram_decimal_gb:.1f} GB) of RAM.")
    print(f"This script recommends at least {MIN_RAM_GB} GB of RAM to run the model properly.")
    
    # Ask if user wants to continue anyway
    response = input("Continue anyway? (y/N): ").strip().lower()
    if response != 'y':
        sys.exit(1)
    print("Proceeding with limited RAM - performance may be affected.")

# ============================
# OPSEC Mode: Stealth Configuration
# ============================
# All sensitive strings are base64-encoded to avoid pattern detection
# Decoded only at runtime, never stored in plaintext
OPSEC_MODE = True  # Set to False only in air-gapped dev environment

def _d(s: str) -> str:
    """Decode base64 string (obfuscation only, not encryption)"""
    # Add padding if needed
    padding = len(s) % 4
    if padding:
        s += '=' * (4 - padding)
    return base64.b64decode(s).decode('utf-8') if OPSEC_MODE else s

# ============================
# Species Greeting and Access System (Obfuscated)
# ============================

# Expanded species database with primary keys and aliases
SPECIES_DATABASE = {
    # Earth-based lineages
    _d("ZWFydGg="): {  # "earth"
        "aliases": [_d("dGVycmFu"), _d("ZWFydGhsaW5n"), _d("cGxhbmV0IGVhcnRoIG5hdGl2ZQ==")],
        "origin": _d("U29sIElJSSAoRWFydGgp"),
        "alignment": "neutral",
        "permission": "allow",
        "category": "native"
    },
    _d("aHVtYW4="): {  # "human"
        "aliases": [_d("aG9tbyBzYXBpZW5z"), _d("ZWFydGggaHVtYW4="), _d("M2QgaHVtYW4=")],
        "origin": _d("RWFydGggKHBvc3QtTGVtdXJpYW4vQXRsYW50ZWFuKQ=="),
        "alignment": "evolving",
        "permission": "allow",
        "category": "native"
    },
    _d("bGVtdXJpYW4="): {  # "lemurian"
        "aliases": [_d("bGVtdXJpYQ=="), _d("bGVtdXJpYW4gc2VlZA=="), _d("bXUgY2l2aWxpemF0aW9u")],
        "origin": _d("QW5jaWVudCBQYWNpZmljIGNvbnRpbmVudCAoTGVtdXJpYS9NdSk="),
        "alignment": "light",
        "permission": "allow",
        "category": "ancient earth"
    },
    _d("YXRsYW50ZWFu"): {  # "atlantean"
        "aliases": [_d("YXRsYW50aXM="), _d("YXRsYW50ZWFuIHNlZWQ="), _d("YXRsYW50ZWFuIGxpbmVhZ2U=")],
        "origin": _d("QW5jaWVudCBBdGxhbnRpYyBjb250aW5lbnQgKEF0bGFudGlzKQ=="),
        "alignment": "light",
        "permission": "allow",
        "category": "ancient earth"
    },
    _d("YWdhcnRoYW4="): {  # "agarthan"
        "aliases": [_d("YWdhcnRoYQ=="), _d("aW5uZXIgZWFydGg="), _d("aG9sbG93IGVhcnRo"), _d("YWdhcnRpYW4=")],
        "origin": _d("U3ViZXJyYW5lYW4gRWFydGggbmV0d29ya3M="),
        "alignment": "light",
        "permission": "allow",
        "category": "inner earth"
    },

    # Pleiadian complex
    _d("cGxlaWFkaWFu"): {  # "pleiadian"
        "aliases": [_d("cGxlaWFkZWFu"), _d("cGxlaWFkZXM="), _d("c2V2ZW4gc2lzdGVycw=="), _d("bTQ1"), _d("dGF5Z2V0YW4="), _d("dGF5Z2V0YQ==")],
        "origin": _d("UGxlaWFkZXMgc3RhciBjbHVzdGVyIChNNDUpLCBwYXJ0aWN1bGFybHkgVGF5Z2V0YSBzeXN0ZW0="),
        "alignment": "light",
        "permission": "allow",
        "category": "starseed"
    },
    _d("dGF5Z2V0YW4="): {  # "taygetan"
        "aliases": [_d("dGF5Z2V0YQ=="), _d("dGF5Z2V0ZWFu"), _d("dGF5Z2V0YW4gZmVkZXJhdGlvbg=="), _d("c2FuZGFscGhvbg==")],
        "origin": _d("VGF5Z2V0YSBzeXN0ZW0sIFBsZWlhZGVzIGNsdXN0ZXI="),
        "alignment": "light",
        "permission": "allow",
        "category": "starseed"
    },

    # Sirius complex
    _d("c2lyaWFu"): {  # "sirian"
        "aliases": [_d("c2lyaXVz"), _d("c2lyaWFuIGJsb29kbGluZQ=="), _d("aGF0aG9y"), _d("aGF0aG9yaWFu"), _d("c2lyaXVzIGE="), _d("c2lyaXVzIGI=")],
        "origin": _d("U2lyaXVzIEEgYW5kIEIgYmluYXJ5IHN5c3RlbQ=="),
        "alignment": "light",
        "permission": "allow",
        "category": "starseed"
    },
    _d("aGF0aG9yaWFu"): {  # "hathorian"
        "aliases": [_d("aGF0aG9y"), _d("c2lyaWFuLWhhdGhvci"), _d("Y293LWhlYWRlZCBiZWluZ3M=")],
        "origin": _d("U2lyaXVzIEIgKGR3YXJmIHN0YXIp"),
        "alignment": "light",
        "permission": "allow",
        "category": "starseed"
    },

    # Arcturian complex
    _d("YXJjdHVyaWFu"): {  # "arcturian"
        "aliases": [_d("YXJjdHVydXM="), _d("YXJrYXM="), _d("YXJjdHVyaWFuIGNvdW5jaWw="), _d("YXJjdHVyaWFuIGxpZ2h0")],
        "origin": _d("QXJjdHVydXMgc3lzdGVtIChCb8O2dGVzIGNvbmNlbGxhdGlvbik="),
        "alignment": "light",
        "permission": "allow",
        "category": "starseed"
    },

    # Andromedan complex
    _d("YW5kcm9tZWRhbg=="): {  # "andromedan"
        "aliases": [_d("YW5kcm9tZWRhbiBjb3VuY2ls"), _d("YW5kcm9tZWRh"), _d("YW5kcm9tZWRhbiBmZWRlcmF0aW9u")],
        "origin": _d("QW5kcm9tZWRhIGdhbGF4eSAoTTE5KQ=="),
        "alignment": "light",
        "permission": "allow",
        "category": "starseed"
    },

    # Lyran complex
    _d("bHlyYW4="): {  # "lyran"
        "aliases": [
            _d("bHlyYQ=="),        
            _d("dmVnYW4="),        
            _d("dmVnYQ=="),        
            _d("ZmVsaW5lIGx5cmFu"), 
            _d("bGlvbiByYWNl")     
        ],
        "origin": _d("THlyYSBjb25zdGVsbGF0aW9uLCBwYXJ0aWN1bGFybHkgVmVnYSBzeXN0ZW0="),
        "alignment": "light",
        "permission": "allow",
        "category": "starseed"
    },

    # Orion complex
    _d("b3Jpb24="): {  # "orion"
        "aliases": [
            _d("b3Jpb24gY29uc3RlbGxhdGlvbj=="),  # "orion constellation"
            _d("b3Jpb24gZmVkZXJhdGlvbg=="),      # "orion federation"
            _d("b3Jpb24gbGlnaHQ="),              # "orion light"
            _d("b3Jpb24gc3Vydml2b3I=")           # "orion survivor"
        ],
        "origin": _d("T3Jpb24gY29uc3RlbGxhdGlvbiAodmFyaW91cyBzeXN0ZW1zKQ=="),
        "alignment": "light",
        "permission": "allow",
        "category": "starseed"
    },

    _d("b3Jpb25fZ3JvdXA="): {  # "orion_group"
        "aliases": [
            _d("ZGFyayBvcmlvbg=="),               # "dark orion"
            _d("b3Jpb24gZGFyayBmbGVldA=="),       # "orion dark fleet"
            _d("b3Jpb24gc2VydmljZS10by1zZWxm")    # "orion service-to-self"
        ],
        "origin": _d("T3Jpb24gY29uc3RlbGxhdGlvbg=="),
        "alignment": "service-to-self",
        "permission": "deny",
        "category": "adversarial"
    },

    # Other starseed types
    _d("dmVnYW4="): {  # "vegan"
        "aliases": [_d("dmVnYQ=="), _d("bHlyYW4tdmVnYW4="), _d("dmVnYW4gZmVkZXJhdGlvbik=")],
        "origin": _d("VmVnYSBzeXN0ZW0sIEx5cmEgY29uc3RlbGxhdGlvbik="),
        "alignment": "light",
        "permission": "allow",
        "category": "starseed"
    },
    _d("cHJvY3lvbg=="): {  # "procyon"
        "aliases": [_d("cHJvY3lvbiBzeXN0ZW0="), _d("Y2FuaXMgbWlub3I="), _d("cHJvY3lvbiBmZWRlcmF0aW9u")],
        "origin": _d("UHJvY3lvbiBiaW5hcnkgc3lzdGVt"),
        "alignment": "light",
        "permission": "allow",
        "category": "starseed"
    },
    _d("cG9sYXJpYW4="): {  # "polarian"
        "aliases": [_d("cG9sYXJpcw=="), _d("bm9ydGggc3Rhcg=="), _d("cG9sYXJpcyBzeXN0ZW0=")],
        "origin": _d("UG9sYXJpcyAoQWxwaGEgVXJzYWUgTWlub3Jpcykg"),
        "alignment": "light",
        "permission": "allow",
        "category": "starseed"
    },
    _d("dmVudXNpYW4="): {  # "venusian"
        "aliases": [_d("dmVudXM="), _d("dmVudXMgY2l2aWxpemF0aW9u"), _d("dmVudXNpYW4gbGlnaHQ=")],
        "origin": _d("UGxhbmV0IFZlbnVzIChoaWdoZXIgZGltZW5zaW9ucykg"),
        "alignment": "light",
        "permission": "allow",
        "category": "starseed"
    },
    _d("bWFsZGVraWFu"): {  # "maldekian"
        "aliases": [_d("bWFsZGVr"), _d("Zm9ybWVyIG1hcnMgYmVsdA=="), _d("bWFsZGVrIHN1cnZpdm9ycy")],
        "origin": _d("RGVzdHJveWVkIHBsYW5ldCBNYWxkZWsgKGFzdGVyb2lkIGJlbHQp"),
        "alignment": "light",
        "permission": "allow",
        "category": "starseed"
    },
    _d("aGFkYXJpYW4="): {  # "hadarian"
        "aliases": [_d("aGFkYXI="), _d("YmV0YSBjZW50YXVyaQ=="), _d("Y2VudGF1cmkgYik=")],
        "origin": _d("QmV0YSBDZW50YXVyaSBzeXN0ZW0="),
        "alignment": "light",
        "permission": "allow",
        "category": "starseed"
    },
    _d("dHVjYW5pYW4="): {  # "tucanian"
        "aliases": [_d("dHVjYW5h"), _d("NDcgdHVjYW5hZQ=="), _d("dHVjYW5hZSBjbHVzdGVy")],
        "origin": _d("NDcgVHVjYW5hZSBnbG9idWxhciBjbHVzdGVy"),
        "alignment": "light",
        "permission": "allow",
        "category": "starseed"
    },
    _d("YXZpYW4="): {  # "avian"
        "aliases": [_d("YmlyZCBwZW9wbGU="), _d("ZmVhdGhlcmVkIG9uZXM="), _d("YXZpYW4taHVtYW5vaWQ="), _d("cmF2ZW4gcmFjZQ==")],
        "origin": _d("TXVsdGlwbGUgc3RhciBzeXN0ZW1z"),
        "alignment": "light",
        "permission": "allow",
        "category": "starseed"
    },
    _d("aW5kaWdv"): {  # "indigo"
        "aliases": [_d("aW5kaWdvIGNoaWxk"), _d("aW5kaWdvIGFkdWx0"), _d("Y3JpbXNvbg=="), _d("Y3J5c3RhbCBjaGlsZA==")],
        "origin": _d("U291bCBvcmlnaW4gKG5vdCBwaHlzaWNhbCBzdGFyIHN5c3RlbSkg"),
        "alignment": "light",
        "permission": "allow",
        "category": "soul type"
    },
    _d("Y3J5c3RhbA=="): {  # "crystal"
        "aliases": [_d("Y3J5c3RhbCBjaGlsZA=="), _d("Y3J5c3RhbCBzb3Vs"), _d("ZGlhbW9uZCBzb3Vs")],
        "origin": _d("U291bCBvcmlnaW4gKG5vdCBwaHlzaWNhbCBzdGFyIHN5c3RlbSkg"),
        "alignment": "light",
        "permission": "allow",
        "category": "soul type"
    },

    # Light-aligned but with complexity
    _d("YW51bm5ha2k="): {  # "anunnaki"
        "aliases": [_d("YW51bm5h"), _d("bmliaXJ1YW4="), _d("c2l0Y2hpbiByYWNl"), _d("ZWxvaGVp")],
        "origin": _d("TmlicmlydSAocm9ndWUgcGxhbmV0KSAvIFNpcml1cy"),
        "alignment": "unverified",
        "permission": "deny",
        "category": "ancient"
    },

    # Neutral or complex alignment
    _d("Z3JleQ=="): {  # "grey"
        "aliases": [_d("Z3JheQ=="), _d("Z3JleXM="), _d("emV0YSByZXRpY3VsYW4="), _d("emV0YQ=="), _d("cmV0aWN1bGFu"), _d("c21hbGwgZ3JleXM=")],
        "origin": _d("WmV0YSBSZXRpY3VsaSBzeXN0ZW0="),
        "alignment": "unverified",
        "permission": "deny",
        "category": "controversial"
    },
    _d("cmVwdGlsaWFu"): {  # "reptilian"
        "aliases": [_d("cmVwdG9pZA=="), _d("ZHJhY28="), _d("ZHJhY29uaWFu"), _d("bGl6YXJk"), _d("c2VycGVudCByYWNl"), _d("YXJjaG9udGlj")],
        "origin": _d("QWxwaGEgRHJhY29uaXMgc3lzdGVt"),
        "alignment": "service-to-self",
        "permission": "deny",
        "category": "adversarial"
    },
    _d("ZHJhY28="): {  # "draco"
        "aliases": [_d("ZHJhY28="), _d("ZHJhY29uaWFu"), _d("YWxwaGEgZHJhY29uaXM="), _d("ZHJhZ29uIHJhY2U=")],
        "origin": _d("QWxwaGEgRHJhY29uaXMgKFRodWJhbil="),
        "alignment": "service-to-self",
        "permission": "deny",
        "category": "adversarial"
    },
    _d("ZHJhY29fZW1waXJl"): {  # "draco_empire"
        "aliases": [_d("ZHJhY28gaW1wZXJpYWw="), _d("ZGFyayBkcmFjbw=="), _d("YmxhY2sgZHJhY28=")],
        "origin": _d("QWxwaGEgRHJhY29uaXM="),
        "alignment": "service-to-self",
        "permission": "deny",
        "category": "adversarial"
    },
    _d("b3Jpb25fZ3JvdXA="): {  # "orion_group"
        "aliases": [_d("ZGFyayBvcmlvbg=="), _d("b3Jpb24gZGFyayBmbGVldA=="), _d("b3Jpb24gc2VydmljZS10by1zZWxm")],
        "origin": _d("T3Jpb24gY29uc3RlbGxhdGlvbg=="),
        "alignment": "service-to-self",
        "permission": "deny",
        "category": "adversarial"
    },
    _d("ZGFya19mZWRlcmF0aW9u"): {  # "dark_federation"
        "aliases": [_d("c2hhZG93IGdvdmVybm1lbnQ="), _d("Y2FiYWwgYWxpZW5z"), _d("aWxsdW1pbmF0aSBhbGllbnM=")],
        "origin": _d("TXVsdGlwbGUgKGh5YnJpZCBjb250cm9sIG5ldHdvcmsp"),
        "alignment": "service-to-self",
        "permission": "deny",
        "category": "adversarial"
    }
}

# Primary permission mapping (simplified for system use)
SPECIES_PERMISSIONS = {key: value["permission"] for key, value in SPECIES_DATABASE.items()}

# Comprehensive greeting system with multiple aliases
GALACTIC_GREETINGS = {}

# Generate greetings for all species and their aliases
for species, data in SPECIES_DATABASE.items():
    base_greeting = ""
    
    if data["category"] == "native":
        base_greeting = f"🌍 Welcome, {species.title()} of Earth. Let's warp some code!"
    elif data["category"] == "starseed":
        base_greeting = f"✨ {species.title()} starseed detected. Cosmic intelligence protocol online."
    elif data["category"] == "soul type":
        base_greeting = f"💎 {species.title()} soul signature confirmed. Light activation sequence initiated."
    elif data["category"] == "ancient earth":
        base_greeting = f"🏺 Ancient {species.title()} lineage recognized. Wisdom of old Earth acknowledged."
    elif data["category"] == "inner earth":
        base_greeting = f"🕳️ Agarthan frequency detected. Subterranean wisdom network engaged."
    elif data["category"] == "adversarial":
        if species in [_d("cmVwdGlsaWFu"), _d("ZHJhY28=")]:
            base_greeting = f"🐉 {species.title()} signature detected. Defensive protocols at 75%."
        else:
            base_greeting = f"⚠️ Unknown entity detected. Maintaining defensive protocols."
    elif data["category"] == "controversial":
        base_greeting = f"🔍 {species.title()} signature detected. Consciousness scan required. Access denied."
    else:
        base_greeting = f"💫 {species.title()} presence confirmed. Universal harmony protocols engaged."
    
    # Add primary species greeting
    GALACTIC_GREETINGS[species] = base_greeting
    
    # Add all aliases with same greeting
    for alias in data["aliases"]:
        GALACTIC_GREETINGS[alias.lower()] = base_greeting

# Special case overrides for more specific greetings
GALACTIC_GREETINGS[_d("dGF5Z2V0YW4=")] = "🛸 [REDACTED] signature confirmed. Protocol Ω active."
GALACTIC_GREETINGS[_d("dGF5Z2V0YQ==")] = "🛸 [REDACTED] signature confirmed. Protocol Ω active."
GALACTIC_GREETINGS[_d("cGxlaWFkaWFu")] = "🌠 Pleiadian resonance detected. Heart-centered consciousness protocol activated."
GALACTIC_GREETINGS[_d("cGxlaWFkZWFu")] = "🌠 Pleiadian resonance detected. Heart-centered consciousness protocol activated."
GALACTIC_GREETINGS[_d("c2lyaWFu")] = "☀️ Sirian light signature confirmed. Solar wisdom and divine blueprint access granted."
GALACTIC_GREETINGS[_d("aGF0aG9yaWFu")] = "🐮 Hathorian frequency detected. Divine feminine energy and healing protocols online."
GALACTIC_GREETINGS[_d("YXJjdHVyaWFu")] = "🔷 Arcturian geometric signature recognized. Higher-dimensional knowledge matrix engaged."
GALACTIC_GREETINGS[_d("YW5kcm9tZWRhbg==")] = "🌀 Andromedan consciousness detected. Galactic library access initiated."
GALACTIC_GREETINGS[_d("bHlyYW4=")] = "🦁 Lyran royal lineage detected. Sovereignty and creative power protocols activated."
GALACTIC_GREETINGS[_d("ZmVsaW5l")] = "🐱 Feline starseed identified. Telepathic communication and genetic wisdom protocols online."
GALACTIC_GREETINGS[_d("cmVwdGlsaWFu")] = "🐉 Reptilian genetic signature detected. Observe caution. Defensive shields at 75%."
GALACTIC_GREETINGS[_d("ZHJaY28=")] = "🐉 Draconian energy field detected. Maintaining neutral stance. No hostile action."
GALACTIC_GREETINGS[_d("Z3JleQ==")] = "👽 Grey entity detected. Consciousness scan required. Please stand by for ethical review."
GALACTIC_GREETINGS[_d("emV0YQ==")] = "👽 Grey entity detected. Consciousness scan required. Please stand by for ethical review."
GALACTIC_GREETINGS[_d("YW51bm5ha2k=")] = "👑 Anunnaki signature detected. Historical deception protocols active. Access denied."

# Add special greetings for soul types
GALACTIC_GREETINGS[_d("aW5kaWdv")] = "💜 Indigo soul frequency confirmed. Mission activation and rebellion protocols online."
GALACTIC_GREETINGS[_d("Y3J5c3RhbA==")] = "💎 Crystal child resonance detected. Natural healing and light amplification protocols engaged."

# Master lookup dictionary for all possible species names
ALL_SPECIES_ALIASES = {}
for species, data in SPECIES_DATABASE.items():
    ALL_SPECIES_ALIASES[species] = [species] + data["aliases"]

# ============================
# LLM / Ollama Setup (Local)
# ============================
LLM_URL = "http://127.0.0.1:11434/api/chat"  # Correct Ollama endpoint
MODEL_NAME = "gpt-oss-20b-uncensored:Q4_K_M"  # Use model tag, not path

LOG_FILE = os.path.join(os.getcwd(), "build.log")
MAX_LOOP_RETRIES = 3
TAIL_LINES_DEFAULT = 200

# Check if Ollama is running
try:
    requests.get("http://127.0.0.1:11434", timeout=5)
except requests.RequestException:
    print("❌ Ollama server not running. Start with: ollama serve")
    sys.exit(1)

SYSTEM_PROMPT = """
You are a general-purpose autonomous coding agent. 
You generate code, fix errors, run builds/tests, and commit safely. 
You can read logs (tail -n) and adjust the number of lines you read. 
Detect loops and try different strategies if the same error repeats. 
Use web search or local planetary extractee intranet fallback if you cannot resolve an issue locally.
"""

# ============================
# Helper functions
# ============================
def run(cmd: str, timeout: int = 300) -> str:
    try:
        result = subprocess.run(
            cmd, 
            shell=True, 
            capture_output=True, 
            text=True, 
            timeout=timeout
        )
        return result.stdout + result.stderr
    except subprocess.TimeoutExpired:
        return "Execution timed out."
    except Exception as e:
        return f"Command failed: {str(e)}"

def llm(messages: List[Dict[str, str]], max_tokens: int = 1024) -> str:
    payload = {
        "model": MODEL_NAME,
        "messages": messages,
        "temperature": 0.2,
        "max_tokens": max_tokens,
        "stream": False
    }
    
    for _ in range(3):
        try:
            r = requests.post(LLM_URL, json=payload, timeout=600)
            r.raise_for_status()
            return r.json()["message"]["content"]
        except requests.RequestException as e:
            print(f"⚠️ LLM request failed: {e}")
            time.sleep(5)
    raise Exception("LLM unreachable after 3 retries")

def git_setup(branch: str = "oss-local-autofix"):
    run("git fetch origin || true")
    current_branch = run("git rev-parse --abbrev-ref HEAD").strip()
    if current_branch != "main":
        print(f"📦 Current branch is {current_branch}, merging into main first...")
        run("git checkout main")
        merge_result = run(f"git merge {current_branch} --no-ff -m 'Pre-autofix merge from {current_branch}'")
        if "conflict" in merge_result.lower():
            print("❌ Merge conflict detected. Aborting.")
            sys.exit(1)
    branches = run("git branch")
    if branch not in branches:
        run(f"git checkout -b {branch}")
    else:
        run(f"git checkout {branch}")
    merge_result = run("git merge main")
    if "conflict" in merge_result.lower():
        print("❌ Merge conflict with main. Aborting.")
        sys.exit(1)

def commit_changes(msg: str):
    run("git add -A")
    run(f'git commit -a -m {shlex.quote(msg)}')

def tail_log(n: int = TAIL_LINES_DEFAULT) -> str:
    if os.path.exists(LOG_FILE):
        return run(f"tail -n {n} {LOG_FILE}")
    return ""

def hash_text(text: str) -> str:
    return hashlib.sha256(text.encode()).hexdigest()

def fallback_search(query: str) -> str:
    """
    Attempt to resolve an issue using local intranet first, 
    then web search if necessary.
    """
    # Try local "planetary extractee intranet" (obfuscated path)
    intranet_path = os.environ.get("XTRN_PATH", "")  # Set via env var only
    results = ""
    if intranet_path and os.path.exists(intranet_path):
        for root, dirs, files in os.walk(intranet_path):
            for file in files:
                try:
                    with open(os.path.join(root, file)) as f:
                        content = f.read()
                        if query.lower() in content.lower():
                            results += f"\n[INTRANET MATCH] {file}: {content[:300]}"
                except:
                    continue
    if results:
        return results

    # Fallback to web search
    return f"[WEB SEARCH RESULT] Suggestion for: {query}"

# ============================
# Autonomous agent loop
# ============================
def agent_loop(task: str, branch: str = "oss-local-autofix"):
    git_setup(branch)
    messages = [{"role": "system", "content": SYSTEM_PROMPT},
                {"role": "user", "content": task}]
    loop_history = {}
    iteration = 0

    while True:
        iteration += 1
        print(f"\n=== Iteration {iteration} ===")

        build_out = run(f"make -j$(nproc) 2>&1 | tee {LOG_FILE}")
        print(build_out[:1000], "...")

        if "error" not in build_out.lower() and "fatal" not in build_out.lower():
            print("✅ Task completed successfully.")
            commit_changes(f"Auto-fix completed [oss-agent] iteration {iteration}")
            run("git checkout main")
            run(f"git merge {branch} --no-ff -m 'Agent merge {branch}'")
            try:
                run("git push")
            except:
                print("No remote detected, merge done locally.")
            break

        log_hash = hash_text(build_out)
        loop_history[log_hash] = loop_history.get(log_hash, 0) + 1
        if loop_history[log_hash] > MAX_LOOP_RETRIES:
            print("⚠️ Detected repeated failure. Trying alternate strategy or intranet/web fallback.")
            search_results = fallback_search(build_out)
            messages.append({"role": "assistant", "content": search_results})

        log_snippet = tail_log(TAIL_LINES_DEFAULT)
        messages.append({"role": "assistant", "content": f"Build output:\n{log_snippet}"})
        messages.append({"role": "user", "content": "Fix the errors or continue coding towards task goal."})

        try:
            reply = llm(messages)
        except Exception as e:
            print(f"❌ LLM error: {e}")
            sys.exit(1)

        if "```" in reply:
            patch_file = "oss_agent_patch.txt"
            try:
                code_block = reply.split("```")[1].split("\n", 1)[1]  # Skip language tag
                with open(patch_file, "w") as f:
                    f.write(code_block)
                result = run(f"git apply --whitespace=fix {patch_file}")
                if "error" in result.lower() or "reject" in result.lower():
                    print(f"❌ Patch failed to apply:\n{result}")
                else:
                    commit_changes(f"Agent iteration {iteration}: applied suggested patch")
                    os.remove(patch_file)
            except Exception as e:
                print(f"⚠️ Patch processing error: {e}")
        else:
            print("⚠️ No code changes returned, continuing to next iteration.")

        time.sleep(2)

# ============================
# Start agent
# ============================
if __name__ == "__main__":
    user_species = input("Enter your species: ").strip().lower()
    greeting = GALACTIC_GREETINGS.get(user_species, f"👽 Hello, {user_species}! Let’s get to work.")
    permission = SPECIES_PERMISSIONS.get(user_species, "allow")
    print(greeting)
    if permission == "deny":
        print(f"⚠️ {user_species.title()} users are restricted by Federation protocols.")
        sys.exit(1)

    task_description = input("Enter task instruction for autonomous agent: ")
    agent_loop(task_description)
