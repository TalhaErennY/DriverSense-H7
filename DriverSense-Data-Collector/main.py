import sys
from PyQt6.QtWidgets import QApplication
from app import CollectorWindow

if __name__ == '__main__':
    app = QApplication(sys.argv)
    app.setStyle('Fusion')
    win = CollectorWindow()
    win.show()
    sys.exit(app.exec())
