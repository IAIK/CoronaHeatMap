

import csv
import socket
import sqlite3
import sys

from PyQt5 import QtWidgets, QtCore
from PyQt5.QtWidgets import QApplication, QMainWindow

import startPrompt as sp
import dataWidget
import fileDialog
import networkDialog
import requestWidget
import radiusPrompt
from heatMap import HeatMapApp


def sql_fetch(databasePath):
    con = sqlite3.connect(databasePath)
    curserObj = con.cursor()
    curserObj.execute('SELECT * FROM patients')
    data = curserObj.fetchall()
    con.close()
    return data


def readCSV(csv_path, selector):
    with open(csv_path) as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        for row in csv_reader:
            for number in row:
                selector.numbers += [number]
    #selector.numbers = selector.numbers[:-1]


class DataSelectorWindow(QMainWindow):
    def __init__(self, parent=None):
        super(DataSelectorWindow, self).__init__(parent)
        self.numbers = []
        self.installEventFilter(self)
        self.resize(1280, 720)
        self.ip = 'localhost'
        self.port = 8081
        self.stacked = QtWidgets.QStackedWidget(self)
        self.stacked.resize(1280, 720)
        self.widgetDict = {}
        self.netDialog = QtWidgets.QDialog(self)
        self.net_ui = networkDialog.Ui_NetzwerkDialog()
        self.net_ui.setupUi(self.netDialog, self)

        self.requestDialog = requestWidget.RequestDialog(self)
        self.requestUI = requestWidget.Ui_Dialog()
        self.requestUI.setupUi(self.requestDialog, self)



        self.dataWidget = None
#        threading.Thread(target=readCSV, args=("numbers.csv", self)).start()

#        self.readCSV("numbers.csv")

    def start_plain(self):
        self.dataWidget.start_plain()
        self.stacked.setEnabled(True)

    def run_enc(self):
        self.dataWidget.run_plain.setEnabled(False)
        self.stacked.setEnabled(True)

    def eventFilter(self, obj, event):
        if event.type() == QtCore.QEvent.Resize:
            print("resize window")
            self.stacked.resize(self.width(), self.height())
            if self.dataWidget is not None:
                self.dataWidget.resize(self.width(), self.height())

        return super(DataSelectorWindow, self).eventFilter(obj, event)

    def addQWidget(self, data_widget, widget, name):
        self.dataWidget = data_widget
        self.stacked.addWidget(widget)
        self.widgetDict[name] = self.stacked.count() - 1

    def setQWidget(self, name):
        index = self.widgetDict[name]
        self.stacked.setCurrentIndex(index)

    def updateNetConf(self, ip, port):
        self.ip = ip
        self.port = port
        print('updating config')
        print("ip: ", ip, "port: ", port)

    def acceptFromServer(self):
        print("verarbeite Daten")
        fD = fileDialog.fileDialog()
        fileName = fD.openDirectoryDialog('Ordner zum speichern der Anfrage')
        if fileName is None:
            fileName = 'backend/'
        else:
            fileName += '/'
        del fD
        args = ["backend/client", "dec", fileName]
        self.requestUI.run_thread(args)
        self.requestDialog.show()
        self.stacked.setCurrentIndex(1)
        self.stacked.setEnabled(False)

    def run_plain(self):
        indices = []

        numbers = self.dataWidget.tableWidget.selectedNumbers()
        print(len(self.numbers))
        print(len(self.dataWidget.tableWidget.selectedNumbers()))
        for number in numbers:
            #            bisect.insort(self.numbers, str(number))
            indices += [self.numbers.index(str(number))]
        indices.sort()
        print(len(indices))
        #        indices += [0]
        index = 0

        hot_file = 'data/oneHot.csv'

        print(indices)
        print("writing file")
        with open(hot_file, "w") as csv_file:
            for n, i in enumerate(self.numbers):
                if i in numbers:
                    csv_file.write(str(1))
                else:
                    csv_file.write(str(0))
                #            for i in range(len(self.numbers)):
                #                if index < len(indices) and indices[index] == i:
                #                    csv_file.write(str(1))
                #                    index += 1
                #                else:
                #                    csv_file.write(str(0))
                if n != len(self.numbers) - 1:
                    csv_file.write(",")
        print("closed file")
        args = ["backend/plain"]
        self.requestUI.run_thread(args)
        self.requestDialog.show()
        self.stacked.setCurrentIndex(1)
        self.stacked.setEnabled(False)

    def connect(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((self.ip, self.port))
        self.conn = CustomSocket(self.sock)

    def openNetDialog(self):
        self.net_ui.retranslateUi(self.netDialog)
        self.netDialog.show()

    def closeNetDialog(self):
        self.netDialog.close()

    def generateOneHot(self, numbers):
        indices = []
        print(len(self.numbers))
        print(len(numbers))
        for number in numbers:
#            bisect.insort(self.numbers, str(number))
            indices += [self.numbers.index(str(number))]
        indices.sort()
        print(len(indices))
#        indices += [0]
        index = 0
        fD = fileDialog.fileDialog()
        fileName = fD.openDirectoryDialog('Ordner zum speichern der Anfrage')
        if fileName is None:
            fileName = ''
        else:
            fileName += '/'

        hot_file = 'data/oneHot.csv'

        print(indices)
        print("writing file")
        with open(hot_file, "w") as csv_file:
            for n, i in enumerate(self.numbers):
                if i in numbers:
                    csv_file.write(str(1))
                else:
                    csv_file.write(str(0))
#            for i in range(len(self.numbers)):
#                if index < len(indices) and indices[index] == i:
#                    csv_file.write(str(1))
#                    index += 1
#                else:
#                    csv_file.write(str(0))
                if n != len(self.numbers) - 1:
                    csv_file.write(",")
        print("closed file")
        print(fileName)
        args = ["backend/client", "enc", fileName, "data/"]
        self.requestUI.run_thread(args)
        self.requestDialog.show()
        self.stacked.setEnabled(False)





def main():
    print("main called")
    data = sql_fetch('data/patients.sqlite')
    print(len(data))
    if len(data) == 0:
        print("error fetching data")
        exit(1)
    app = QApplication(sys.argv)
    window = DataSelectorWindow()
    form_ui = QtWidgets.QWidget()

    ui_form = dataWidget.Ui_Form()
    ui_form.setupUi(form_ui, window, 10000)
    # dialog = QtWidgets.QDialog(window)
    # start_prompt = sp.Ui_Dialog()
    # start_prompt.setupUi(dialog, window)
    # window.stacked.setEnabled(False)
    # dialog.show()
    # window.setEnabled(True)
    heatMap = HeatMapApp(window)
    window.addQWidget(ui_form, form_ui, 'data')
    window.stacked.addWidget(heatMap)
#    window.openNetDialog()

    window.show()

#    ui_form.conf_selection.clicked.connect()
    sys.exit(app.exec_())
    pass


if __name__ == "__main__":
    main()
