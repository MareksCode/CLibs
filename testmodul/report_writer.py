import os
from datetime import datetime


class TestReport:
    def __init__(self, title: str, base_dir="reports"):
        """
        base_dir wird relativ zum aktuellen Arbeitsverzeichnis angelegt
        """
        self.title = title
        self.base_dir = os.path.abspath(base_dir)
        self.image_dir = os.path.join(self.base_dir, "images")
        self.tests = []

        # Ordner automatisch anlegen
        os.makedirs(self.image_dir, exist_ok=True)

    def _truncate(self, text, max_length=300):
        """
        Kürzt Text ab max_length Zeichen und fügt Hinweis hinzu
        """
        text = str(text)

        if len(text) <= max_length:
            return text

        return text[:max_length] + "...\n(gekürzt)]"


    # -------------------------------------------------
    # Testfall hinzufügen
    # -------------------------------------------------
    def add_test(
        self,
        name: str,
        input_data: dict,
        expected,
        output,
        passed: bool,
        plots: list[str] | None = None,
        notes: str | None = None,
    ):
        self.tests.append({
            "name": name,
            "input": input_data,
            "expected": expected,
            "output": output,
            "passed": passed,
            "plots": plots or [],
            "notes": notes,
        })

    # -------------------------------------------------
    # Report schreiben
    # -------------------------------------------------
    def write(self, filename="report.md"):
        path = os.path.join(self.base_dir, filename)

        with open(path, "w", encoding="utf-8") as f:
            self._write_header(f)
            for i, test in enumerate(self.tests, start=1):
                self._write_test(f, i, test)

        return path

    # -------------------------------------------------
    # Interne Writer
    # -------------------------------------------------
    def _write_header(self, f):
        f.write(f"# {self.title}\n\n")
        f.write(f"**Erstellt am:** {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n")
        f.write("---\n\n")

    def _write_test(self, f, index, test):
        status = test["passed"]
        match status:
            case True:
                status = "✅ PASS"
            case False:
                status = "❌ FAIL"
            case "maby":
                status = "❓ UNTESTED - Kein Vergleich durchführbar"
            case _:
                status = "⭕ WARNING - Unbekannter Teststatus"

        f.write(f"## Test {index}: {test['name']}\n\n")

        f.write("### Eingabe\n")
        for key, value in test["input"].items():
            f.write(f"- **{key}**: {self._truncate(value)}\n")

        expected_text = self._truncate(test["expected"])
        output_text = self._truncate(test["output"])

        f.write("\n### Erwartete Ausgabe\n")
        f.write(f"```\n{expected_text}\n```\n")

        f.write("\n### Tatsächliche Ausgabe\n")
        f.write(f"```\n{output_text}\n```\n")

        f.write(f"\n### Vergleich\n**{status}**\n")

        if test["notes"]:
            f.write("\n### Hinweise\n")
            f.write(f"{test['notes']}\n")

        if test["plots"]:
            f.write("\n### Visualisierung\n")
            for img in test["plots"]:
                # Bilder relativ zum Report referenzieren
                f.write(f"![Plot](images/{os.path.basename(img)})\n")

        f.write("\n---\n\n")
