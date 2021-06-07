import sys
from PyQt5.QtWidgets import QMainWindow, QApplication
from PyQt5 import QtCore, QtGui, QtWidgets ,QtPrintSupport
from newgui import Ui_MainWindow
from vtk.qt.QVTKRenderWindowInteractor import QVTKRenderWindowInteractor
import vtk
import os

surfaceExtractor = vtk.vtkContourFilter()
volumeColor = vtk.vtkColorTransferFunction()



def slider_SLOT(val):
    surfaceExtractor.SetValue(0, val)
    iren.update()
    
class AppWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.ui.verticalSlider.setValue(100)
        self.ui.verticalSlider_2.setValue(50)
        self.ui.verticalSlider_3.setValue(30)
        self.ui.label_6.setText("1")
        self.ui.label_7.setText("0.5")
        self.ui.label_8.setText("0.3")

        self.ui.menuOpen_File.triggered.connect(lambda:self.opendcm())
        self.ui.comboBox.currentTextChanged.connect(lambda:self.mode())


        self.ui.horizontalSlider.valueChanged.connect(slider_SLOT)
        self.ui.horizontalSlider.valueChanged.connect(lambda:self.update())


        self.ui.verticalSlider.valueChanged.connect(lambda:self.rgb())
        self.ui.verticalSlider_2.valueChanged.connect(lambda:self.rgb())
        self.ui.verticalSlider_3.valueChanged.connect(lambda:self.rgb())


        self.ui.verticalSlider.valueChanged.connect(lambda:self.update2())
        self.ui.verticalSlider_2.valueChanged.connect(lambda:self.update2())
        self.ui.verticalSlider_3.valueChanged.connect(lambda:self.update2()) 


        self.surface=[self.ui.horizontalSlider, self.ui.label, self.ui.label_2 ]
        self.ray=[self.ui.verticalSlider, self.ui.verticalSlider_2, self.ui.verticalSlider_3, self.ui.label_3, self.ui.label_4,self.ui.label_5, self.ui.label_6, self.ui.label_7, self.ui.label_8, self.ui.label_9]


        for i in range(len(self.ray)):
            self.ray[i].hide()


##### OPENING DICOM DATA  #######
    def opendcm(self):
        fname = QtWidgets.QFileDialog.getExistingDirectory(self,"Open a folder",os.path.expanduser("~"),QtWidgets.QFileDialog.ShowDirsOnly)
        self.path=fname.split("/")[-1]
        print(fname)
        print(self.path)
        self.mode()

##### UPDATING THE SLIDERS VALUE LABELS ######


    def update(self):

        self.ui.label_2.setText(str(self.ui.horizontalSlider.value()))

    def update2(self):
        self.ui.label_6.setText(str(self.ui.verticalSlider.value()/100))
        self.ui.label_7.setText(str(self.ui.verticalSlider_2.value()/100))
        self.ui.label_8.setText(str(self.ui.verticalSlider_3.value()/100))



##### CHECKING RENDERING MODE ######

    def mode(self):

        if self.ui.comboBox.currentText()=="Surface Rendering":
            for i in range(len(self.surface)):
                 self.surface[i].show()
            for i in range(len(self.ray)):
                self.ray[i].hide()
            self.surfacerendering()
        elif self.ui.comboBox.currentText()=="Ray Casting Rendering":
            for i in range(len(self.surface)):
                self.surface[i].hide()
            for i in range(len(self.ray)):
                self.ray[i].show()
            self.raycasting()

##### SURFACE RENDERING FUNCTION ######

    def surfacerendering(self):
        self.aRender = vtk.vtkRenderer()
        self.ui.widget.vtkWidget.GetRenderWindow().AddRenderer(self.aRender)
        self.iren = self.ui.widget.vtkWidget.GetRenderWindow().GetInteractor()
        self.iren.SetInteractorStyle(vtk.vtkInteractorStyleTrackballCamera())
        reader = vtk.vtkDICOMImageReader()
        reader.SetDirectoryName(self.path)
        reader.Update()

        
        surfaceExtractor.SetInputConnection(reader.GetOutputPort())
        surfaceExtractor.SetValue(0, 0)
        surfaceNormals = vtk.vtkPolyDataNormals()
        surfaceNormals.SetInputConnection(surfaceExtractor.GetOutputPort())
        surfaceNormals.SetFeatureAngle(60.0)
        surfaceMapper = vtk.vtkPolyDataMapper()
        surfaceMapper.SetInputConnection(surfaceNormals.GetOutputPort())
        surfaceMapper.ScalarVisibilityOff()
        surface = vtk.vtkActor()
        surface.SetMapper(surfaceMapper)
        
        aCamera = vtk.vtkCamera()
        aCamera.SetViewUp(0, 0, -1)
        aCamera.SetPosition(0, 1, 0)
        aCamera.SetFocalPoint(0, 0, 0)
        aCamera.ComputeViewPlaneNormal()
        
        self.aRender.AddActor(surface)
        self.aRender.SetActiveCamera(aCamera)
        self.aRender.ResetCamera()
        
        self.aRender.SetBackground(0, 0, 0)
        
        self.aRender.ResetCameraClippingRange()
        
        # Interact with the data.
        self.iren.Initialize()
        # renWin.Render()
        self.iren.Start()
        # self.iren.show()
        



##### RAY CASTING (VOLUME) RENDERING FUNCTION ######

    def raycasting(self):
        self.aRender = vtk.vtkRenderer()
        self.ui.widget.vtkWidget.GetRenderWindow().AddRenderer(self.aRender)
        self.iren = self.ui.widget.vtkWidget.GetRenderWindow().GetInteractor()
        self.iren.SetInteractorStyle(vtk.vtkInteractorStyleTrackballCamera())
        self.reader = vtk.vtkDICOMImageReader()
        self.reader.SetDirectoryName(self.path)
        self.reader.Update()


        self.volumeMapper = vtk.vtkGPUVolumeRayCastMapper()
        self.volumeMapper.SetInputConnection(self.reader.GetOutputPort())
        self.volumeMapper.SetBlendModeToComposite()



        self.volumeColor = vtk.vtkColorTransferFunction()
        self.volumeColor.AddRGBPoint(0,    0.0, 0.0, 0.0)
        self.volumeColor.AddRGBPoint(25,  1.0, 0.5, 0.3)
        self.volumeColor.AddRGBPoint(50, 1.0, 0.5, 0.3)
        self.volumeColor.AddRGBPoint(70, 1.0, 1.0, 0.9)


        self.volumeScalarOpacity = vtk.vtkPiecewiseFunction()
        self.volumeScalarOpacity.AddPoint(0,    0.0)
        self.volumeScalarOpacity.AddPoint(25,  0.85)
        self.volumeScalarOpacity.AddPoint(50, 0.15)
        self.volumeScalarOpacity.AddPoint(70, 0.15)
        


        self.volumeGradientOpacity = vtk.vtkPiecewiseFunction()
        self.volumeGradientOpacity.AddPoint(0,   0.0)
        self.volumeGradientOpacity.AddPoint(10,  0.5)
        self.volumeGradientOpacity.AddPoint(15, 1.0)



        self.volumeProperty = vtk.vtkVolumeProperty()
        self.volumeProperty.SetColor(self.volumeColor)
        self.volumeProperty.SetScalarOpacity(self.volumeScalarOpacity)
        self.volumeProperty.SetGradientOpacity(self.volumeGradientOpacity)
        self.volumeProperty.SetInterpolationTypeToLinear()
        self.volumeProperty.ShadeOn()
        self.volumeProperty.SetAmbient(0.4)
        self.volumeProperty.SetDiffuse(0.6)
        self.volumeProperty.SetSpecular(0.2)


        self.volume = vtk.vtkVolume()
        self.volume.SetMapper(self.volumeMapper)
        self.volume.SetProperty(self.volumeProperty)

        self.aRender.AddViewProp(self.volume)

        self.camera =  self.aRender.GetActiveCamera()
        self.c = self.volume.GetCenter()
        self.camera.SetFocalPoint(self.c[0], self.c[1], self.c[2])
        self.camera.SetPosition(self.c[0] + 600, self.c[1], self.c[2])
        self.camera.SetViewUp(0, 0, -1)


        # Interact with the data.
        self.iren.Initialize()
        # renWin.Render()
        self.iren.Start()



######## BONUS ########

##### UPDATING THE TRANSFERE FUNCTION WITH RGB VALUES OF THE SKIN ######

    def rgb(self):


        self.volumeMapper = vtk.vtkGPUVolumeRayCastMapper()
        self.volumeMapper.SetInputConnection(self.reader.GetOutputPort())
        self.volumeMapper.SetBlendModeToComposite()


        self.a=self.ui.verticalSlider.value()/100
        self.b=self.ui.verticalSlider_2.value()/100
        self.c=self.ui.verticalSlider_3.value()/100

        self.volumeColor = vtk.vtkColorTransferFunction()
        self.volumeColor.AddRGBPoint(0,    0.0, 0.0, 0.0)
        self.volumeColor.AddRGBPoint(25, self.a, self.b, self.c)
        self.volumeColor.AddRGBPoint(50, 1.0, 0.5, 0.3)
        self.volumeColor.AddRGBPoint(70, 1.0, 1.0, 0.9)



        self.volumeScalarOpacity = vtk.vtkPiecewiseFunction()
        self.volumeScalarOpacity.AddPoint(0,    0.0)
        self.volumeScalarOpacity.AddPoint(25,  0.85)
        self.volumeScalarOpacity.AddPoint(50, 0.15)
        self.volumeScalarOpacity.AddPoint(70, 0.15)
        

        self.volumeGradientOpacity = vtk.vtkPiecewiseFunction()
        self.volumeGradientOpacity.AddPoint(0,   0.0)
        self.volumeGradientOpacity.AddPoint(10,  0.5)
        self.volumeGradientOpacity.AddPoint(15, 1.0)



        self.volumeProperty = vtk.vtkVolumeProperty()
        self.volumeProperty.SetColor(self.volumeColor)
        self.volumeProperty.SetScalarOpacity(self.volumeScalarOpacity)
        self.volumeProperty.SetGradientOpacity(self.volumeGradientOpacity)
        self.volumeProperty.SetInterpolationTypeToLinear()
        self.volumeProperty.ShadeOn()
        self.volumeProperty.SetAmbient(0.4)
        self.volumeProperty.SetDiffuse(0.6)
        self.volumeProperty.SetSpecular(0.2)


        self.volume = vtk.vtkVolume()
        self.volume.SetMapper(self.volumeMapper)
        self.volume.SetProperty(self.volumeProperty)


        self.aRender.AddViewProp(self.volume)

        self.camera =  self.aRender.GetActiveCamera()
        self.c = self.volume.GetCenter()
        self.camera.SetFocalPoint(self.c[0], self.c[1], self.c[2])
        self.camera.SetPosition(self.c[0] + 600, self.c[1], self.c[2])
        self.camera.SetViewUp(0, 0, -1)


        iren.update()






app = QApplication(sys.argv)
# The class that connect Qt with VTK
iren = QVTKRenderWindowInteractor()
aRender= vtk.vtkRenderer()
w = AppWindow()
# surfacerendering()
w.show()
sys.exit(app.exec_())
# Start the event loop.
