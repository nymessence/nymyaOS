import os
import time
import subprocess
import glob
import logging
import google.generativeai as genai

# Configure your API key. It's recommended to set it as an environment variable
# GEMINI_API_KEY and the client will pick it up automatically.
# Alternatively, you can explicitly provide it by calling genai.configure()
# with the key.

# We will use the model specified in your request.
MODEL_NAME = "gemini-2.5-flash"

# Map of related gates so Gemini knows what to cross-link
RELATED_GATES = {
    "3301_identity_gate": ["3302_global_phase", "3303_pauli_x", "3304_pauli_y", "3305_pauli_z"],
    "3302_global_phase": ["3301_identity_gate", "3306_phase_gate"],
    "3303_pauli_x": ["3304_pauli_y", "3305_pauli_z", "3308_hadamard_gate"],
    "3304_pauli_y": ["3303_pauli_x", "3305_pauli_z"],
    "3305_pauli_z": ["3303_pauli_x", "3304_pauli_y", "3315_phase_shift"],
    "3306_phase_gate": ["3302_global_phase", "3315_phase_shift"],
    "3307_sqrt_x_gate": ["3303_pauli_x", "3308_hadamard_gate"],
    "3308_hadamard_gate": ["3303_pauli_x", "3305_pauli_z", "3307_sqrt_x_gate"],
    "3309_controlled_not": ["3310_anticontrol_not", "3311_controlled_z", "3312_double_controlled_not"],
    "3310_anticontrol_not": ["3309_controlled_not"],
    "3311_controlled_z": ["3309_controlled_not", "3317_controlled_phase"],
    "3312_double_controlled_not": ["3309_controlled_not"],
    "3313_swap": ["3314_imaginary_swap", "3326_sqrt_swap", "3329_fredkin"],
    "3314_imaginary_swap": ["3313_swap"],
    "3315_phase_shift": ["3305_pauli_z", "3306_phase_gate"],
    "3316_phase_gate": ["3306_phase_gate"],
    "3317_controlled_phase": ["3311_controlled_z", "3318_controlled_phase_s"],
    "3318_controlled_phase_s": ["3317_controlled_phase"],
    "3319_rotate_x": ["3320_rotate_y", "3321_rotate_z", "3330_rotate"],
    "3320_rotate_y": ["3319_rotate_x", "3321_rotate_z", "3330_rotate"],
    "3321_rotate_z": ["3319_rotate_x", "3320_rotate_y", "3330_rotate"],
    "3326_sqrt_swap": ["3313_swap", "3327_sqrt_iswap"],
    "3327_sqrt_iswap": ["3326_sqrt_swap"],
    "3328_swap_pow": ["3313_swap"],
    "3329_fredkin": ["3313_swap"],
    "3330_rotate": ["3319_rotate_x", "3320_rotate_y", "3321_rotate_z"],
}

# Dummy functions for demonstration, assuming you have these defined elsewhere
def read_file(filepath):
    """Reads the content of a file."""
    try:
        with open(filepath, 'r') as f:
            return f.read()
    except Exception as e:
        logging.error(f"Failed to read file {filepath}: {e}")
        return None

def make_prompt_for_gemini(content, related_entries, filename):
    """Creates a prompt for the Gemini API."""
    # Placeholder: Your actual prompt-building logic would go here
    return f"Rewrite the following man page ({filename}) with a more poetic and philosophical tone. The content is:\n\n---\n{content}\n---\n\nRelated gates to reference are: {', '.join(related_entries)}"

def call_gemini_api(prompt, model):
    """Calls the Gemini API using the genai client to get a rewritten man page."""
    try:
        # Correctly call the generate_content function on the model object
        response = model.generate_content(contents=prompt)
        return response.text
    except Exception as e:
        # Catch specific API-related errors if needed, but a general catch is fine for now.
        raise RuntimeError(f"Gemini API call failed: {e}")

def sanitize_model_output(raw_out):
    """Cleans up the model's output."""
    # Placeholder: Your actual sanitization logic
    if raw_out.startswith('```groff'):
        return raw_out.strip().lstrip('```groff').rstrip('```').strip()
    return raw_out

def replace_th_date(polished, target_date):
    """Replaces the date in the .TH section."""
    # Placeholder: Your actual logic for replacing the date
    return polished

def replace_see_also(polished, formatted_see_also):
    """Replaces the SEE ALSO block."""
    # Placeholder: Your actual logic for replacing the see also block
    return polished

def backup_file(filepath):
    """Creates a backup of the original file."""
    # Placeholder: Your actual backup logic
    pass

def write_file(filepath, content):
    """Writes content to a file."""
    try:
        with open(filepath, 'w') as f:
            f.write(content)
    except Exception as e:
        logging.error(f"Failed to write to file {filepath}: {e}")

def related_entries_for(filepath):
    """Returns the list of related entries for a given filepath."""
    # Placeholder: Your actual logic to get related gates
    return ["3302_global_phase", "3303_pauli_x"]

def extract_canonical_name(filepath):
    """Extracts the canonical name from a filepath."""
    # Placeholder: Your actual logic
    return "3301_identity_gate"

def format_see_also(gates):
    """Formats the SEE ALSO block."""
    # Placeholder: Your actual logic
    return ".Xr nymya_3302_global_phase 2"

def detect_related_gates(canonical_name, name_map):
    """Detects related gates from the map."""
    # Placeholder: Your actual logic
    return name_map.get(canonical_name, [])

# The functions below were present in your original code but not defined.
# I've added them as dummy functions to make the script runnable.
TARGET_DATE = "2025-08-19"
name_map = RELATED_GATES

def load_gemini_key():
    token_path = os.path.expanduser("~/.gemini_token")
    with open(token_path, "r") as f:
        return f.read().strip()

def rewrite_and_commit(filepath, model, endpoint):
    original = read_file(filepath)
    if original is None:
        return False
    
    # build prompt as usual
    prompt = make_prompt_for_gemini(original, related_entries_for(filepath), os.path.basename(filepath))

    try:
        raw_out = call_gemini_api(prompt, model)
    except Exception as e:
        logging.error("Gemini API failed for %s: %s", filepath, e)
        return False  # skip commit

    polished = sanitize_model_output(raw_out)

    # ensure .TH date and SEE ALSO block
    polished = replace_th_date(polished, TARGET_DATE)
    polished = replace_see_also(polished, format_see_also(detect_related_gates(extract_canonical_name(filepath), name_map)))

    if polished == original:
        logging.info("No changes detected for %s, skipping write/commit", filepath)
        return False

    backup_file(filepath)
    write_file(filepath, polished)

    # only commit if file changed
    try:
        subprocess.run(["git", "add", filepath], check=True)
        subprocess.run(["git", "commit", "-m", f"Rewrite {os.path.basename(filepath)} with Gemini 2.5 Flash"], check=True)
        logging.info("Committed %s", filepath)
    except subprocess.CalledProcessError as e:
        logging.error("Git commit failed for %s: %s", filepath, e)
        return False # Return False on failure to prevent waiting
    return True

def main():
    # It's a good idea to set up basic logging at the start
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    api_key = load_gemini_key()
    if not api_key:
        logging.error("API key not found. Please set a key in ~/.gemini_token.")
        return

    # Configure the API key before making any calls
    genai.configure(api_key=api_key)
    model = genai.GenerativeModel(MODEL_NAME)

    manpages = sorted(glob.glob("*.1"))

    for idx, filepath in enumerate(manpages):
        print(f"[{idx+1}/{len(manpages)}] Rewriting {filepath}...")
        # The rewrite_and_commit function now handles all the logic,
        # including committing the changes if the API call is successful and
        # the file is updated. It returns a boolean to indicate success.
        file_changed = rewrite_and_commit(filepath, model, MODEL_NAME)
        
        # Only wait if a change was actually committed
        if idx < len(manpages) - 1 and file_changed:
            print("Waiting 60 seconds before next file...")
            time.sleep(60)
        elif not file_changed:
            print(f"Skipping wait for {filepath} because no changes were committed.")

if __name__ == "__main__":
    main()

