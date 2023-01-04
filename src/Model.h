#ifndef MODEL_H
#define MODEL_H

#include <memory>
#include <mutex>

#include <QObject>
#include <QColor>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkPolyData.h>
#include <vtkQuadricDecimation.h>
#include <vtkLoopSubdivisionFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkWarpVector.h>
#include <vtkOutlineFilter.h>
#include <vtkMassProperties.h>

#include "QVTKFramebufferObjectRenderer.h"


class Model : public QObject
{
	Q_OBJECT

public:
	Model(vtkSmartPointer<vtkPolyData> modelData);

	const vtkSmartPointer<vtkActor>& getModelActor() const;
	const vtkSmartPointer<vtkActor>& getModelOutlineActor() const;

	double getPositionX();
	double getPositionY();
	void getModelData();

	int getModelRepresentation();
	double getModelTargetReduction();
	int getModelIncreasePolygonsIterations();
	double getModelOpacity();
	int getModelSmooth();
	bool getModelGI();
	int getModelColorR();
	int getModelColorG();
	int getModelColorB();
	int getModelPolygons();
	int getModelPoints();
	double getModelVolume();
	double getModelSurfaceArea();
	double getModelMaxAreaOfCell();
	double getModelMinAreaOfCell();

	void translateToPosition(const double x, const double y);

	void setSelected(const bool selected);
	static void setSelectedModelColor(const QColor &selectedModelColor);

	const double getMouseDeltaX() const;
	const double getMouseDeltaY() const;
	void setMouseDeltaXY(const double deltaX, const double deltaY);

	// functions
	void saveModel(const QUrl modelPath);

	void setModelRepresentation(const int modelsRepresentationOption);
	void setModelDecreasePolygons(const double modelPolygons);
	void setModelIncreasePolygons(const int iterations);
	void setModelOpacity(const double modelsOpacity);
	void setModelGouraudInterpolation(const bool enableGouraudInterpolation);
	void setModelColor(const QColor &color);
	void setModelSmooth(const int value);

	void updateModelColor();

signals:
	void positionXChanged(const double positionX);
	void positionYChanged(const double positionY);
	void modelRepresentationChanged(const int option);
	void modelDecreasePolygonsChanged(const double polygons);
	void modelIncreasePolygonsChanged(const int iterations);
	void modelOpacityChanged(const double opacity);
	void modelSmoothChanged(const int smooth);
	void modelGIChanged(const bool checked);
	void modelColorRChanged(const int colorR);
	void modelColorGChanged(const int colorG);
	void modelColorBChanged(const int colorB);
	void modelPolygonsChanged(const int polygons);
	void modelPointsChanged(const int points);

private:
	void setPositionX(const double positionX);
	void setPositionY(const double positionY);

	void setColor(const QColor &color);
	void setOutlineColor(const QColor &color);

	static QColor m_defaultModelColor;
	static QColor m_selectedModelColor;

	vtkSmartPointer<vtkPolyData> m_modelData = nullptr;
	vtkSmartPointer<vtkPolyDataMapper> m_modelMapper = nullptr;
	vtkSmartPointer<vtkActor> m_modelActor = nullptr;
	vtkSmartPointer<vtkSmoothPolyDataFilter> m_modelSmoothFilter = nullptr;
	vtkSmartPointer<vtkTransformPolyDataFilter> m_modelFilterTranslate = nullptr;
	vtkSmartPointer<vtkQuadricDecimation> m_modelDecimation = nullptr;
	vtkSmartPointer<vtkTriangleFilter> m_modelTriangles = nullptr;
	vtkSmartPointer<vtkLoopSubdivisionFilter> m_modelLoopFilter = nullptr;
	//vtkSmartPointer<vtkPolyDataNormals> m_modelNormals;
	//vtkSmartPointer<vtkWarpVector> m_modelWarp;
	vtkSmartPointer<vtkOutlineFilter> m_modelOutlineFilter = nullptr;
	vtkSmartPointer<vtkPolyDataMapper> m_modelOutlineMapper = nullptr;
	vtkSmartPointer<vtkActor> m_modelOutlineActor = nullptr;
	vtkSmartPointer<vtkMassProperties> m_modelMassProp = nullptr;

	std::mutex m_propertiesMutex;

	double m_positionX {0.0};
	double m_positionY {0.0};
	double m_positionZ {0.0};

	bool m_selected = false;

	double m_mouseDeltaX = 0.0;
	double m_mouseDeltaY = 0.0;

	int m_Representation = 2;
	double m_TargetReduction = 0.0;
	int m_Iterations = 0;
	double m_Opacity = 1.0;
	int m_Smooth = 0;
	bool m_GI = false;
	int m_colorR = 255;
	int m_colorG = 255;
	int m_colorB = 255;
	int m_polygons = 0;
	int m_points = 0;
	double m_volume = 0.0;
	double m_surfaceArea = 0.0;
	double m_maxAreaOfCell = 0.0;
	double m_minAreaOfCell = 0.0;
};

#endif // MODEL_H
