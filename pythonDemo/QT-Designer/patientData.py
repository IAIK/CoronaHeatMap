# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '.\patient_data.ui'
#
# Created by: PyQt5 UI code generator 5.14.2
#
# WARNING! All changes made in this file will be lost!


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(1920, 1080)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.tableWidget = QtWidgets.QTableWidget(self.centralwidget)
        self.tableWidget.setGeometry(QtCore.QRect(530, 160, 1301, 851))
        self.tableWidget.setMidLineWidth(0)
        self.tableWidget.setObjectName("tableWidget")
        self.tableWidget.setColumnCount(0)
        self.tableWidget.setRowCount(0)
        self.reset_selection = QtWidgets.QPushButton(self.centralwidget)
        self.reset_selection.setGeometry(QtCore.QRect(290, 980, 201, 31))
        self.reset_selection.setObjectName("reset_selection")
        self.conf_selection = QtWidgets.QPushButton(self.centralwidget)
        self.conf_selection.setGeometry(QtCore.QRect(40, 980, 201, 31))
        self.conf_selection.setObjectName("conf_selection")
        self.conf_selection.clicked.connect(self.acceptSelection)
        self.label = QtWidgets.QLabel(self.centralwidget)
        self.label.setGeometry(QtCore.QRect(880, 60, 601, 41))
        self.label.setObjectName("label")
        self.county_selector = QtWidgets.QComboBox(self.centralwidget)
        self.county_selector.setEnabled(True)
        self.county_selector.setGeometry(QtCore.QRect(170, 520, 201, 31))
        self.county_selector.setCursor(QtGui.QCursor(QtCore.Qt.ArrowCursor))
        self.county_selector.setAutoFillBackground(False)
        self.county_selector.setEditable(False)
        self.county_selector.setDuplicatesEnabled(False)
        self.county_selector.setObjectName("county_selector")
        self.county_selector.addItem("")
        self.county_selector.addItem("")
        self.county_selector.addItem("")
        self.county_selector.addItem("")
        self.county_selector.addItem("")
        self.county_selector.addItem("")
        self.county_selector.addItem("")
        self.county_selector.addItem("")
        self.county_selector.addItem("")
        self.county_selector.addItem("")
        self.manual_radio = QtWidgets.QRadioButton(self.centralwidget)
        self.manual_radio.setGeometry(QtCore.QRect(190, 300, 201, 31))
        self.manual_radio.setObjectName("manual_radio")
        self.selection_group = QtWidgets.QGroupBox(self.centralwidget)
        self.selection_group.setGeometry(QtCore.QRect(170, 210, 231, 161))
        self.selection_group.setFlat(False)
        self.selection_group.setCheckable(False)
        self.selection_group.setObjectName("selection_group")
        self.region_radio = QtWidgets.QRadioButton(self.selection_group)
        self.region_radio.setGeometry(QtCore.QRect(20, 50, 201, 17))
        self.region_radio.setObjectName("region_radio")
        self.selection_group.raise_()
        self.tableWidget.raise_()
        self.reset_selection.raise_()
        self.conf_selection.raise_()
        self.label.raise_()
        self.county_selector.raise_()
        self.manual_radio.raise_()
        MainWindow.setCentralWidget(self.centralwidget)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.tableWidget.setToolTip(_translate("MainWindow", "<html><head/><body><p>Dies ist die Datenbank der an Covid-19 erkrankten Personen. Sie können die gewünschten Einträge entweder nach Region oder manuell auswählen. Wenn Sie fertig sind klicken Sie auf \'Auswahl bestätigen\' um mit der Kommunikation zu beginnen.</p></body></html>"))
        self.reset_selection.setText(_translate("MainWindow", "Auswahl zurücksetzen"))
        self.conf_selection.setText(_translate("MainWindow", "Auswahl bestätigen"))
        self.label.setText(_translate("MainWindow", "<html><head/><body><p><span style=\" font-size:22pt;\">Wählen Sie die Daten zur Überprüfung aus</span></p></body></html>"))
        self.county_selector.setItemText(0, _translate("MainWindow", "Burgenland"))
        self.county_selector.setItemText(1, _translate("MainWindow", "Kärnten"))
        self.county_selector.setItemText(2, _translate("MainWindow", "Niederösterreich"))
        self.county_selector.setItemText(3, _translate("MainWindow", "Oberösterreich"))
        self.county_selector.setItemText(4, _translate("MainWindow", "Salzburg"))
        self.county_selector.setItemText(5, _translate("MainWindow", "Steiermark"))
        self.county_selector.setItemText(6, _translate("MainWindow", "Tirol"))
        self.county_selector.setItemText(7, _translate("MainWindow", "Vorarlberg"))
        self.county_selector.setItemText(8, _translate("MainWindow", "Wien"))
        self.county_selector.setItemText(9, _translate("MainWindow", "Österreich"))
        self.manual_radio.setText(_translate("MainWindow", "Manuelle Auswahl"))
        self.selection_group.setTitle(_translate("MainWindow", "Auswahl Methode"))
        self.region_radio.setText(_translate("MainWindow", "Auswahl nach Region"))

    def acceptSelection(self):
        print("accept: ", self)


if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
