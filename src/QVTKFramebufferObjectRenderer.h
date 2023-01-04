#ifndef QVTKFRAMEBUFFEROBJECTRENDERER_H
#define QVTKFRAMEBUFFEROBJECTRENDERER_H

#include <memory>
#include <vector>
#include <mutex>
#include <map>

#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QQuickFramebufferObject>
#include <QUndoStack>
#include <QDir>

#include <vtkActor.h>
#include <vtkCellPicker.h>
#include <vtkCubeSource.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkGenericRenderWindowInteractor.h>
#include <vtkObject.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkAxesActor.h>
#include <vtkSphereSource.h>
#include <vtkIterativeClosestPointTransform.h>
#include <QVTKOpenGLNativeWidget.h>

class Model;
class QVTKFramebufferObjectItem;
class ProcessingEngine;

class QVTKFramebufferObjectRenderer : public QObject, public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	QVTKFramebufferObjectRenderer();

	void setProcessingEngine(const std::shared_ptr<ProcessingEngine> processingEngine);

	virtual void synchronize(QQuickFramebufferObject *item);
	virtual void render();
	virtual void openGLInitState();
	QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);

	void addModelActor(const std::shared_ptr<Model> model);

	std::shared_ptr<Model> getSelectedModel() const;
	bool isModelSelected() const;

	const vtkSmartPointer<vtkRenderer>& getRenderer() const;

	void showPlatform(const bool checked);
	void showAxes(const bool checked);
	void closeModel();
	void closeAllModel(std::vector<std::shared_ptr<Model>> models);

	void setSelectedModelPositionX(const double positionX);
	void setSelectedModelPositionY(const double positionY);
	void setSelectedModelRepresentation(const int option);
	void setSelectedModelDecreasePolygons(const double polygons);
	void setSelectedModelIncreasePolygons(const int iterations);
	void setSelectedModelOpacity(const double opacity);
	void setSelectedModelSmooth(const int smooth);
	void setSelectedModelGI(const bool checked);
	void setSelectedModelColorR(const int colorR);
	void setSelectedModelColorG(const int colorG);
	void setSelectedModelColorB(const int colorB);
	void setSelectedModelPolygons(const int polygons);
	void setSelectedModelPoints(const int points);
	void setSelectedModelPath(const QString path);
	void setSelectedModelVolume(const double volume);
	void setSelectedModelSurfaceArea(const double surfaceArea);
	void setSelectedModelMaxAreaOfCell(const double maxAreaOfCell);
	void setSelectedModelMinAreaOfCell(const double minAreaOfCell);

	double getSelectedModelPositionX() const;
	double getSelectedModelPositionY() const;
	int getSelecteModelRepresentation() const;
	double getSelecteModelDecreasePolygons() const;
	int getSelecteModelIncreasePolygons() const;
	double getSelecteModelOpacity() const;
	int getSelecteModelSmooth() const;
	bool getSelecteModelGI() const;
	int getSelectedModelColorR() const;
	int getSelectedModelColorG() const;
	int getSelectedModelColorB() const;
	int getSelecteModelPolygons() const;
	int getSelecteModelPoints() const;
	QString getSelecteModelFileName() const;
	double getSelecteModelVolume() const;
	double getSelecteModelSurfaceArea() const;
	double getSelecteModelMaxAreaOfCell() const;
	double getSelecteModelMinAreaOfCell() const;
	int getModelCount() const;

	void resetCamera();
	const bool screenToWorld(const int16_t screenX, const int16_t screenY, double worldPos[]);

	void setModelFilePathToMap(QUrl path);

signals:
	void isModelSelectedChanged();

	void selectedModelPositionXChanged();
	void selectedModelPositionYChanged();

	void isModelRepresentationChanged();
	void isModelDecreasePolygonsChanged();
	void isModelIncreasePolygonsChanged();
	void isModelOpacityChanged();
	void isModelSmoothChanged();
	void isModelGIChanged();
	void isModelColorRChanged();
	void isModelColorGChanged();
	void isModelColorBChanged();
	void isModelPolygonsChanged();
	void isModelPointsChanged();
	void isModelFileNameChanged(const QString path);
	void isModelVolumeChanged(const double volume);
	void isModelSurfaceAreaChanged(const double surfaceArea);
	void isModelMaxAreaOfCellChanged(const double maxAreaOfCell);
	void isModelMinAreaOfCellChanged(const double minAreaOfCell);
	void isModelCountChanged(const int count);

private:
	void initScene();
	void generatePlatform();
	void updatePlatform();

	void selectModel(const int16_t x, const int16_t y);
	void clearSelectedModel(int param);
	void setIsModelSelected(const bool isModelSelected);

	void createLine(const double x1, const double y1, const double z1, const double x2, const double y2, const double z2, vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkCellArray> cells);
	std::shared_ptr<Model> getSelectedModelNoLock() const;

	std::shared_ptr<ProcessingEngine> m_processingEngine;
	QVTKFramebufferObjectItem *m_vtkFboItem = nullptr;
	vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_vtkRenderWindow;
	vtkSmartPointer<vtkRenderer> m_renderer;
	vtkSmartPointer<vtkGenericRenderWindowInteractor> m_vtkRenderWindowInteractor;

	vtkSmartPointer<vtkCellPicker> m_picker;

	std::shared_ptr<Model> m_selectedModel = nullptr;
	vtkSmartPointer<vtkActor> m_selectedActor = nullptr;
	vtkSmartPointer<vtkActor> m_tmpAddActor = nullptr;
	std::map<vtkSmartPointer<vtkActor>, QUrl> m_fileNameMap;
	
	bool m_isModelSelected = false;

	double m_selectedModelPositionX = 0.0;
	double m_selectedModelPositionY = 0.0;

	std::shared_ptr<QMouseEvent> m_mouseLeftButton = nullptr;
	std::shared_ptr<QMouseEvent> m_mouseEvent = nullptr;
	std::shared_ptr<QMouseEvent> m_moveEvent = nullptr;
	std::shared_ptr<QWheelEvent> m_wheelEvent = nullptr;

	vtkSmartPointer<vtkCubeSource> m_platformModel = nullptr;;
	vtkSmartPointer<vtkPolyData> m_platformGrid = nullptr;;
	vtkSmartPointer<vtkActor> m_platformModelActor = nullptr;;
	vtkSmartPointer<vtkActor> m_platformGridActor = nullptr;;
	vtkSmartPointer<vtkAxesActor> m_axes = nullptr;;

	double m_platformWidth = 200.0;
	double m_platformDepth = 200.0;
	double m_platformHeight = 200.0;
	double m_platformThickness = 2.0;
	double m_gridBottomHeight = 0.15;
	uint16_t m_gridSize = 10;

	double m_camPositionX;
	double m_camPositionY;
	double m_camPositionZ;

	double m_clickPositionZ = 0.0;

	bool m_firstRender = true;

	int m_modelsRepresentationOption = 0;
	double m_modelsOpacity = 1.0;
	bool m_modelsGouraudInterpolation = false;

	int m_modelRepresentation = 2;
	double m_modelDecreasePolygons = 0.0;
	int m_modelIncreasePolygons = 0;
	double m_modelOpacity = 1.0;
	int m_modelSmooth = 0;
	bool m_modelGI = false;
	int m_modelColorR = 255;
	int m_modelColorG = 255;
	int m_modelColorB = 255;
	int m_modelPolygons = 0;
	int m_modelPoints = 0;
	QString m_modelPath = "None";
	double m_modelVolume = 0.0;
	double m_modelSurfaceArea = 0.0;
	double m_modelMaxAreaOfCell = 0.0;
	double m_modelMinAreaOfCell = 0.0;
	int m_modelCount = 0;
};

#endif // QVTKFRAMEBUFFEROBJECTRENDERER_H
