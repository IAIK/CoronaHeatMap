# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'QT-Designer/startPrompt.ui'
#
# Created by: PyQt5 UI code generator 5.14.2
#
# WARNING! All changes made in this file will be lost!


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_Dialog(object):
    def setupUi(self, Dialog, parent):
        Dialog.setObjectName("Dialog")
        Dialog.resize(490, 257)
        self.dialog = Dialog
        self.parent = parent
        self.RunPlain = QtWidgets.QPushButton(Dialog)
        self.RunPlain.setGeometry(QtCore.QRect(110, 90, 271, 25))
        self.RunPlain.setObjectName("RunPlain")
        self.RunEncrypted = QtWidgets.QPushButton(Dialog)
        self.RunEncrypted.setGeometry(QtCore.QRect(110, 160, 271, 25))
        self.RunEncrypted.setObjectName("runEncrypted")
        self.RunPlain.clicked.connect(self.runPlain)
        self.RunEncrypted.clicked.connect(self.runEncrypted)
        self.retranslateUi(Dialog)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def runPlain(self):
        print("run plain")
        self.parent.start_plain()
        self.dialog.accept()

    def runEncrypted(self):
        print("run enc")
        self.parent.run_enc()
        self.dialog.accept()

    def retranslateUi(self, Dialog):
        _translate = QtCore.QCoreApplication.translate
        Dialog.setWindowTitle(_translate("Dialog", "Dialog"))
        self.RunPlain.setText(_translate("Dialog", "Run Protocol in Plain"))
        self.RunEncrypted.setText(_translate("Dialog", "Run Encrypted Protocol"))
