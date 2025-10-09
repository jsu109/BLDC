import sys
import re
import serial
from collections import deque
from PyQt5 import QtWidgets
import pyqtgraph as pg

# ---- Serial config ----
PORT = "/dev/tty.usbmodem101"   # change to your port
BAUD = 115200

# ---- Data capture config ----
MAX_POINTS = 300  # history length

# regex to extract floats/ints from your printf format
pattern = re.compile(
    r"target:\s*([-\d\.]+),\s*vel:\s*([-\d\.]+),\s*err:\s*([-\d\.]+),\s*Iq:\s*([-\d\.]+),\s*theta:\s*([-\d\.]+),\s*angle:\s*([-\d\.]+),\s*duties U:(\d+)\s*V:(\d+)\s*W:(\d+)"
)

class SerialPlotter(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()

        self.ser = serial.Serial(PORT, BAUD, timeout=0.1)

        # storage deque for target only
        self.data = {
            "target": deque(maxlen=MAX_POINTS),
        }

        # Central widget and layout
        central_widget = QtWidgets.QWidget()
        self.setCentralWidget(central_widget)
        layout = QtWidgets.QVBoxLayout()
        central_widget.setLayout(layout)

        # PyQtGraph setup
        self.graphWidget = pg.GraphicsLayoutWidget()
        layout.addWidget(self.graphWidget)

        self.plot1 = self.graphWidget.addPlot(title="commanded")
        self.curve_target = self.plot1.plot(pen="y", name="target")
        self.graphWidget.nextRow()

        # QLineEdit for user input
        self.input_line = QtWidgets.QLineEdit()
        self.input_line.setPlaceholderText("Enter command and press Enter")
        layout.addWidget(self.input_line)
        self.input_line.returnPressed.connect(self.send_command)

        self.timer = pg.QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(11)  # 1000 Hz update

    def send_command(self):
        text = self.input_line.text()
        if text:
            try:
                self.ser.write((text + '\n').encode())  # Send command with newline
            except Exception as e:
                print(f"Error sending command: {e}")
            self.input_line.clear()

    def update(self):
        try:
            line = self.ser.readline().decode(errors="ignore").strip()
            if line.startswith("target:"):
                try:
                    # Extract the value after 'target:' and before the first comma
                    target_str = line.split(",")[0].split(":")[1].strip()
                    target_val = float(target_str)
                    self.data["target"].append(target_val)
                    self.curve_target.setData(self.data["target"])
                except ValueError:
                    pass
        except Exception as e:
            print(f"Serial read error: {e}")


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    win = SerialPlotter()
    win.show()
    sys.exit(app.exec_())