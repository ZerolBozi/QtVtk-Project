#include <QDebug>

#include <vtkAlgorithmOutput.h>
#include <vtkProperty.h>
#include <vtkTransform.h>
#include <vtkSTLWriter.h>
#include <vtkOBJWriter.h>
#include <vtkPLYWriter.h>
#include <vtkPolyDataWriter.h> // vtk
#include <vtkXMLPolyDataWriter.h> //vtp

#include "Model.h"

QColor Model::m_defaultModelColor = QColor{"#ffffff"};
QColor Model::m_selectedModelColor = QColor{"#ff0000"}; // Red

Model::Model(vtkSmartPointer<vtkPolyData> modelData)
	: m_modelData{modelData}
{
	// 串連filter -> setMapper -> setActor
	// Place model with lower Z bound at zero
	m_positionZ = -m_modelData->GetBounds()[4];

	vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
	translation->Translate(m_positionX, m_positionY, m_positionZ);

	m_modelFilterTranslate = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	m_modelFilterTranslate->SetInputData(m_modelData);
	m_modelFilterTranslate->SetTransform(translation);
	m_modelFilterTranslate->Update();

	// Model Triangles
	m_modelTriangles = vtkSmartPointer<vtkTriangleFilter>::New();
	m_modelTriangles->SetInputConnection(m_modelFilterTranslate->GetOutputPort());
	m_modelTriangles->Update();

	// Model Decimation
	m_modelDecimation = vtkSmartPointer<vtkQuadricDecimation>::New();
	m_modelDecimation->SetInputConnection(m_modelTriangles->GetOutputPort());
	m_modelDecimation->SetTargetReduction(0);
	m_modelDecimation->AttributeErrorMetricOn();
	m_modelDecimation->VolumePreservationOn();
	m_modelDecimation->Update();

	// Model Loop
	m_modelLoopFilter = vtkSmartPointer<vtkLoopSubdivisionFilter>::New();
	m_modelLoopFilter->SetInputConnection(m_modelDecimation->GetOutputPort());
	m_modelLoopFilter->SetNumberOfSubdivisions(0);
	m_modelLoopFilter->Update();

	// Model MassProp
	m_modelMassProp = vtkSmartPointer<vtkMassProperties>::New();
	m_modelMassProp->SetInputConnection(m_modelTriangles->GetOutputPort());
	m_modelMassProp->Update();

	// Model Smooth
	m_modelSmoothFilter = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
	m_modelSmoothFilter->SetInputConnection(m_modelLoopFilter->GetOutputPort());
	m_modelSmoothFilter->SetNumberOfIterations(0);
	m_modelSmoothFilter->Update();

	// Model Outline
	m_modelOutlineFilter = vtkSmartPointer<vtkOutlineFilter>::New();
	m_modelOutlineFilter->SetInputConnection(m_modelSmoothFilter->GetOutputPort());
	m_modelOutlineFilter->Update();

	// Model Outline Mapper
	m_modelOutlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_modelOutlineMapper->SetInputConnection(m_modelOutlineFilter->GetOutputPort());
	m_modelOutlineMapper->Update();

	// Model Outline Actor
	m_modelOutlineActor = vtkSmartPointer<vtkActor>::New();
	m_modelOutlineActor->SetMapper(m_modelOutlineMapper);
	this->setOutlineColor(m_selectedModelColor);
	
	// Model Mapper
	m_modelMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_modelMapper->SetInputConnection(m_modelSmoothFilter->GetOutputPort());
	m_modelMapper->ScalarVisibilityOff();
	m_modelMapper->Update();

	// Model Actor
	m_modelActor = vtkSmartPointer<vtkActor>::New();
	m_modelActor->SetMapper(m_modelMapper);
	m_modelActor->GetProperty()->SetInterpolationToFlat();

	m_modelActor->GetProperty()->SetAmbient(0.1);
	m_modelActor->GetProperty()->SetDiffuse(0.7);
	m_modelActor->GetProperty()->SetSpecular(0.3);
	this->setColor(m_defaultModelColor);

	m_modelActor->SetPosition(0.0, 0.0, 0.0);
}


const vtkSmartPointer<vtkActor> &Model::getModelActor() const
{
	return m_modelActor;
}

const vtkSmartPointer<vtkActor> &Model::getModelOutlineActor() const
{
	return m_modelOutlineActor;
}

double Model::getPositionX()
{
	m_propertiesMutex.lock();
	double positionX = m_positionX;
	m_propertiesMutex.unlock();
	return positionX;
}

double Model::getPositionY()
{
	m_propertiesMutex.lock();
	double positionY = m_positionY;
	m_propertiesMutex.unlock();
	return positionY;
}

void Model::getModelData()
{
	// 取得模型數據，並將數據設定在檔案變數
	// representation
	int t_representation = m_modelActor->GetProperty()->GetRepresentation();
	if (t_representation != m_Representation)
	{
		m_Representation = t_representation;
	}

	// decreasePolygons
	double t_targetReduction = m_modelDecimation->GetTargetReduction();
	if (t_targetReduction != m_TargetReduction)
	{
		m_TargetReduction = t_targetReduction;
	}

	// increasePolygons
	int t_Iterations = m_modelLoopFilter->GetNumberOfSubdivisions();
	if (t_Iterations != m_Iterations)
	{
		m_Iterations = t_Iterations;
	}

	// opacity
	double t_opacity = m_modelActor->GetProperty()->GetOpacity();
	if (t_opacity != m_Opacity)
	{
		m_Opacity = t_opacity;
	}

	// smooth
	int t_smooth = m_modelSmoothFilter->GetNumberOfIterations();
	if (t_smooth != m_Smooth)
	{
		m_Smooth = t_smooth;
	}

	// gouraud interpolation
	bool t_GI = bool(m_modelActor->GetProperty()->GetInterpolation());
	if (t_GI != m_GI)
	{
		m_GI = t_GI;
	}

	// color
	QColor &qcolor = QColor{};
	double colorData[3] = { 0 };
	m_modelActor->GetProperty()->GetColor(colorData);
	qcolor.setRgbF(colorData[0], colorData[1], colorData[2]);
	if (qcolor.red() != m_colorR)
	{
		m_colorR = qcolor.red();
	}
	if (qcolor.green() != m_colorG)
	{
		m_colorG = qcolor.green();
	}
	if (qcolor.blue() != m_colorB)
	{
		m_colorB = qcolor.blue();
	}

	// polygons
	int t_polys = m_modelSmoothFilter->GetOutput()->GetNumberOfPolys();
	if (t_polys != m_polygons)
	{
		m_polygons = t_polys;
	}

	// points
	int t_points = m_modelSmoothFilter->GetOutput()->GetNumberOfPoints();
	if (t_points != m_points)
	{
		m_points = t_points;
	}

	// volume
	double t_volume = m_modelMassProp->GetVolume();
	if (t_volume != m_volume)
	{
		m_volume = t_volume;
	}

	// surfaceArea
	double t_surfaceArea = m_modelMassProp->GetSurfaceArea();
	if (t_surfaceArea != m_surfaceArea)
	{
		m_surfaceArea = t_surfaceArea;
	}
	
	// maxAreaOfCell
	double t_maxAreaOfCell = m_modelMassProp->GetMaxCellArea();
	if (t_maxAreaOfCell != m_maxAreaOfCell)
	{
		m_maxAreaOfCell = t_maxAreaOfCell;
	}

	// minAreaOfCell
	double t_minAreaOfCell = m_modelMassProp->GetMinCellArea();
	if (t_minAreaOfCell != m_minAreaOfCell)
	{
		m_minAreaOfCell = t_minAreaOfCell;
	}
}

void Model::setPositionX(const double positionX)
{
	if (m_positionX != positionX)
	{
		m_positionX = positionX;
		emit positionXChanged(m_positionX);
	}
}

void Model::setPositionY(const double positionY)
{
	if (m_positionY != positionY)
	{
		m_positionY = positionY;
		emit positionYChanged(m_positionY);
	}
}

void Model::translateToPosition(const double x, const double y)
{
	if (m_positionX == x && m_positionY == y)
	{
		return;
	}

	m_propertiesMutex.lock();
	this->setPositionX(x);
	this->setPositionY(y);
	m_propertiesMutex.unlock();

	vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
	translation->Translate(m_positionX, m_positionY, m_positionZ);
	m_modelFilterTranslate->SetTransform(translation);
	m_modelFilterTranslate->Update();

	emit positionXChanged(m_positionX);
	emit positionYChanged(m_positionY);
}


void Model::setSelected(const bool selected)
{
	if (m_selected != selected)
	{
		m_selected = selected;

		//this->updateModelColor();
	}
}

void Model::saveModel(const QUrl modelPath)
{
	// 另存模型
	QString modelFilePathExtension = QFileInfo(modelPath.toString()).suffix().toLower();

	vtkSmartPointer<vtkSTLWriter> stlWriter = vtkSmartPointer<vtkSTLWriter>::New();
	vtkSmartPointer<vtkOBJWriter> objWriter = vtkSmartPointer<vtkOBJWriter>::New();
	vtkSmartPointer<vtkPLYWriter> plyWriter = vtkSmartPointer<vtkPLYWriter>::New();
	vtkSmartPointer<vtkXMLPolyDataWriter> vtpWriter = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
	vtkSmartPointer<vtkPolyDataWriter> vtkWriter = vtkSmartPointer<vtkPolyDataWriter>::New();

	if (modelFilePathExtension == "stl")
	{
		stlWriter->SetFileName(modelPath.toString().toStdString().c_str());
		stlWriter->SetFileTypeToBinary();
		stlWriter->SetInputConnection(m_modelSmoothFilter->GetOutputPort());
		stlWriter->Write();
	}
	else if (modelFilePathExtension == "obj")
	{
		objWriter->SetFileName(modelPath.toString().toStdString().c_str());
		objWriter->SetInputConnection(m_modelSmoothFilter->GetOutputPort());
		objWriter->Write();
	}
	else if (modelFilePathExtension == "ply")
	{
		plyWriter->SetFileName(modelPath.toString().toStdString().c_str());
		plyWriter->SetFileTypeToBinary();
		plyWriter->SetInputConnection(m_modelSmoothFilter->GetOutputPort());
		plyWriter->Write();
	}
	else if (modelFilePathExtension == "vtp")
	{
		vtpWriter->SetFileName(modelPath.toString().toStdString().c_str());
		vtpWriter->SetInputConnection(m_modelSmoothFilter->GetOutputPort());
		vtpWriter->Write();
	}
	else if (modelFilePathExtension == "vtk")
	{
		vtkWriter->SetFileName(modelPath.toString().toStdString().c_str());
		vtkWriter->SetFileTypeToBinary();
		vtkWriter->SetInputConnection(m_modelSmoothFilter->GetOutputPort());
		vtkWriter->Write();
	}
}

void Model::setModelRepresentation(const int modelsRepresentationOption)
{
	if (modelsRepresentationOption != m_Representation)
	{
		// 功能被使用了，所以數值不同，設定完功能後emit
		m_Representation = modelsRepresentationOption;
		m_modelActor->GetProperty()->SetRepresentation(m_Representation);
		emit modelRepresentationChanged(m_Representation);
	}
}

void Model::setModelDecreasePolygons(const double modelPolygons)
{
	if (modelPolygons != m_TargetReduction)
	{
		// 功能被使用了，所以數值不同，設定完功能後emit
		// Filter跟Mapper要Update更新一下
		m_TargetReduction = modelPolygons;
		m_modelDecimation->SetTargetReduction(m_TargetReduction);
		m_modelDecimation->Update();
		m_modelMapper->Update();
		emit modelDecreasePolygonsChanged(m_TargetReduction);
		emit modelPolygonsChanged(m_modelDecimation->GetOutput()->GetNumberOfPolys());
		emit modelPointsChanged(m_modelDecimation->GetOutput()->GetNumberOfPoints());
	}
}

void Model::setModelIncreasePolygons(const int iterations)
{
	if (iterations != m_Iterations)
	{
		// 功能被使用了，所以數值不同，設定完功能後emit
		// Filter跟Mapper要Update更新一下
		m_Iterations = iterations;
		m_modelLoopFilter->SetNumberOfSubdivisions(iterations);
		m_modelLoopFilter->Update();
		m_modelMapper->Update();
		emit modelIncreasePolygonsChanged(m_Iterations);
		emit modelPolygonsChanged(m_modelLoopFilter->GetOutput()->GetNumberOfPolys());
		emit modelPointsChanged(m_modelLoopFilter->GetOutput()->GetNumberOfPoints());
	}
}

void Model::setModelOpacity(const double modelsOpacity)
{
	if (modelsOpacity != m_Opacity)
	{
		// 功能被使用了，所以數值不同，設定完功能後emit
		m_Opacity = modelsOpacity;
		m_modelActor->GetProperty()->SetOpacity(m_Opacity);
		emit modelOpacityChanged(m_Opacity);
	}
}

void Model::setModelGouraudInterpolation(const bool enableGouraudInterpolation)
{
	if (enableGouraudInterpolation != m_GI)
	{
		m_GI = enableGouraudInterpolation;
		if (m_GI)
		{
			m_modelActor->GetProperty()->SetInterpolationToGouraud();
		}
		else
		{
			m_modelActor->GetProperty()->SetInterpolationToFlat();
		}
		// 功能被使用了，所以數值不同，設定完功能後emit
		emit modelGIChanged(m_GI);
	}
}

void Model::setModelColor(const QColor &color)
{
	// 功能被使用了，所以數值不同，設定完功能後emit
	if (color.red() != m_colorR)
	{
		m_colorR = color.red();
		emit modelColorRChanged(m_colorR);
	}
	if (color.green() != m_colorG)
	{
		m_colorG = color.green();
		emit modelColorGChanged(m_colorG);
	}
	if (color.blue() != m_colorB)
	{
		m_colorB = color.blue();
		emit modelColorBChanged(m_colorB);
	}
	m_modelActor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
}

void Model::setModelSmooth(const int value)
{
	if (value != m_Smooth)
	{
		// 功能被使用了，所以數值不同，設定完功能後emit
		// Filter跟Mapper要Update更新一下
		m_Smooth = value;
		m_modelSmoothFilter->SetNumberOfIterations(m_Smooth);
		m_modelSmoothFilter->Update();
		m_modelMapper->Update();
		emit modelSmoothChanged(m_Smooth);
	}
}

void Model::setSelectedModelColor(const QColor &selectedModelColor)
{
	m_selectedModelColor = selectedModelColor;
}

void Model::updateModelColor()
{
	if (m_selected)
	{
		this->setColor(m_selectedModelColor);
	}
	else
	{
		this->setColor(m_defaultModelColor);
	}
}

void Model::setColor(const QColor &color)
{
	m_modelActor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
}

void Model::setOutlineColor(const QColor &color)
{
	m_modelOutlineActor->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
}


const double Model::getMouseDeltaX() const
{
	return m_mouseDeltaX;
}

const double Model::getMouseDeltaY() const
{
	return m_mouseDeltaY;
}

void Model::setMouseDeltaXY(const double deltaX, const double deltaY)
{
	m_mouseDeltaX = deltaX;
	m_mouseDeltaY = deltaY;
}

// 返回模型數據給handler

int Model::getModelRepresentation()
{
	return m_Representation;
}

double Model::getModelTargetReduction()
{
	return m_TargetReduction;
}

int Model::getModelIncreasePolygonsIterations()
{
	return m_Iterations;
}

double Model::getModelOpacity()
{
	return m_Opacity;
}

int Model::getModelSmooth()
{
	return m_Smooth;
}

bool Model::getModelGI()
{
	return m_GI;
}

int Model::getModelColorR()
{
	return m_colorR;
}

int Model::getModelColorG()
{
	return m_colorG;
}

int Model::getModelColorB()
{
	return m_colorB;
}

int Model::getModelPolygons()
{
	return m_polygons;
}

int Model::getModelPoints()
{
	return m_points;
}

double Model::getModelVolume()
{
	return m_volume;
}

double Model::getModelSurfaceArea()
{
	return m_surfaceArea;
}

double Model::getModelMaxAreaOfCell()
{
	return m_maxAreaOfCell;
}

double Model::getModelMinAreaOfCell()
{
	return m_minAreaOfCell;
}