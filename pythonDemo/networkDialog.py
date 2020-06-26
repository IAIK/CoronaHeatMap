# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '.\networkDialog.ui'
#
# Created by: PyQt5 UI code generator 5.14.2
#
# WARNING! All changes made in this file will be lost!


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_NetzwerkDialog(object):
    def setupUi(self, NetzwerkDialog, window):
        print("ui netzwerkdialog")
        NetzwerkDialog.setObjectName("NetzwerkDialog")
        NetzwerkDialog.resize(400, 300)
        self.defaultIP = 'localhost'
        self.defaultPort = 8081
        self.window = window
        self.Buttons = QtWidgets.QDialogButtonBox(NetzwerkDialog)
        self.Buttons.setGeometry(QtCore.QRect(30, 240, 341, 32))
        self.Buttons.setOrientation(QtCore.Qt.Horizontal)
        self.Buttons.setStandardButtons(QtWidgets.QDialogButtonBox.Cancel|QtWidgets.QDialogButtonBox.Ok|QtWidgets.QDialogButtonBox.RestoreDefaults)
        self.Buttons.setObjectName("Buttons")
        self.Buttons.button(QtWidgets.QDialogButtonBox.RestoreDefaults).setText("Zurücksetzen")
        self.IPInput = QtWidgets.QLineEdit(NetzwerkDialog)
        self.IPInput.setGeometry(QtCore.QRect(30, 90, 161, 31))
        self.IPInput.setClearButtonEnabled(False)
        self.IPInput.setObjectName("IPInput")
        self.PortInput = QtWidgets.QLineEdit(NetzwerkDialog)
        self.PortInput.setGeometry(QtCore.QRect(30, 160, 161, 31))
        self.PortInput.setObjectName("PortInput")
        self.currentIPHead = QtWidgets.QLabel(NetzwerkDialog)
        self.currentIPHead.setGeometry(QtCore.QRect(270, 80, 91, 20))
        self.currentIPHead.setObjectName("currentIPHead")
        self.currentIPBody = QtWidgets.QLabel(NetzwerkDialog)
        self.currentIPBody.setGeometry(QtCore.QRect(290, 110, 47, 13))
        self.currentIPBody.setObjectName("currentIPBody")
        self.currentPortHead = QtWidgets.QLabel(NetzwerkDialog)
        self.currentPortHead.setGeometry(QtCore.QRect(260, 150, 111, 20))
        self.currentPortHead.setObjectName("currentPortHead")
        self.currentPortBody = QtWidgets.QLabel(NetzwerkDialog)
        self.currentPortBody.setGeometry(QtCore.QRect(290, 180, 47, 13))
        self.currentPortBody.setObjectName("currentPortBody")
        self.Header = QtWidgets.QLabel(NetzwerkDialog)
        self.Header.setGeometry(QtCore.QRect(30, 20, 351, 31))
        self.Header.setObjectName("Header")

        self.retranslateUi(NetzwerkDialog)
        self.Buttons.rejected.connect(self.cancelBtn)
        self.Buttons.accepted.connect(self.acceptBtn)
        self.Buttons.button(QtWidgets.QDialogButtonBox.RestoreDefaults).clicked.connect(self.restore)
        QtCore.QMetaObject.connectSlotsByName(NetzwerkDialog)

    def retranslateUi(self, NetzwerkDialog):
        _translate = QtCore.QCoreApplication.translate
        NetzwerkDialog.setWindowTitle(_translate("NetzwerkDialog", "Netzwerk Einstellungen"))
        self.IPInput.setText("")
        self.PortInput.setText("")
        self.IPInput.setPlaceholderText(_translate("NetzwerkDialog", "IP Adresse eingeben"))
        self.PortInput.setPlaceholderText(_translate("NetzwerkDialog", "Port eingeben"))
        self.currentIPHead.setText(_translate("NetzwerkDialog", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">Aktuelle IP</span></p></body></html>"))
        self.currentIPBody.setText(_translate("NetzwerkDialog", "<html><head/><body><p align=\"center\">" + self.window.ip + "</p></body></html>"))
        self.currentPortHead.setText(_translate("NetzwerkDialog", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">Aktueller Port</span></p></body></html>"))
        self.currentPortBody.setText(_translate("NetzwerkDialog", "<html><head/><body><p align=\"center\">" + str(self.window.port) + "</p><p align=\"center\"><br/></p></body></html>"))
        self.Header.setText(_translate("NetzwerkDialog", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">Wählen Sie IP und Port des Empfängers</span></p><p align=\"center\"><br/></p></body></html>"))

    def cancelBtn(self):
        self.window.closeNetDialog()

    def acceptBtn(self):
        print("accept")
        ip = self.defaultIP
        if self.IPInput.text() != "":
            ip = self.IPInput.text()
        port = self.defaultPort
        if self.PortInput.text() != "":
            port = self.PortInput.text()
        self.window.updateNetConf(ip, port)
        self.window.closeNetDialog()

    def restore(self):
        print("restore")
        self.window.updateNetConf(self.defaultIP, self.defaultPort)
        self.retranslateUi(self.window.netDialog)

'''

import sys
app = QtWidgets.QApplication(sys.argv)
NetzwerkDialog = QtWidgets.QDialog()
ui = Ui_NetzwerkDialog()
ui.setupUi(NetzwerkDialog)
NetzwerkDialog.show()
sys.exit(app.exec_())

'''