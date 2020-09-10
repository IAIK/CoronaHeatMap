import sys
from PyQt5.QtWidgets import QApplication, QWidget, QInputDialog, QLineEdit, QFileDialog
from PyQt5.QtGui import QIcon


class fileDialog(QWidget):

    def __init__(self, parent=None):
        super().__init__(parent)
        self.title = 'File Browser'
        self.left = 10
        self.top = 10
        self.width = 2640
        self.height = 480
        self.dialog = None
        #self.openFileNameDialog()
        #self.close()

    def openFileNameDialog(self):
        self.dialog = QFileDialog(self)
        options = self.dialog.Options()
        options |= self.dialog.DontUseNativeDialog
        fileName, _ = self.dialog.getOpenFileName(self, "Datei auswählen", "",
                                                  "All Files (*);;Python Files (*.py)", options=options)
        del self.dialog
        if fileName:
            print(fileName)
            return fileName
        return None

    def openFileNamesDialog(self):
        self.dialog = QFileDialog(self)
        options = self.dialog.Options()
        options |= self.dialog.DontUseNativeDialog
        files, _ = self.dialog.getOpenFileNames(self, "Dateien auswählen", "",
                                                "All Files (*);;Python Files (*.py)", options=options)
        del self.dialog
        if files:
            print(files)
            return files
        return None

    def saveFileDialog(self):
        self.dialog = QFileDialog(self)
        options = self.dialog.Options()
        options |= self.dialog.DontUseNativeDialog
        fileName, _ = self.dialog.getSaveFileName(self, "Speicherort wählen", "",
                                                  "All Files (*);;Text Files (*.txt)", options=options)
        del self.dialog
        if fileName:
            print(fileName)
            return fileName
        return None

    def openDirectoryDialog(self, header: str):
        file = str(QFileDialog.getExistingDirectory(self, header))

        if file:
            print(file)
            return file
        return None

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = fileDialog()
    sys.exit(app.exec_())