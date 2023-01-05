import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.3
import QtQuick.Controls.Material 2.2
import QtVTK 1.0

ApplicationWindow {
    id: root
    minimumWidth: 1366
    minimumHeight: 768
    visible: true
    title: "FinalProject"

    Material.primary: Material.Indigo
    Material.accent: Material.Teal

    Rectangle {
        id: screenCanvasUI
        anchors.fill: parent

        VtkFboItem {
            id: vtkFboItem
            objectName: "vtkFboItem"
            anchors.fill: parent

            MouseArea {
                acceptedButtons: Qt.LeftButton
                anchors.fill: parent

                onPositionChanged: {
                    canvasHandler.mouseMoveEvent(pressedButtons, mouseX, mouseY);
                }
                onPressed: {
                    canvasHandler.mousePressEvent(pressedButtons, mouseX, mouseY);
                }
                onReleased: {
                    canvasHandler.mouseReleaseEvent(pressedButtons, mouseX, mouseY);
                }
            }
        }

        Button {
            id: openFileButton
            text: "打開模型"
            font.pixelSize: 16
            font.bold: true
            visible: !canvasHandler.isModelSelected
            highlighted: true
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 50
            onClicked: canvasHandler.showFileDialog = true;

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: "打開一個3D檔案"
        }

        Button {
            id: appendFileButton
            text: "合併模型"
            font.pixelSize: 16
            font.bold: true
            visible: !canvasHandler.isModelSelected
            highlighted: true
            anchors.right: parent.right
            anchors.bottom: openFileButton.bottom
            anchors.margins: 50
            onClicked: canvasHandler.showFilesDialog = true;

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: "打開多個3D檔案並合併"
        }

        Button {
            id: saveFileButton
            text: "另存模型"
            font.pixelSize: 16
            font.bold: true
            visible: canvasHandler.isModelSelected
            highlighted: true
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 50
            onClicked: canvasHandler.showSaveFileDialog = true;

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: "另存為其他類型的3D檔案"
        }

        Button {
            id: resetButton
            visible: canvasHandler.isModelSelected
            text: "回到原點"
            font.pixelSize: 16
            font.bold: true
            highlighted: true
            anchors.right: parent.right
            anchors.bottom: openFileButton.bottom
            anchors.margins: 50
            onClicked: canvasHandler.resetModelPositionEvent()

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: "重置模型位置至(0,0)"
        }

        Button {
            id: closeFileButton
            visible: canvasHandler.isModelSelected
            text: "關閉模型"
            Material.background: Material.Red
            Material.foreground: "#ffffff"
            font.pixelSize: 16
            font.bold: true

            anchors.right: parent.right
            anchors.bottom: resetButton.bottom
            anchors.margins: 50
            onClicked: canvasHandler.closeModel()
        }

        Button {
            id: closeAllFileButton
            visible: !canvasHandler.isModelSelected
            text: "清空模型"
            Material.background: Material.Red
            Material.foreground: "#ffffff"
            font.pixelSize: 16
            font.bold: true

            anchors.right: parent.right
            anchors.bottom: resetButton.bottom
            anchors.margins: 50
            onClicked: canvasHandler.closeAllModel()
        }

        Switch {
            id: platformSwitch
            text: "顯示平台"
            checked: true;
            font.bold: true
            Material.foreground: "#000000"
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.rightMargin: 30

            onCheckedChanged: canvasHandler.showPlatform(checked);
        }

        Switch {
            id: axesSwitch
            text: "顯示坐標軸"
            checked: true;
            font.bold: true
            Material.foreground: "#000000"
            anchors.top: platformSwitch.bottom
            anchors.right: parent.right
            anchors.rightMargin: 15

            onCheckedChanged: canvasHandler.showAxes(checked);
        }

        TextField {
            id: cubeZLengthTextField
            validator: IntValidator{bottom: 0; top:100}
            text: "10"
            font.bold: true
            color: "#333333"
            maximumLength: 3
            anchors.right: parent.right
            anchors.top: axesSwitch.bottom
            anchors.rightMargin: 15
            width: 35
        }

        TextField {
            id: cubeYLengthTextField
            validator: IntValidator{bottom: 0; top:100}
            text: "10"
            font.bold: true
            color: "#333333"
            maximumLength: 3
            anchors.right: cubeZLengthTextField.left
            anchors.top: axesSwitch.bottom
            anchors.rightMargin: 15
            width: 35
        }

        TextField {
            id: cubeXLengthTextField
            validator: IntValidator{bottom: 0; top:100}
            text: "10"
            font.bold: true
            color: "#333333"
            maximumLength: 3
            anchors.right: cubeYLengthTextField.left
            anchors.top: axesSwitch.bottom
            anchors.rightMargin: 15
            width: 35
        }

        Button {
            id: createCubeButton
            text: "生成立方體"
            highlighted: true
            font.pixelSize: 16
            font.bold: true
            anchors.top: cubeZLengthTextField.bottom
            anchors.right: parent.right
            anchors.rightMargin: 35
            onClicked: canvasHandler.createCube(cubeXLengthTextField.text,cubeYLengthTextField.text,cubeZLengthTextField.text)
        }

        TextField {
            id: sphereRadiusTextField
            validator: IntValidator{bottom: 0; top:100}
            text: "10"
            font.bold: true
            color: "#333333"
            maximumLength: 3
            anchors.right: createSphareButton.left
            anchors.top: createCubeButton.bottom
            anchors.rightMargin: 15
            anchors.topMargin: 5
            width: 35
        }

        Button {
            id: createSphareButton
            text: "生成球體"
            highlighted: true
            font.pixelSize: 16
            font.bold: true
            anchors.top: createCubeButton.bottom
            anchors.right: parent.right
            anchors.rightMargin: 20
            onClicked: canvasHandler.createSphare(sphereRadiusTextField.text)
        }

        ComboBox {
            id: representationCombobox
            visible: canvasHandler.isModelSelected
            width: 200
            model: ["點", "線", "面"]
            currentIndex: canvasHandler.modelRepresentation
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 40
            anchors.topMargin: 20

            onActivated: canvasHandler.setModelRepresentation(currentIndex);
        }

        Label {
            id: decreasePolygonsLabel
            visible: canvasHandler.isModelSelected
            color: "#333333"
            text: "減少網格: " + decreasePolygonsSlider.value + "%   （二次誤差度量算法）"
            font.pixelSize: 16
            font.bold: true
            anchors.top: representationCombobox.bottom
            anchors.left: parent.left
            anchors.topMargin: 15
            anchors.leftMargin: 40
        }

        Slider {
            id: decreasePolygonsSlider
            visible: canvasHandler.isModelSelected
            width: 200
            value: canvasHandler.modelDecreasePolygons
            from: 0
            to: 0.99
            stepSize: 0.01
            anchors.left: parent.left
            anchors.top: decreasePolygonsLabel.bottom
            anchors.leftMargin: 40
            anchors.topMargin: 5

            onValueChanged: canvasHandler.setModelDecreasePolygons(value);
        }

        Label {
            id: increasePolygonsLabel
            visible: canvasHandler.isModelSelected
            color: "#333333"
            text: "增加網格迭代次數: " + increasePolygonsSlider.value + "   （Loop細分算法）"
            font.pixelSize: 16
            font.bold: true
            anchors.top: decreasePolygonsSlider.bottom
            anchors.left: parent.left
            anchors.topMargin: 5
            anchors.leftMargin: 40
        }

        Slider {
            id: increasePolygonsSlider
            visible: canvasHandler.isModelSelected
            width: 200
            value: canvasHandler.modelIncreasePolygons
            from: 0
            to: 6
            stepSize: 1
            anchors.left: parent.left
            anchors.top: increasePolygonsLabel.bottom
            anchors.leftMargin: 40
            anchors.topMargin: 5

            onValueChanged: canvasHandler.setModelIncreasePolygons(value);
        }

        Label {
            id: smoothLabel
            visible: canvasHandler.isModelSelected
            color: "#333333"
            text: "平滑迭代次數: " + smoothSlider.value + "   （拉普拉斯平滑算法）"
            font.pixelSize: 16
            font.bold: true
            anchors.top: increasePolygonsSlider.bottom
            anchors.left: parent.left
            anchors.topMargin: 5
            anchors.leftMargin: 40
        }

        Slider {
            id: smoothSlider
            visible: canvasHandler.isModelSelected
            width: 200
            value: canvasHandler.modelSmooth
            from: 0
            to: 200
            stepSize: 1
            anchors.left: parent.left
            anchors.top: smoothLabel.bottom
            anchors.leftMargin: 40
            anchors.topMargin: 5

            onValueChanged: canvasHandler.setModelSmooth(value);
        }

        Label {
            id: opacityLabel
            visible: canvasHandler.isModelSelected
            color: "#333333"
            text: "透明度: " + opacitySlider.value
            font.pixelSize: 16
            font.bold: true
            anchors.top: smoothSlider.bottom
            anchors.left: parent.left
            anchors.topMargin: 5
            anchors.leftMargin: 40
        }

        Slider {
            id: opacitySlider
            visible: canvasHandler.isModelSelected
            width: 200
            value: canvasHandler.modelOpacity
            from: 0.1
            to: 1
            stepSize: 0.01
            anchors.left: parent.left
            anchors.top: opacityLabel.bottom
            anchors.leftMargin: 40
            anchors.topMargin: 5

            onValueChanged: canvasHandler.setModelOpacity(value);
        }

        Switch {
            id: gouraudInterpolationSwitch
            visible: canvasHandler.isModelSelected
            text: "Gouraud著色"
            checked: canvasHandler.modelGouraud;
            font.bold: true
            anchors.left: parent.left
            anchors.top: opacitySlider.bottom
            anchors.leftMargin: 40

            onCheckedChanged: canvasHandler.setGouraudInterpolation(checked);
        }

        TextField {
            id: modelColorR
            visible: canvasHandler.isModelSelected
            validator: IntValidator{bottom: 0; top:255}
            text: canvasHandler.modelColorR
            font.bold: true
            color: "red"
            maximumLength: 3
            anchors.left: parent.left
            anchors.top: gouraudInterpolationSwitch.bottom
            anchors.leftMargin: 40
            anchors.topMargin: 5
            width: 35
        }

        TextField {
            id: modelColorG
            visible: canvasHandler.isModelSelected
            validator: IntValidator{bottom: 0; top:255}
            text: canvasHandler.modelColorG
            font.bold: true
            color: "green"
            maximumLength: 3
            anchors.left: modelColorR.right
            anchors.top: gouraudInterpolationSwitch.bottom
            anchors.leftMargin: 15
            anchors.topMargin: 5
            width: 35
        }

        TextField {
            id: modelColorB
            visible: canvasHandler.isModelSelected
            validator: IntValidator{bottom: 0; top:255}
            text: canvasHandler.modelColorB
            font.bold: true
            color: "blue"
            maximumLength: 3
            anchors.left: modelColorG.right
            anchors.top: gouraudInterpolationSwitch.bottom
            anchors.leftMargin: 15
            anchors.topMargin: 5
            width: 35
        }

        Button {
            id: lockColorButton
            text: "鎖定顏色"
            font.bold: true
            font.pixelSize: 16
            visible: canvasHandler.isModelSelected
            highlighted: true
            anchors.left: modelColorB.right
            anchors.top: gouraudInterpolationSwitch.bottom
            anchors.leftMargin: 15
            onClicked:
            {
                if(modelColorR.readOnly && modelColorG.readOnly && modelColorB.readOnly)
                {
                    modelColorR.color = "red";
                    modelColorG.color = "green";
                    modelColorB.color = "blue";
                    this.text = "鎖定顏色"
                }
                else
                {
                    modelColorR.color = "#333333";
                    modelColorG.color = "#333333";
                    modelColorB.color = "#333333";
                    canvasHandler.setModelColorR(modelColorR.text);
                    canvasHandler.setModelColorG(modelColorG.text);
                    canvasHandler.setModelColorB(modelColorB.text);
                    this.text = "解除鎖定"
                }
                modelColorR.readOnly = !modelColorR.readOnly;
                modelColorG.readOnly = !modelColorG.readOnly;
                modelColorB.readOnly = !modelColorB.readOnly;
            }
        }

        Label {
            id: volumeLabel
            color: "#333333"
            visible: canvasHandler.isModelSelected
            text: "體積: " + canvasHandler.modelVolume
            font.pixelSize: 18
            font.bold: true
            anchors.bottom: polygonsLabel.top
            anchors.left: parent.left
            anchors.leftMargin: 40
            anchors.bottomMargin: 10
        }

        Label {
            id: surfaceAreaLabel
            color: "#333333"
            visible: canvasHandler.isModelSelected
            text: "表面積: " + canvasHandler.modelSurfaceArea
            font.pixelSize: 18
            font.bold: true
            anchors.bottom: polygonsLabel.top
            anchors.left: volumeLabel.right
            anchors.leftMargin: 10
            anchors.bottomMargin: 10
        }

        Label {
            id: maxAreaLabel
            color: "#333333"
            visible: canvasHandler.isModelSelected
            text: "最大單元面積: " + canvasHandler.modelMaxAreaOfCell
            font.pixelSize: 18
            font.bold: true
            anchors.bottom: polygonsLabel.top
            anchors.left: surfaceAreaLabel.right
            anchors.leftMargin: 10
            anchors.bottomMargin: 10
        }

        Label {
            id: minAreaLabel
            color: "#333333"
            visible: canvasHandler.isModelSelected
            text: "最小單元面積: " + canvasHandler.modelMinAreaOfCell
            font.pixelSize: 18
            font.bold: true
            anchors.bottom: polygonsLabel.top
            anchors.left: maxAreaLabel.right
            anchors.leftMargin: 10
            anchors.bottomMargin: 10
        }

        Label {
            id: polygonsLabel
            color: "#333333"
            visible: canvasHandler.isModelSelected
            text: "網格數量: " + canvasHandler.modelPolygons
            font.pixelSize: 18
            font.bold: true
            anchors.bottom: positionLabelX.top
            anchors.left: parent.left
            anchors.leftMargin: 40
            anchors.bottomMargin: 10
        }

        Label {
            id: pointLabel
            color: "#333333"
            visible: canvasHandler.isModelSelected
            text: "點的數量: " + canvasHandler.modelPoints
            font.pixelSize: 18
            font.bold: true
            anchors.bottom: positionLabelX.top
            anchors.left: polygonsLabel.right
            anchors.leftMargin: 10
            anchors.bottomMargin: 10
        }

        Label {
            id: positionLabelX
            color: "#333333"
            visible: canvasHandler.isModelSelected
            text: "X: " + canvasHandler.modelPositionX
            font.pixelSize: 18
            font.bold: true
            anchors.bottom: fileNameLabelX.top
            anchors.left: parent.left
            anchors.leftMargin: 40
            anchors.bottomMargin: 10
        }

        Label {
            id: positionLabelY
            color: "#333333"
            visible: canvasHandler.isModelSelected
            text: "Y: " + canvasHandler.modelPositionY
            font.pixelSize: 18
            font.bold: true
            anchors.bottom: fileNameLabelX.top
            anchors.left: positionLabelX.right
            anchors.leftMargin: 10
            anchors.bottomMargin: 10
        }

        Label {
            id: fileNameLabelX
            color: "#333333"
            visible: canvasHandler.isModelSelected
            text: canvasHandler.modelFileName
            font.pixelSize: 18
            font.bold: true
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.leftMargin: 40
            anchors.bottomMargin: 10
        }

        Label{
            id: modelCountLabel
            visible: !canvasHandler.isModelSelected && canvasHandler.modelCount >= 1
            color: "#333333"
            text: "目前已載入" + canvasHandler.modelCount + "個模型"
            font.pixelSize: 18
            font.bold: true
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.leftMargin: 40
            anchors.bottomMargin: 20
        }
    }

    FileDialog {
        id: openModelFileDialog
        visible: canvasHandler.showFileDialog
        title: "載入模型（支持多個模型同時載入）"
        folder: shortcuts.documents
        selectMultiple: true
        nameFilters: ["Model files" + "(*.stl *.STL *.obj *.OBJ *.ply *.PLY *.vtk *.VTK *.vtp *.VTP)"]

        onAccepted: {
            canvasHandler.showFileDialog = false; // 關閉開關
            canvasHandler.openModel(fileUrls); // 把文件路徑丟進去C++裡面
        }
        onRejected: {
            canvasHandler.showFileDialog = false;
        }
    }

    FileDialog {
        id: openModelFilesDialog
        visible: canvasHandler.showFilesDialog
        title: "載入多個模型並合併"
        folder: shortcuts.documents
        selectMultiple: true
        nameFilters: ["Model files" + "(*.stl *.STL *.obj *.OBJ *.ply *.PLY *.vtk *.VTK *.vtp *.VTP)"]

        onAccepted: {
            canvasHandler.showFilesDialog = false; // 關閉開關
            canvasHandler.openModels(fileUrls); // 把文件路徑丟進去C++裡面
        }
        onRejected: {
            canvasHandler.showFilesDialog = false;
        }
    }


    FileDialog {
        id: saveModelFileDialog
        visible: canvasHandler.showSaveFileDialog
        title: "模型存檔"
        folder: shortcuts.documents
        selectExisting: false
        nameFilters: ["STL file (*.stl *.STL)","OBJ file (*.obj *.OBJ)","VTK file (*.vtk *.VTK)","PLY file (*.ply *.PLY)","VTP file (*.vtp *.VTP)"]

        onAccepted: {
            canvasHandler.showSaveFileDialog = false; // 關閉開關
            canvasHandler.saveModel(fileUrl); // 把文件路徑丟進去C++裡面
        }
        onRejected: {
            canvasHandler.showSaveFileDialog = false;
        }
    }
}
