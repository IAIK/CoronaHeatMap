# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'generate_request.ui'
#
# Created by: PyQt5 UI code generator 5.14.2
#
# WARNING! All changes made in this file will be lost!


from PyQt5 import QtCore, QtGui, QtWidgets
import threading
import subprocess
from functools import partial


class RequestDialog(QtWidgets.QDialog):
    def __init__(self, parent=None):
        super(RequestDialog, self).__init__(parent)
        self.installEventFilter(self)
        self.ui_dialog = None

    def eventFilter(self, obj, event):
        if event.type() == QtCore.QEvent.Close:
            print("close window")
            if self.ui_dialog is not None:
                self.ui_dialog.abort_button_pressed()

        return super(RequestDialog, self).eventFilter(obj, event)

class Ui_Dialog(object):
    def setupUi(self, Dialog, window):

        self.status_lock = threading.Lock()
        self.status_dict = {"finished": False, "Correct": False, "Error_code": "No errors detected"}
        self.dialog = Dialog
        self.dialog.ui_dialog = self
        self.dialog.setObjectName("Dialog")
        self.dialog.resize(464, 509)
        font = QtGui.QFont()
        font.setKerning(True)
        self.dialog.setFont(font)
        self.window = window
        self.buttonBox = QtWidgets.QDialogButtonBox(self.dialog)
        self.buttonBox.setGeometry(QtCore.QRect(370, 10, 81, 241))
        self.buttonBox.setOrientation(QtCore.Qt.Vertical)
        self.buttonBox.setStandardButtons(QtWidgets.QDialogButtonBox.Cancel| QtWidgets.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.textBrowser = QtWidgets.QTextBrowser(self.dialog)
        self.textBrowser.setGeometry(QtCore.QRect(10, 90, 441, 401))
        self.textBrowser.setObjectName("textBrowser")
        self.label = QtWidgets.QLabel(self.dialog)
        self.label.setGeometry(QtCore.QRect(16, 20, 341, 51))
        font = QtGui.QFont()
        font.setPointSize(22)
        font.setKerning(True)
        self.label.setFont(font)
        self.label.setTextFormat(QtCore.Qt.AutoText)
        self.label.setScaledContents(False)
        self.label.setAlignment(QtCore.Qt.AlignCenter)
        self.label.setIndent(0)
        self.label.setObjectName("label")

        self.buttonBox.accepted.connect(self.ok_button_pressed)
        self.buttonBox.rejected.connect(self.abort_button_pressed)
        QtCore.QMetaObject.connectSlotsByName(self.dialog)

    def retranslateUi(self, label: str):
        _translate = QtCore.QCoreApplication.translate
        self.dialog.setWindowTitle(_translate("Dialog", "Dialog"))
        self.textBrowser.setHtml(_translate("Dialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
                                            "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
                                            "p, li { white-space: pre-wrap; }\n"
                                            "</style></head><body style=\" font-family:\'Ubuntu\'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
                                            "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Starting Application ...</p></body></html>"))
        self.label.setText(_translate("Dialog", label))

    def run_thread(self, args: list):
        if len(args) > 1:
            string = 'Erzeuge Anfrage' if args[1] == 'enc' else 'Verarbeite Antwort'
        else:
            string = 'Verarbeite Anfrage'

        self.retranslateUi(string)
        self.request_thread = threading.Thread(target=execute_client_enc, args=(self, args))
        self.request_thread.start()

    def ok_button_pressed(self):
        self.window.stacked.setEnabled(True)
        self.dialog.accept()

    def abort_button_pressed(self):
        self.window.stacked.setEnabled(True)
        self.dialog.reject()

    def add_text(self, text):
        self.textBrowser.append(text)


def execute_client_enc(dialog: Ui_Dialog, args: list):

#   args = ["/home/fabian/Documents/privacy-preserving-disease-analysis/pythonDemo/client", "enc"]

    popen = subprocess.Popen(args, stdout=subprocess.PIPE)
    poll = 0
    for line in iter(popen.stdout.readline, ''):
        wrapper = partial(dialog.textBrowser.append, line.decode('utf-8'))
        QtCore.QTimer.singleShot(0, wrapper)
        poll = popen.poll()
        if poll is not None:
            break

    dialog.status_lock.acquire()
    dialog.status_dict['finished'] = True
    dialog.status_dict['Correct'] = True if poll == 0 else False
    string = "finished with exit code: " + str(poll)
    dialog.status_lock.release()
    QtCore.QTimer.singleShot(0, partial(dialog.textBrowser.append, string))
