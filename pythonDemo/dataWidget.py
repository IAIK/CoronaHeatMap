# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '.\data_widget.ui'
#
# Created by: PyQt5 UI code generator 5.14.2
#
# WARNING! All changes made in this file will be lost!
import sqlite3

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtGui import QPalette


class Ui_Form(object):
    def setupUi(self, Form, window):
        Form.setObjectName("Form")
        Form.resize(1280, 720)
        self.window = window
        self.radio_state = {'manual': True, 'region': False}
        self.tableWidget = PatientTable(Form)
        self.tableWidget.setGeometry(QtCore.QRect(550, 170, 1301, 871))
        self.tableWidget.setMidLineWidth(0)
        self.tableWidget.setObjectName("DataTable")
        window.numbers = self.tableWidget.getData()
        self.label = QtWidgets.QLabel(Form)
        self.label.setGeometry(QtCore.QRect(920, 60, 601, 41))
        self.label.setObjectName("label")
        self.selection_group = QtWidgets.QGroupBox(Form)
        self.selection_group.setGeometry(QtCore.QRect(170, 230, 231, 161))
        self.selection_group.setFlat(False)
        self.selection_group.setCheckable(False)
        self.selection_group.setObjectName("selection_group")
        self.region_radio = QtWidgets.QRadioButton(self.selection_group)
        self.region_radio.setGeometry(QtCore.QRect(20, 50, 201, 17))
        self.region_radio.setObjectName("region_radio")
        self.region_radio.clicked.connect(self.selectRegionRadio)
        self.manual_radio = QtWidgets.QRadioButton(self.selection_group)
        self.manual_radio.setGeometry(QtCore.QRect(20, 100, 201, 31))
        self.manual_radio.setObjectName("manual_radio")
        self.manual_radio.clicked.connect(self.selectManualRadio)
        self.conf_selection = QtWidgets.QPushButton(Form)
        self.conf_selection.setGeometry(QtCore.QRect(40, 990, 201, 31))
        self.conf_selection.setObjectName("conf_selection")
        self.conf_selection.clicked.connect(self.confirmSelection)
        self.reset_selection = QtWidgets.QPushButton(Form)
        self.reset_selection.setGeometry(QtCore.QRect(290, 990, 201, 31))
        self.reset_selection.setObjectName("reset_selection")
        self.reset_selection.clicked.connect(self.clear)
        self.network_selection = QtWidgets.QPushButton(Form)
        self.network_selection.setGeometry(QtCore.QRect(170, 730, 201, 31))
        self.network_selection.setObjectName("Netzwek Einstellungen")
        self.network_selection.clicked.connect(self.networkSetting)
        self.accept_response = QtWidgets.QPushButton(Form)
        self.accept_response.setGeometry(QtCore.QRect(170, 800, 201, 31))
        self.accept_response.clicked.connect(self.acceptFromServer)
        self.county_selector = QtWidgets.QComboBox(Form)
        self.county_selector.setEnabled(True)
        self.county_selector.setGeometry(QtCore.QRect(170, 530, 201, 31))
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
        self.tableRatio =             {'obj': self.tableWidget,
                                       'xpos': 550/1920, 'ypos': 170/1080, 'xratio': 1301/1920, 'yratio': 871/1080}
        self.labelRatio =             {'obj': self.label,
                                       'xpos': 920/1920, 'ypos': 60/1080, 'xratio': 601 / 1920, 'yratio': 41 / 1080}
        self.selection_groupRatio =   {'obj': self.selection_group,
                                       'xpos': 150/1920, 'ypos': 230/1080, 'xratio': 300 / 1920, 'yratio': 161 / 1080}
        self.region_radioRatio =      {'obj': self.region_radio,
                                       'xpos': 20/1920, 'ypos': 50/1080, 'xratio': 270 / 1920, 'yratio': 40 / 1080}
        self.manual_radioRatio =      {'obj': self.manual_radio,
                                       'xpos': 20/1920, 'ypos': 100/1080, 'xratio': 270 / 1920, 'yratio': 40 / 1080}
        self.conf_selectionRatio =    {'obj': self.conf_selection,
                                       'xpos': 40/1920, 'ypos': 990/1080, 'xratio': 230 / 1920, 'yratio': 31 / 1080}
        self.reset_selectionRatio =   {'obj': self.reset_selection,
                                       'xpos': 290/1920, 'ypos': 990/1080, 'xratio': 230 / 1920, 'yratio': 31 / 1080}
        self.network_selectionRatio = {'obj': self.network_selection,
                                       'xpos': 150/1920, 'ypos': 730/1080, 'xratio': 270 / 1920, 'yratio': 31 / 1080}
        self.accept_responseRatio = {'obj': self.accept_response,
                                       'xpos': 150 / 1920, 'ypos': 800 / 1080, 'xratio': 270 / 1920,
                                       'yratio': 31 / 1080}
        self.county_selectorRatio =   {'obj': self.county_selector,
                                       'xpos': 150/1920, 'ypos': 530/1080, 'xratio': 270 / 1920, 'yratio': 31 / 1080}

        self.ratioList = [self.tableRatio, self.labelRatio, self.selection_groupRatio, self.region_radioRatio,
                          self.manual_radioRatio, self.conf_selectionRatio, self.reset_selectionRatio,
                          self.network_selectionRatio, self.accept_responseRatio, self.network_selectionRatio, self.county_selectorRatio]
        self.resize(Form.width(), Form.height())
        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(_translate("Form", "Form"))
        self.tableWidget.setToolTip(_translate("Form", "<html><head/><body><p>Dies ist die Datenbank der an Covid-19 erkrankten Personen. Sie können die gewünschten Einträge entweder nach Region oder manuell auswählen. Wenn Sie fertig sind klicken Sie auf \'Auswahl bestätigen\' um mit der Kommunikation zu beginnen.</p></body></html>"))
        self.label.setText('Wählen Sie die Daten zur Überprüfung aus')
        self.selection_group.setTitle("Auswahl Methode")
        self.region_radio.setText("Auswahl nach Region")
        self.manual_radio.setText("Manuelle Auswahl")
        self.conf_selection.setText("Auswahl bestätigen")
        self.network_selection.setText("Netzwerkeinstellungen")
        self.accept_response.setText("Antwort verarbeiten")
        self.reset_selection.setText("Auswahl zurücksetzen")
        self.county_selector.setItemText(0, "Burgenland")
        self.county_selector.setItemText(1, "Kärnten")
        self.county_selector.setItemText(2,  "Niederösterreich")
        self.county_selector.setItemText(3, "Oberösterreich")
        self.county_selector.setItemText(4, "Salzburg")
        self.county_selector.setItemText(5, "Steiermark")
        self.county_selector.setItemText(6, "Tirol")
        self.county_selector.setItemText(7, "Vorarlberg")
        self.county_selector.setItemText(8, "Wien")
        self.county_selector.setItemText(9, "Österreich")
        self.county_selector.currentIndexChanged.connect(self.regionChange)
        self.county_selector.setEnabled(False)
        self.network_selection.setEnabled(False)
        self.tableWidget.disableSelection()
        self.label.setAlignment(QtCore.Qt.AlignCenter)

    def resize(self, width, height):
        print("resizing to", width, height)
        for ratio in self.ratioList:
            ratio['obj'].setGeometry(width * ratio['xpos'], height * ratio['ypos'], width * ratio['xratio'], height * ratio['yratio'])
        w = self.tableWidget.geometry().x() + (self.tableWidget.geometry().width() // 2) - (self.label.width() // 2)
        self.label.setGeometry(w, height * self.labelRatio['ypos'], self.label.width(), self.label.height())  # This is done to assure label stays in the middle

    def clear(self):
        self.tableWidget.clearSelection()

    def regionChange(self):
        print('region ', self.radio_state['region'])
        if self.radio_state['region']:
            print('region changed')
            self.tableWidget.fetchRegion(self.county_selector.currentText())

    def selectRegionRadio(self):
        print('select region')
        self.county_selector.setEnabled(True)
        self.tableWidget.clearSelection()
        self.tableWidget.disableSelection()
        self.radio_state['region'] = True
        self.radio_state['manual'] = False

    def selectManualRadio(self):
        print('select manual')
        self.county_selector.setEnabled(False)
        self.tableWidget.clearSelection()
        self.tableWidget.enableSelection()
        self.radio_state['region'] = False
        self.radio_state['manual'] = True

    def networkSetting(self):
        self.window.openNetDialog()

    def acceptFromServer(self):
        self.window.acceptFromServer()

    def confirmSelection(self):
        self.window.generateOneHot(self.tableWidget.selectedNumbers())
        pass


class PatientTable(QtWidgets.QTableWidget):
    def __init__(self, parent=None):
        super(PatientTable, self).__init__(parent)
        self.setSelectionBehavior(QtWidgets.QTableWidget.SelectRows)
        self.initData('data/patients.sqlite')
        self.setColumnCount(len(self.data[0]))
        [self.horizontalHeader().setSectionResizeMode(i, QtWidgets.QHeaderView.Stretch) for i in range(len(self.data[0]))]
        self.setRowCount(0)
        for row_i, row_data in enumerate(self.data):
            self.insertRow(row_i)
            for col_i, col_data in enumerate(row_data):
                item = QtWidgets.QTableWidgetItem(str(col_data))
                item.setFlags(QtCore.Qt.ItemIsSelectable | QtCore.Qt.ItemIsEnabled)
                self.setItem(row_i, col_i, item)
        self.horizontalHeader().setHighlightSections(False)
        self.verticalHeader().setHighlightSections(False)
        self.verticalHeader().hide()
        self.setHorizontalHeaderLabels(['Name', 'Vorname', 'Provider-Vorwahl', 'Rufnummer', 'Bundesland'])
        self.providerCol = 2
        self.numberCol = 3
        self.enableSelection()
        palette = QPalette()
        palette.setColor(QPalette.Highlight, QtGui.QColor('#3a7fc2'))
        palette.setColor(QPalette.HighlightedText, QtGui.QColor("white"))
        self.setPalette(palette)

    def disableSelection(self):
        self.setSelectionMode(QtWidgets.QTableWidget.NoSelection)

    def enableSelection(self):
        self.setSelectionMode(QtWidgets.QTableWidget.MultiSelection)

    def initData(self, databasePath):
        con = sqlite3.connect(databasePath)
        curserObj = con.cursor()
        curserObj.execute('SELECT * FROM patients')
        self.data = curserObj.fetchall()
        con.close()

#   Returns list of all numbers plus provider (i.e. +43664  and 123456789)
    def getData(self):
        return [d[2] + d[3] for d in self.data]

    def fetchRegion(self, region):
        print("fetch ", region)
        self.enableSelection()
        self.clearSelection()
        if region == "Österreich":
            self.selectAll()
        else:
            items = self.findItems(region, QtCore.Qt.MatchExactly)
            for item in items:
                self.selectRow(item.row())
        self.disableSelection()

    def selectedNumbers(self):
        rows = self.selectionModel().selectedRows()

        numbers = []
        for row in rows:
            provider = self.item(row.row(), self.providerCol)
            no = self.item(row.row(), self.numberCol)
            number = provider.text() + no.text()
            numbers += [number]
        return numbers
