from PySide6.QtWidgets import QMainWindow, QHBoxLayout, QVBoxLayout, QWidget, QGroupBox, QLabel, QPushButton, QLineEdit, QTextEdit
from PySide6.QtGui import QIcon, QPalette, QColor, QFont
from PySide6.QtCore import Qt
import pyqtgraph as pg
import random


class MainWindow(QMainWindow):
    promotie: str = "2023-2024"
    team: list[str] = [
        "Surdu Vladina",
        "Bota Iulia",
    ]

    counter = 0
    plot_widget = 0
    random_val = 0

    def __init__(self):
        super().__init__()
        self.setWindowTitle(f"Proiect Microprocesoare {self.promotie}")
        self.setWindowIcon(QIcon("./icon.png"))
        

        primary_layout = QVBoxLayout()
        secondary_layout = QHBoxLayout()
        tertiary_layout = QVBoxLayout()

        # team box

        team_box = QGroupBox("Membrii echipei")
        bold_font = QFont()
        bold_font.setBold(True)
        team_box.setFont(bold_font)

        first_member = QLabel(f"Membru 1: {self.team[0]}")
        second_member = QLabel(f"Membru 2: {self.team[1]}")
        team_box_layout = QVBoxLayout()
        team_box_layout.addWidget(first_member,1)
        team_box_layout.addWidget(second_member,1)
        team_box.setLayout(team_box_layout)

        # control panel box

        control_panel_box = QGroupBox("Control Panel")
        control_panel_box.setFont(bold_font)

        button1 = QPushButton("Control 1")
        button2 = QPushButton("Control 2")
        button3 = QPushButton("Send")
        button3.clicked.connect(self.send_input)
        self.line_edit = QLineEdit()
        self.line_edit.setAlignment(Qt.AlignmentFlag.AlignBottom)
        line_edit_label = QLabel("Input:", parent=self.line_edit)
        control_panel_box_layout = QVBoxLayout()
        control_panel_box_layout.setSpacing(5)
        control_panel_box_layout.addWidget(button1,1)
        control_panel_box_layout.addWidget(button2,1)

        control_panel_box_layout.addStretch()
        control_panel_box_layout.addWidget(line_edit_label)
        control_panel_box_layout.addWidget(self.line_edit, 1)
        control_panel_box_layout.addWidget(button3,1)

        control_panel_box.setLayout(control_panel_box_layout)

        # graph

        tertiary_layout.addWidget(team_box, 1)
        tertiary_layout.addWidget(control_panel_box,5)
 
        self.plot_widget = pg.PlotWidget()
        #plot_widget.plot(second, light_intensity)
        # Set labels for the axes
        self.plot_widget.setLabel('left','Light Intensity')  # Label for the left axis
        self.plot_widget.setLabel('bottom','Time (s)')  # Label for the bottom axis

        # Set limits for the y-axis (left axis)
        self.plot_widget.setLimits(xMin=0, yMin=0, yMax=255)

        secondary_layout.addWidget(self.plot_widget, 3)
        secondary_layout.addLayout(tertiary_layout, 1)

        primary_layout.addLayout(secondary_layout, 4)
        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)

        # debug box

        debug_box = QGroupBox("Debug")
        debug_box_layout = QVBoxLayout()
        debug_box_layout.addWidget(self.text_edit, 1)
        debug_box.setLayout(debug_box_layout)

        primary_layout.addWidget(debug_box, 1)

        widget = QWidget()
        widget.setLayout(primary_layout)
        
        self.setCentralWidget(widget)

        self.timer = pg.QtCore.QTimer(self)
        self.timer.timeout.connect(self.update_graph)
        self.timer.start(50)  # Update every 1000 milliseconds (1 second)

    def scale_value(self, input_value, min_input, max_input, min_output, max_output):
        scaled_value = ((input_value - min_input) / (max_input - min_input)) * (max_output - min_output) + min_output
        return int(120 - scaled_value)

    def update_graph(self):
        # Update the bar graph data with random values for demonstration purposes

        new_data = random.uniform(self.random_val - 30, self.random_val + 30)
        if new_data < 0:
            new_data = 0
        elif new_data > 255:
            new_data = 255
    
        #if new_data < 84:
        #    # Green for values below 84
        #    brush_color = QColor.fromHslF(120/360, 1.0, 0.5)  # HSL for green
        #elif 84 <= new_data <= 166:
        #    # Yellow for values between 84 and 166
        #    brush_color = QColor.fromHslF(60/360, 1.0, 0.5)  # HSL for yellow
        #else:
        #    # Red for values above 166
        #    brush_color = QColor.fromHslF(0/360, 1.0, 0.5)  # HSL for red

        if new_data < 20:
            brush_color = 'g'
        elif new_data > 235:
            brush_color = 'r'
        else:
            hue = self.scale_value(new_data, 20, 235, 0, 120)
            brush_color = QColor.fromHslF(hue/360, 1.0, 0.5)

        bar_item = pg.BarGraphItem(x=[self.counter], height=[new_data], width=1, brush=brush_color)
        self.plot_widget.addItem(bar_item)
        self.counter += 1
        self.random_val = new_data
        

    def send_input(self):
        input = self.line_edit.text()
        self.line_edit.clear()
        self.text_edit.insertPlainText(f"INPUT: {input}\n")