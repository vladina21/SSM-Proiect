from PySide6.QtWidgets import QMainWindow, QHBoxLayout, QVBoxLayout, QWidget, QGroupBox, QLabel, QPushButton, QLineEdit, QTextEdit, QDialog, QFormLayout
from PySide6.QtGui import QIcon, QColor, QFont, QTextCharFormat
#from PySide6.QtCore import Qt
import pyqtgraph as pg

import serial
import datetime

class ConnectDialog(QDialog):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Connect to Board")
        self.setWindowIcon(QIcon("./icon.png"))
        self.setFixedSize(200, 110)

        layout = QFormLayout()

        self.serial_port_edit = QLineEdit()
        self.serial_port_edit.setPlaceholderText("Enter Serial Port")
        layout.addRow(QLabel("Serial Port:"), self.serial_port_edit)

        layout.setSpacing(10)

        self.baud_rate_edit = QLineEdit()
        self.baud_rate_edit.setPlaceholderText("Enter Baud Rate")
        layout.addRow(QLabel("Baud Rate:"), self.baud_rate_edit)

        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.accept)
        layout.addRow(self.connect_button)

        self.setLayout(layout)

class MainWindow(QMainWindow):
    promotie: str = "2023-2024"
    team: list[str] = [
        "Surdu Vladina",
        "Bota Iulia",
    ]

    serial = 0

    counter = 0
    is_reading = 0
    data_read_timer = 500

    button_connect_board = 0
    button_start_led_sequence = 0
    button_stop_led_sequence = 0
    button_reverse_led_sequence = 0
    button_start_reading_sensor_data = 0
    button_stop_reading_sensor_data = 0
    button_terminate_connection = 0

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

        self.button_connect_board = QPushButton("Connect to board")
        self.button_connect_board.clicked.connect(self.connect_board)

        self.button_start_led_sequence = QPushButton("Start led sequence")
        self.button_start_led_sequence.clicked.connect(self.start_led_sequence)
        self.button_start_led_sequence.setEnabled(False)

        self.button_stop_led_sequence = QPushButton("Stop led sequence")
        self.button_stop_led_sequence.clicked.connect(self.stop_led_sequence)
        self.button_stop_led_sequence.setVisible(False)

        self.button_reverse_led_sequence = QPushButton("Reverse led sequence")
        self.button_reverse_led_sequence.clicked.connect(self.reverse_led_sequence)
        self.button_reverse_led_sequence.setEnabled(False)

        self.button_start_reading_sensor_data = QPushButton("Start reading sensor data")
        self.button_start_reading_sensor_data.clicked.connect(self.start_reading_sensor_data)
        self.button_start_reading_sensor_data.setEnabled(False)

        self.button_stop_reading_sensor_data = QPushButton("Stop reading sensor data")
        self.button_stop_reading_sensor_data.clicked.connect(self.stop_reading_sensor_data)
        self.button_stop_reading_sensor_data.setVisible(False)

        self.button_terminate_connection = QPushButton("Terminate connection")
        self.button_terminate_connection.clicked.connect(self.terminate_connection)
        self.button_terminate_connection.setEnabled(False)

        control_panel_box_layout = QVBoxLayout()
        control_panel_box_layout.setSpacing(5)
        control_panel_box_layout.addWidget(self.button_connect_board,1)
        control_panel_box_layout.addWidget(self.button_start_led_sequence,1)
        control_panel_box_layout.addWidget(self.button_stop_led_sequence,1)
        control_panel_box_layout.addWidget(self.button_reverse_led_sequence,1)
        control_panel_box_layout.addWidget(self.button_start_reading_sensor_data,1)
        control_panel_box_layout.addWidget(self.button_stop_reading_sensor_data,1)
        control_panel_box_layout.addWidget(self.button_terminate_connection,1)

        control_panel_box_layout.addStretch()

        control_panel_box.setLayout(control_panel_box_layout)

        tertiary_layout.addWidget(team_box, 1)
        tertiary_layout.addWidget(control_panel_box,5)

        # graph
 
        self.plot_widget = pg.PlotWidget()
        # Set labels for the axes
        self.plot_widget.setLabel('left','Light Intensity')  # Label for the left axis
        self.plot_widget.setLabel('bottom','Time (s)')  # Label for the bottom axis

        # Set limits for the y-axis (left axis)
        self.plot_widget.setLimits(xMin=0, yMin=0, yMax=255)

        secondary_layout.addWidget(self.plot_widget, 3)
        secondary_layout.addLayout(tertiary_layout, 1)

        primary_layout.addLayout(secondary_layout, 6)
        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)

        # debug box

        debug_box = QGroupBox("Debug")
        debug_box_layout = QVBoxLayout()
        debug_box_layout.addWidget(self.text_edit, 1)
        debug_box.setLayout(debug_box_layout)

        primary_layout.addWidget(debug_box, 2)

        widget = QWidget()
        widget.setLayout(primary_layout)
        
        self.setCentralWidget(widget)

        #timer

        self.timer = pg.QtCore.QTimer(self)
        self.timer.timeout.connect(self.update_graph)
        self.timer.start(self.data_read_timer)

    def parse_sensor_data(self,data):
        lines = data.split('\n') 
        sensor_values = []
    
        for line in lines:
            if line.startswith('Sensor: ') and len(line) >= 10:
                value_str = line[8:11]
                if value_str.isdigit():
                    value = int(value_str)
                    sensor_values.append(value)

        if sensor_values:
            mean = sum(sensor_values) / len(sensor_values)
            return mean

        return 0

    def check_serial(self):
        data = self.serial.read_all().decode('utf-8').strip()
        if data:
            mean = self.parse_sensor_data(data)
            return mean
        return 0


    def scale_value(self, input_value, min_input, max_input, min_output, max_output):
        scaled_value = ((input_value - min_input) / (max_input - min_input)) * (max_output - min_output) + min_output
        return int(120 - scaled_value)

    def update_graph(self):
        if self.is_reading:
            data = self.check_serial()

            if data < 20:
                brush_color = 'g'
            elif data > 235:
                brush_color = 'r'
            else:
                hue = self.scale_value(data, 20, 235, 0, 120)
                brush_color = QColor.fromHslF(hue/360, 1.0, 0.5)

            bar_item = pg.BarGraphItem(x=[self.counter], height=[data], width=self.data_read_timer/1000, brush=brush_color)
            self.plot_widget.addItem(bar_item)


        self.counter += self.data_read_timer/1000
        

    def print_info(self, info_msg):
        cursor = self.text_edit.textCursor()

        info_format = QTextCharFormat()
        info_format.setForeground(QColor(0,0,235))
        info_format.setFontWeight(QFont.Bold)
        info_format.setFontPointSize(11)
        cursor.insertText(f"{self.get_timestamp()} INFO: ", info_format)

        text_format = QTextCharFormat()
        text_format.setForeground(QColor(0,0,235))
        text_format.setFontPointSize(11)
        cursor.insertText(info_msg, text_format)

    def print_info_success(self, info_msg):
        cursor = self.text_edit.textCursor()

        info_format = QTextCharFormat()
        info_format.setForeground(QColor(0,150,70))
        info_format.setFontWeight(QFont.Bold)
        info_format.setFontPointSize(11)
        cursor.insertText(f"{self.get_timestamp()} INFO: ", info_format)

        text_format = QTextCharFormat()
        text_format.setForeground(QColor(0,150,70))
        text_format.setFontPointSize(11)
        cursor.insertText(info_msg, text_format)

    def print_error(self, error_msg):
        cursor = self.text_edit.textCursor()

        error_format = QTextCharFormat()
        error_format.setForeground(QColor(235,0,0))
        error_format.setFontWeight(QFont.Bold)
        error_format.setFontPointSize(11)
        cursor.insertText(f"{self.get_timestamp()} ERROR: ", error_format)

        text_format = QTextCharFormat()
        text_format.setForeground(QColor(235,0,0))
        text_format.setFontPointSize(11)
        cursor.insertText(error_msg, text_format)

    def get_timestamp(self):
        return datetime.datetime.now().strftime("[%H:%M]")


    def connect_board(self):
        dialog = ConnectDialog()
        result = dialog.exec_()

        if result == QDialog.Accepted:
            self.print_info("Connecting to board\n")

            serial_port = dialog.serial_port_edit.text()
            baud_rate = dialog.baud_rate_edit.text()
            if not baud_rate:
                baud_rate = '0'

            try:
                self.serial = serial.Serial(port=serial_port, baudrate=int(baud_rate), bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, timeout=1)
            except serial.SerialException as e:
                self.print_error(f"Couldn't connect to board. Reason: {str(e)}\n")
                return

            self.print_info_success("Connection to board succesful\n")
            self.sender().setEnabled(False)
            self.button_start_led_sequence.setVisible(True)
            self.button_start_led_sequence.setEnabled(True)
            self.button_stop_led_sequence.setVisible(False)
            self.button_stop_led_sequence.setEnabled(True)
            self.button_start_reading_sensor_data.setVisible(True)
            self.button_start_reading_sensor_data.setEnabled(True)
            self.button_stop_reading_sensor_data.setVisible(False)
            self.button_terminate_connection.setEnabled(True)

    def terminate_connection(self):
        self.print_info("Disconnecting from board\n")
        try:
            self.serial.close()
            
        except serial.SerialException as e:
            self.print_error(f"Couldn't disconnect from board. Reason: {str(e)}\n")
            return
        
        self.print_info_success("Disconnection from board succesful\n")
        self.button_connect_board.setEnabled(True)
        self.button_start_led_sequence.setVisible(True)
        self.button_start_led_sequence.setEnabled(False)
        self.button_stop_led_sequence.setVisible(False)
        self.button_stop_led_sequence.setEnabled(False)
        self.button_reverse_led_sequence.setEnabled(False)
        self.button_start_reading_sensor_data.setVisible(True)
        self.button_start_reading_sensor_data.setEnabled(False)
        self.button_stop_reading_sensor_data.setVisible(False)
        self.button_stop_reading_sensor_data.setEnabled(False)
        self.sender().setEnabled(False)

    def start_led_sequence(self):
        self.print_info("Starting led sequence\n")
        self.sender().setVisible(False)
        self.button_stop_led_sequence.setVisible(True)
        self.button_reverse_led_sequence.setEnabled(True)
        self.serial.write(b'1')
 
    def stop_led_sequence(self):
        self.print_info("Stopping led sequence\n")
        self.button_start_led_sequence.setVisible(True)
        self.sender().setVisible(False)
        self.button_reverse_led_sequence.setEnabled(False)
        self.serial.write(b'2')

    def reverse_led_sequence(self):
        self.print_info("Reversing led sequence\n")
        self.serial.write(b'3')

    def start_reading_sensor_data(self):
        self.print_info("Started reading sensor data\n")
        self.sender().setVisible(False)
        self.button_stop_reading_sensor_data.setVisible(True)
        self.button_stop_reading_sensor_data.setEnabled(True)
        self.is_reading = 1
        self.serial.write(b'4')

    def stop_reading_sensor_data(self):
        self.print_info("Stopped reading sensor data\n")
        self.button_start_reading_sensor_data.setVisible(True)
        self.sender().setVisible(False)
        self.is_reading = 0
        self.serial.write(b'5')


            