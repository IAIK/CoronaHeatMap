import folium
import folium.plugins
import numpy as np
import sqlite3
import random
import json
import sys
import csv
import io
from PyQt5 import QtWidgets
from PyQt5.QtWebEngineWidgets import QWebEngineView
from PyQt5.QtWidgets import QApplication, QMainWindow, QComboBox, QWidget
from PyQt5.QtGui import QGuiApplication
from PyQt5.QtCore import QUrl, QDir, QByteArray

mp = [[(48.23092327, 16.2342154), (48.17329879, 16.28029131),  (48.2087746264, 16.3459915059), (48.2150387468, 16.3417482757), (48.1917780919, 16.2813026064)], [(48.2125952684, 16.3760091526), (48.2150387468, 16.3417482757), (48.1754953891, 16.2814870428), (48.1919481, 16.3181166), (48.2199625667, 16.22764564)], [(48.2087746264, 16.3459915059), (48.208144, 16.395962), (48.225019475, 16.404980425)], [(48.1492502924, 16.3523680702), (48.208144, 16.395962)]]
constraint = [(48.323266, 16.418854), (48.151078, 16.578156), (48.119001, 16.311394), (48.222951, 16.185051)]
viennaLocation = [48.225267, 16.383716]
locationDict = {'Wien': {'location': [48.225267, 16.383716], 'zoom': 12},
                'Steiermark': {'location': [47.2195061, 14.8067803], 'zoom': 9},
                'Kaernten': {'location': [46.7519336, 13.8805949], 'zoom': 9},
                'Tirol': {'location': [47.1968643, 11.4718003], 'zoom': 9},
                'Vorarlberg': {'location': [47.2186028, 9.9036666], 'zoom': 10},
                'Salzburg': {'location': [47.4924628, 13.05643], 'zoom': 9},
                'Oberoesterreich': {'location': [48.1163299, 13.8098937], 'zoom': 9},
                'Niederoesterreich': {'location': [48.2208476, 15.6999241], 'zoom': 9},
                'Burgenland': {'location': [47.4741886, 16.5182386], 'zoom': 9},
                'Oesterreich': {'location': [47.6928632, 13.29], 'zoom': 8}}

def sql_fetch(databasePath):
    con = sqlite3.connect(databasePath)
    curserObj = con.cursor()
    curserObj.execute('SELECT latitude, longitude FROM cell_zone')
    coords = curserObj.fetchall()
    latitude = [c[0] for c in coords]
    longitude = [c[1] for c in coords]
    con.close()
    return latitude, longitude


def drawRegion(m, regionName, weigth=15, opacity=0.7):  # regionName can be any region name (Steiermark, Oberoesterreich etc.)
    with open('GeoJSONAustria/oesterreich.json', 'r') as f:
        austriaGeoData = json.load(f)
        regionBorder = list(filter(lambda x: x['properties']['name'] == regionName, austriaGeoData['features']))[0]['geometry']['coordinates']
        if isinstance(regionBorder[0][0][0], list):
            for region in regionBorder:
                border = [(b[1], b[0]) for b in region[0]]
                folium.Polygon(border, color="gray", weight=weigth, opacity=opacity).add_to(m)
            return

        regionBorder = regionBorder[0]
        border = [(b[1], b[0]) for b in regionBorder]                           # It is necessary to change lat and lng since the json has a different format
        folium.Polygon(border, color="gray", weight=weigth, opacity=opacity).add_to(m)


def drawCellTowers(m, cellTowers=None, constraint=None): # for now contraints are only nesw anchors which linit the points
    if cellTowers is None:
        lat, lng = (sql_fetch('data/at.sqlite'))
        cellTowers = zip(lat, lng)
    else:
        lat, lng = zip(cellTowers)
    if constraint is not None:
        north = constraint[0]
        east = constraint[1]
        south = constraint[2]
        west = constraint[3]
        cellTowers = [coord for coord in zip(lat, lng) if (south[0] < coord[0] < north[0] and west[1] < coord[1] < east[1])]
    for tower in cellTowers:
        folium.Circle(tower, radius=10).add_to(m)


def drawHeatMap(m, heatPoints=None, heatMapWeights=None):
    if heatPoints is None or heatMapWeights is None:
        heatPoints = mp
        for myPoints in heatPoints:
            offsetHeatmap = [random.sample(list(np.arange(0, 0.01, 0.001)), 4) for _ in myPoints]
            heat = [random.sample(range(2, 6), 1) for _ in myPoints]

            myPoints.sort()
            heatMapPoits = []
            for p, h, o in zip(myPoints, heat, offsetHeatmap):
                heatMapPoits += [[p[0], p[1], h[0]]] + [[p[0] + o[0], p[1] + o[1], h[0]]] + [
                    [p[0] - o[2], p[1] - o[3], h[0]]] + [[p[0] + o[0] + o[1], p[1] - o[1] - o[2], h[0]]] + [
                                    [p[0] - o[2] - o[3], p[1] + o[2], h[0]]]
            folium.plugins.HeatMap(heatMapPoits, max_zoom=16).add_to(m)
    else:
        print(len(heatPoints), len(heatMapWeights))
        points = [[float(h[0]), float(h[1]), int(w)] for h, w in zip(heatPoints, heatMapWeights)]
        folium.plugins.HeatMap(points, radius=12).add_to(m)

def drawAllAustrianBorders(m):
    drawRegion(m, weigth=5, opacity=1, regionName='Steiermark')
    drawRegion(m, weigth=5, opacity=1, regionName='Wien')
    drawRegion(m, weigth=5, opacity=1, regionName='Salzburg')
    drawRegion(m, weigth=5, opacity=1, regionName='Vorarlberg')
    drawRegion(m, weigth=5, opacity=1, regionName='Tirol')
    drawRegion(m, weigth=5, opacity=1, regionName='Niederoesterreich')
    drawRegion(m, weigth=5, opacity=1, regionName='Oberoesterreich')
    drawRegion(m, weigth=5, opacity=1, regionName='Kaernten')
    drawRegion(m, weigth=5, opacity=1, regionName='Burgenland')


class dropDownRegionen(QWidget):
    def __init__(self, parent=None):
        super(dropDownRegionen, self).__init__(parent)
        self.selectedText = 'Wien'
        self.parent = parent
        layout = QtWidgets.QHBoxLayout()
        self.cb = QComboBox()
        self.cb.addItems(['Burgenland', 'Kärnten', 'Niederösterreich', 'Oberösterreich', 'Salzburg', 'Steiermark', 'Tirol', 'Vorarlberg', 'Wien', 'Österreich'])
        self.cb.setCurrentText('Wien')
        self.cb.currentIndexChanged.connect(self.regionChange)
        layout.addWidget(self.cb)
        self.setLayout(layout)

    def regionChange(self):
        self.selectedText = self.cb.currentText().replace('ä', 'ae').replace('ö', 'oe').replace('Ö', 'Oe')
        if isinstance(self.parent, HeatMapApp):
            dic = locationDict[self.selectedText]
            self.parent.m.location = dic['location']
            self.parent.m.options['zoom'] = dic['zoom']
            self.parent.display()
        print(self.parent.m.location)


class HeatMapApp(QWidget):
    def __init__(self, parent=None):
        super(HeatMapApp, self).__init__(parent)
        self.setGeometry(0, 0, 1280, 720)
        self.stateDict = {}
        self.setWindowTitle('Privacy preserving disease analysis')
        self.dropDown = dropDownRegionen(self)
        self.dropDown.setGeometry(50, 50, 200, 50)
#        self.dropDown.show()
        self.view = QWebEngineView(self)
        self.buttonPos = [50, 460, 200, 25]
        self.resetMapButton = QtWidgets.QPushButton(self)
        self.drawRegionButton = QtWidgets.QPushButton(self)
        self.drawCellTowersButton = QtWidgets.QPushButton(self)
        self.drawHeatMapButton = QtWidgets.QPushButton(self)
        self.label = QtWidgets.QLabel(self)
        self.initUI()
        self.file = io.BytesIO()
        self.initMapObject()
        self.numbers = []
        self.locations = []
        self.getLocationMappings('data/locationID_mapping.csv')

    def readPlainOut(self, path):
        with open(path) as csv_file:
            csv_reader = csv.reader(csv_file, delimiter=',')
            for row in csv_reader:
                for number in row:
                    self.numbers += [number]

    def getLocationMappings(self, path):
        with open(path) as f:
            cr = csv.reader(f, delimiter=',')
            li = []
            for row in cr:
                li += [row]
        self.locations = [[a[1], a[2]] for a in li]

    def initMapObject(self):
        self.stateDict = {'region': [], 'heat': False, 'cell': False}
        dictionary = locationDict[self.dropDown.selectedText]
        self.m = folium.Map(location=dictionary['location'], zoom_start=dictionary['zoom'])
        self.display()

    def initUI(self):
        self.initButton(self.drawRegionButton, 'Landesgrenze einzeichnen', self.drawRegion)
        self.initButton(self.drawCellTowersButton, 'Mobilfunkmasten einzeichnen', self.drawCellTowers)
        self.initButton(self.drawHeatMapButton, 'Heatmap einzeichnen', self.createHeatMap)
        self.initButton(self.resetMapButton, 'Karte zurücksetzen', self.initMapObject)

    def initButton(self, button, name, func):
        button.setText(name)
        button.clicked.connect(func)
        button.setGeometry(self.buttonPos[0], self.buttonPos[1], self.buttonPos[2], self.buttonPos[3])
        self.buttonPos[1] += 50

    def createHeatMap(self):
        if self.stateDict['heat']:
            print('HeatMap already drawn')
            return
        self.readPlainOut('backend/plain_out')
        self.stateDict['heat'] = True
        drawHeatMap(self.m, self.locations, self.numbers)
        #drawHeatMap(self.m, None, None)
        self.display()

    def drawCellTowers(self):
        if self.stateDict['cell']:
            print('cell towers already drawn')
            return
        self.stateDict['cell'] = True
        drawCellTowers(self.m, None, constraint)
        self.display()

    def drawRegion(self):
        locationName = self.dropDown.selectedText
        if locationName in self.stateDict['region'] or 'Oesterreich' in self.stateDict['region']:
            print('region already drawn')
            return
        self.stateDict['region'].append(locationName)
        if locationName == 'Oesterreich':
            drawAllAustrianBorders(self.m)
            pass
        else:
            drawRegion(self.m, locationName, weigth=5, opacity=1)
        self.display()

    def display(self):
        self.m.save('data/HeatMap.html')
        string = self.m.get_root().render()

        path = QDir.current().filePath('data/HeatMap.html')
        local = QUrl.fromLocalFile(path)

        self.view.load(local)
        self.view.show()
        self.view.setGeometry(300, 0, 980, 720)
        #self.label.setText('HeatMap')
        #self.label.move(1000, 1030)


def window():
    app = QApplication(sys.argv)
    win = HeatMapApp()
    win.show()
    sys.exit(app.exec_())


colors = ["black", "green", "red", "#0B0EB3"]
#window()


#m = folium.Map(location=viennaLocation, zoom_start=8)  # Vienna
#drawRegion(m, 'Wien')
#drawAllAustrianBorders(m)
# drawCellTowers(m, None, None)
# drawHeatMap(m, None)
#m.save('HeatMap.html')


