
import writingToterminal
import json
from pathlib import Path

if __name__ == "__main__":
    #\t in commands means TAB
    script_dir = Path(__file__).resolve().parent
    shell_path = script_dir.parent / "shell"
    commands_path = script_dir / "commands.json"

    with open(commands_path, "r", encoding="utf-8") as f:
        data = json.load(f)

    commands = data["stage5"]["commands"]
    proccessed = []
    for command in commands:
        proccessed.append(writingToterminal.expandSpecialKeys(command))
    
    expectedOutput = data["stage5"]["expectedOut"]

    raw_out, code = writingToterminal.runShell(shell_path, proccessed)

    print("==== OUTPUT ====")
    print(writingToterminal.deleteAnsi(raw_out))
    
    print("==== EXPECTED OUTPUT ====")
    for i in range(len(expectedOutput)):
        print(expectedOutput[i])
    
    print(f"\nExit code: {code}")