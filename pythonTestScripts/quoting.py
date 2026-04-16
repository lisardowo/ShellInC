
import writingToterminal
import json
from pathlib import Path

if __name__ == "__main__":

    script_dir = Path(__file__).resolve().parent
    shell_path = script_dir.parent / "shell"
    commands_path = script_dir / "commands.json"

    with open(commands_path, "r", encoding="utf-8") as f:
        data = json.load(f)

    commands = data["stage3"]["commands"]
    expectedOutput = data["stage3"]["expectedOut"]

    raw_out, code = writingToterminal.runShell(shell_path, commands)

    print("==== OUTPUT ====")
    print(writingToterminal.deleteAnsi(raw_out))
    
    print("==== EXPECTED OUTPUT ====")
    for i in range(len(expectedOutput)):
        print(expectedOutput[i])
    
    print(f"\nExit code: {code}")