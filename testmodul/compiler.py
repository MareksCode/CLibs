import os
import subprocess
from tkinter import filedialog
class compiler():
    def __init__(self, filename:str, filepath:str = None) -> None:
        """
        Läd die klasse. Im endefffekt nur zum Compilen.

        :param filename: name der c-bibliothek. ohne dateiendung angeben. es wird gleicher header- und dateiname erwartet.
        :type filename: str
        :param filepath: sollten die bibliotheken nicht im gleichen verzeichnis liegen, kann hier der pfad zum ordner angegeben werden.
        :type filepath: str
        """
        self.filename = filename
        self.filepath = filepath if filepath is not None else os.path.dirname(__file__)
        print("" + self.filename + "   " + self.filepath)
        
    
    def compile_library(self, fpath:str = None):
        """
        Kompiliert die c-bibliothek
        
        :param fpath: pfad, zu dem die co-datei geschrieben werden soll8
        :type fpath: str
        """
        if fpath is None:
            fpath = self.filepath
        if not os.path.exists(self.filepath):
            raise FileNotFoundError("mms25.c oder mms25.h fehlt!")
        
        if not self.check_gcc():
            raise FileNotFoundError("gcc fehlt! datei kann nciht kompiliert werden!")
        
        print(f"🔧 Kompiliere {self.filename}.c…")

        os.system(f"gcc -fPIC -shared -o {os.path.join(fpath, f'{self.filename}.co')} {os.path.join(self.filepath, self.filename)}.c")

        print("✔ Kompiliert: mms25.o\n")

    def check_gcc(self):
        """
        testet ob gcc installiert ist        
        """
        try:
            subprocess.check_output(["gcc", "--version"], stderr=subprocess.STDOUT)
            return True
        except (subprocess.CalledProcessError, FileNotFoundError):
            return False

if __name__ == "__main__":
    file_path = filedialog.askopenfilename(
        title="C-Datei auswählen",
        filetypes=[("C Dateien", "*.c"), ("Alle Dateien", "*.*")]
    )

    if not file_path:
        raise RuntimeError("Keine Datei ausgewählt")

    # Pfad zerlegen
    directory, filename_with_ext = os.path.split(file_path)
    filename, _ = os.path.splitext(filename_with_ext)

    o = compiler(filename, filepath=directory)
    o.compile_library()