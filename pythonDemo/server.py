import sys

from PyQt5 import QtWidgets, QtCore
from PyQt5.QtWidgets import QMainWindow, QApplication

import requestWidget
import fileDialog


import serverWidget


class ServerWindow(QMainWindow):
    def __init__(self, parent=None):
        super(ServerWindow, self).__init__(parent)
        self.installEventFilter(self)
        self.resize(1280, 720)
        self.ip = 'localhost'
        self.port = 8081
        self.stacked = QtWidgets.QStackedWidget(self)
        self.stacked.resize(1280, 720)

        self.requestDialog = requestWidget.RequestDialog(self)
        self.requestUI = requestWidget.Ui_Dialog()

        self.main_window = None

    def setMainWindow(self, widget, qwidget):
        self.main_window = widget
        self.main_window.ProcessRequestBtn.clicked.connect(self.ProcessRequest)
        self.main_window.sendNumBtn.clicked.connect(self.numberVector)
        self.main_window.sendResultBtn.clicked.connect(self.sendResults)
        self.main_window.DatabaseBtn.clicked.connect(self.dataBasePrompt)
        self.main_window.NetworkBtn.clicked.connect(self.netSettings)
        self.stacked.addWidget(qwidget)

    def eventFilter(self, obj, event):
        if event.type() == QtCore.QEvent.Resize:
            self.stacked.resize(self.width(), self.height())
            if self.main_window is not None:
                self.main_window.resize(self.width(), self.height())
        return super(ServerWindow, self).eventFilter(obj, event)

    def numberVector(self):
        prefix = "+43664"
        numGen = phoneNumberGen(32741)
        with open("data/numbers.csv", "w") as f:
            for n in numGen:
                f.write(prefix)
                f.write(str(n).zfill(8))
                f.write(",")
            f.write(prefix)
            f.write(str(99999999))
        print("numbers")
        self.main_window.sendNumBtn.setEnabled(False)
        pass

    def ProcessRequest(self):
        fD = fileDialog.fileDialog()
        fileName = fD.openDirectoryDialog('Ordner zum einlesen der Anfrage')
        if fileName is None:
            fileName = 'backend/'
        else:
            fileName += '/'
        args = ["backend/server",
                str(self.main_window.radius_slider.value()),
                str(self.main_window.privacy_level.value()),
                fileName,
                fileName
                ]
        self.requestUI.setupUi(self.requestDialog, self)
        self.requestUI.run_thread(args)
        self.requestDialog.show()
        self.stacked.setEnabled(False)
        pass

    def sendResults(self):
        print("send results")
        pass

    def dataBasePrompt(self):
        print("databasePrompt")
        pass

    def netSettings(self):
        print("Net Settings")
        pass


def phoneNumberGen(amount):
    n, counter = 0, 0
    step = 100000000 // amount
    while True:
        if counter >= amount:
            return
        yield n
        n += step
        counter += 1

def main():
    app = QApplication(sys.argv)
    window = ServerWindow()
    widget = QtWidgets.QWidget()
    server_widget = serverWidget.Ui_Form()
    server_widget.setupUi(widget)
    window.setMainWindow(server_widget, widget)
    window.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
