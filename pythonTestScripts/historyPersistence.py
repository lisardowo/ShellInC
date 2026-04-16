"""echo persist1
echo persist2
history
exit
Sesion 2:
5. history
6. echo persist3
7. exit

"""

import writingToterminal
import json
from pathlib import Path

if __name__ == "__main__":
    script_dir = Path(__file__).resolve().parent
    shell_path = script_dir.parent / "shell"
    commands_path = script_dir / "commands.json"

    history_path = script_dir / "historyFile.txt"
    print("cleaning history")
    history_path.write_text("", encoding="utf-8")

    with open(commands_path, "r", encoding="utf-8") as f:
        data = json.load(f)

    s1 = data["stage10"]["commands"]["session1"]
    s2 = data["stage10"]["commands"]["session2"]

    expected_out1 = data["stage10"]["expectedOut"]["session1"]
    expected_out2 = data["stage10"]["expectedOut"]["session2"]

    out1, code1 = writingToterminal.runShell(shell_path, s1)
    out2, code2 = writingToterminal.runShell(shell_path, s2)

    print("==== OUTPUT ====")
    print("==== Session 1 ====")
    print(writingToterminal.deleteAnsi(out1))
    print(f"\nExit code: {code1}")

    print("==== Session 2 ====")
    print(writingToterminal.deleteAnsi(out2))
    print(f"\nExit code: {code2}")

    print("==== EXPECTED OUTPUT ====")
    print("==== Session 1 ====")
    for i in range(len(expected_out1)):
        print(expected_out1[i])
    print("==== Session 2 ====")
    for i in range(len(expected_out1)):
        print(expected_out1[i])

