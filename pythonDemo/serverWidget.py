# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '.\serverGUI.ui'
#
# Created by: PyQt5 UI code generator 5.14.2
#
# WARNING! All changes made in this file will be lost!


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName("Form")
        Form.resize(1280, 720)
        self.NetworkBtn = QtWidgets.QCommandLinkButton(Form)
        self.NetworkBtn.setObjectName("NetworkBtn")
        self.DatabaseBtn = QtWidgets.QCommandLinkButton(Form)
        self.DatabaseBtn.setObjectName("DatabaseBtn")
        self.label = QtWidgets.QLabel(Form)
        self.label.setObjectName("label")
        self.tabWidget = QtWidgets.QTabWidget(Form)
        self.tabWidget.setObjectName("tabWidget")
        self.tab = QtWidgets.QWidget()
        self.tab.setObjectName("tab")
        self.datapathLabel = QtWidgets.QLabel(self.tab)
        self.datapathLabel.setObjectName("datapathLabel")
        self.dataPathEdit = QtWidgets.QLineEdit(self.tab)
        self.dataPathEdit.setObjectName("dataPathEdit")
        self.statusHeadLabel = QtWidgets.QLabel(self.tab)
        self.statusHeadLabel.setObjectName("statusHeadLabel")
        self.statusOkLabel = QtWidgets.QLabel(self.tab)
        self.statusOkLabel.setObjectName("statusOkLabel")
        self.tabWidget.addTab(self.tab, "")
        self.tab_2 = QtWidgets.QWidget()
        self.tab_2.setObjectName("tab_2")
        self.statusOkLabelNet = QtWidgets.QLabel(self.tab_2)
        self.statusOkLabelNet.setObjectName("statusOkLabelNet")
        self.statusHeadLabelNet = QtWidgets.QLabel(self.tab_2)
        self.statusHeadLabelNet.setObjectName("statusHeadLabelNet")
        self.IPLabel = QtWidgets.QLabel(self.tab_2)
        self.IPLabel.setObjectName("IPLabel")
        self.PortLabel = QtWidgets.QLabel(self.tab_2)
        self.PortLabel.setObjectName("PortLabel")
        self.IPInfo = QtWidgets.QLabel(self.tab_2)
        self.IPInfo.setObjectName("IPInfo")
        self.PortInfo = QtWidgets.QLabel(self.tab_2)
        self.PortInfo.setObjectName("PortInfo")
        self.tabWidget.addTab(self.tab_2, "")
        self.sendNumBtn = QtWidgets.QPushButton(Form)
        self.sendNumBtn.setObjectName("sendNumBtn")
        self.ProcessRequestBtn = QtWidgets.QPushButton(Form)
        self.ProcessRequestBtn.setObjectName("HeatMapBtn")
        self.sendResultBtn = QtWidgets.QPushButton(Form)
        self.sendResultBtn.setObjectName("sendResultBtn")
        self.NetworkBtn.setGeometry(QtCore.QRect(1054, 650, 191, 41))
        self.DatabaseBtn.setGeometry(QtCore.QRect(830, 650, 191, 41))
        self.label.setGeometry(QtCore.QRect(460, 50, 361, 41))
        self.tabWidget.setGeometry(QtCore.QRect(840, 320, 401, 321))
        self.datapathLabel.setGeometry(QtCore.QRect(20, 40, 141, 16))
        self.dataPathEdit.setGeometry(QtCore.QRect(20, 80, 351, 20))
        self.statusHeadLabel.setGeometry(QtCore.QRect(20, 140, 180, 13))
        self.statusOkLabel.setGeometry(QtCore.QRect(20, 180, 180, 13))
        self.statusOkLabelNet.setGeometry(QtCore.QRect(40, 210, 180, 13))
        self.statusHeadLabelNet.setGeometry(QtCore.QRect(40, 170, 47, 13))
        self.IPLabel.setGeometry(QtCore.QRect(30, 30, 180, 16))
        self.PortLabel.setGeometry(QtCore.QRect(260, 30, 81, 16))
        self.IPInfo.setGeometry(QtCore.QRect(30, 70, 180, 13))
        self.PortInfo.setGeometry(QtCore.QRect(260, 70, 180, 13))
        self.sendNumBtn.setGeometry(QtCore.QRect(80, 330, 251, 31))
        self.radius_slider = QtWidgets.QSlider(QtCore.Qt.Horizontal, Form)
        self.privacy_level = QtWidgets.QSlider(QtCore.Qt.Horizontal, Form)

        self.radius_slider.setGeometry(QtCore.QRect(450, 330, 251, 31))
        self.radius_label = QtWidgets.QLabel(Form)
        self.radius_label.setGeometry(QtCore.QRect(450, 280, 251, 31))
        self.radius_label.setText("Senderadius")
        self.radius_label.setAlignment(QtCore.Qt.AlignCenter)
        self.privacy_level.setGeometry(QtCore.QRect(450, 450, 251, 31))
        self.privacy_label = QtWidgets.QLabel(Form)
        self.privacy_label.setGeometry(QtCore.QRect(450, 400, 251, 31))
        self.privacy_label.setText("Privacy level")
        self.radius_slider.valueChanged.connect(self.adjustRadiusLabel)
        self.privacy_level.valueChanged.connect(self.adjustPrivacyLabel)


        self.radius_value = QtWidgets.QLabel(Form)
        self.privacy_value = QtWidgets.QLabel(Form)
        self.radius_value.setGeometry(QtCore.QRect(450, 305, 251, 25))
        self.privacy_value.setGeometry(QtCore.QRect(450, 425, 251, 25))
        self.privacy_value.setAlignment(QtCore.Qt.AlignCenter)
        self.radius_value.setAlignment(QtCore.Qt.AlignCenter)

        self.privacy_label.setAlignment(QtCore.Qt.AlignCenter)
        self.ProcessRequestBtn.setGeometry(QtCore.QRect(80, 390, 251, 31))
        self.sendResultBtn.setGeometry(QtCore.QRect(80, 450, 251, 31))
        self.radius_slider.setMinimum(1)
        self.radius_slider.setMaximum(15)
        self.privacy_level.setMinimum(1)
        self.privacy_level.setMaximum(15)
        self.privacy_level.setTickPosition(QtWidgets.QSlider.TicksBelow)
        self.radius_slider.setTickPosition(QtWidgets.QSlider.TicksBelow)
        self.privacy_level.setTickInterval(1)
        self.radius_slider.setTickInterval(1)

        self.ratioList = []
        self.addToRatioList(self.radius_slider)
        self.addToRatioList(self.privacy_level)
        self.addToRatioList(self.NetworkBtn)
        self.addToRatioList(self.DatabaseBtn)
        self.addToRatioList(self.label)
        self.addToRatioList(self.tabWidget)
        self.addToRatioList(self.datapathLabel)
        self.addToRatioList(self.dataPathEdit)
        self.addToRatioList(self.statusHeadLabel)
        self.addToRatioList(self.statusHeadLabelNet)
        self.addToRatioList(self.statusOkLabel)
        self.addToRatioList(self.statusOkLabelNet)
        self.addToRatioList(self.IPInfo)
        self.addToRatioList(self.IPLabel)
        self.addToRatioList(self.PortLabel)
        self.addToRatioList(self.PortInfo)
        self.addToRatioList(self.sendNumBtn)
        self.addToRatioList(self.ProcessRequestBtn)
        self.addToRatioList(self.sendResultBtn)
        self.tabWidget.setTabEnabled(1, False)
        self.NetworkBtn.setEnabled(False)
        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def addToRatioList(self, obj: QtWidgets.QWidget):
        rect = obj.geometry()
        self.ratioList += [{'obj': obj, 'xpos': rect.x() / 1280, 'ypos': rect.y() / 720,
                           'xratio': rect.width() / 1280, 'yratio': rect.height() / 720}]

    def resize(self, width, height):
        print("resizing to", width, height)
        for ratio in self.ratioList:
            ratio['obj'].setGeometry(width * ratio['xpos'], height * ratio['ypos'], width * ratio['xratio'],
                                     height * ratio['yratio'])

    def adjustRadiusLabel(self):
        self.radius_value.setText(str(self.radius_slider.value()))

    def adjustPrivacyLabel(self):
        self.privacy_value.setText(str(self.privacy_level.value()))

    def retranslateUi(self, Form):
        _translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(_translate("Form", "Form"))
        self.NetworkBtn.setText(_translate("Form", "Netzerkeinstellungen"))
        self.DatabaseBtn.setText(_translate("Form", "Datenbank auswählen"))
        self.label.setText(_translate("Form", "<html><head/><body><p align=\"center\"><span style=\" font-size:24pt;\">HeatMap Austausch</span></p></body></html>"))
        self.datapathLabel.setText(_translate("Form", "Aktueller Pfad"))
        self.statusHeadLabel.setText(_translate("Form", "Status"))
        self.statusOkLabel.setText(_translate("Form", "TextLabel"))
        self.statusOkLabelNet.setText(_translate("Form", "TextLabel"))
        self.statusHeadLabelNet.setText(_translate("Form", "Status"))
        self.IPLabel.setText(_translate("Form", "IP Adresse"))
        self.PortLabel.setText(_translate("Form", "Port"))
        self.IPInfo.setText(_translate("Form", "TextLabel"))
        self.PortInfo.setText(_translate("Form", "TextLabel"))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_2), _translate("Form", "Verbindungseinstellungen"))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab), _translate("Form", "Datenbank Einstellungen"))
        self.sendNumBtn.setText(_translate("Form", "Nummernvektor exportieren"))
        self.ProcessRequestBtn.setText(_translate("Form", "Anfrage einlesen"))
        self.sendResultBtn.setText(_translate("Form", "Ergebnisse senden"))
        self.dataPathEdit.setReadOnly(True)
        self.dataPathEdit.setFrame(False)
        self.dataPathEdit.setPlaceholderText("Öffnen Sie das Datenbank Menü")
        self.statusOkLabel.setText("Nicht verbunden")
        self.IPInfo.setText("localhost")
        self.PortInfo.setText("8081")
        self.statusOkLabelNet.setText("Nicht verbunden")


if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    Form = QtWidgets.QWidget()
    ui = Ui_Form()
    ui.setupUi(Form)
    Form.show()
    sys.exit(app.exec_())
