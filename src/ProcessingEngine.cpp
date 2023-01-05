#include "ProcessingEngine.h"

#include <thread>
#include <memory>

#include <QDebug>
#include <QFileInfo>

#include <vtkAlgorithmOutput.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkSTLReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h> // vtk
#include <vtkXMLPolyDataReader.h> //vtp

#include "Model.h"


ProcessingEngine::ProcessingEngine()
{
}

const std::shared_ptr<Model> &ProcessingEngine::addModel(const QUrl &modelFilePath)
{
	qDebug() << "ProcessingEngine::addModelData()";

	QString modelFilePathExtension = QFileInfo(modelFilePath.toString()).suffix().toLower();

    vtkSmartPointer<vtkOBJReader> objReader = vtkSmartPointer<vtkOBJReader>::New();
    vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
	vtkSmartPointer<vtkPLYReader> plyReader = vtkSmartPointer<vtkPLYReader>::New();
	vtkSmartPointer<vtkPolyDataReader> vtkReader = vtkSmartPointer<vtkPolyDataReader>::New();
	vtkSmartPointer<vtkXMLPolyDataReader> vtpReader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    vtkSmartPointer<vtkPolyData> inputData;

	// 根據不同檔案類型，使用不同reader

	if (modelFilePathExtension == "obj")
	{
		// Read OBJ file
		objReader->SetFileName(modelFilePath.toString().toStdString().c_str());
		objReader->Update();
		inputData = objReader->GetOutput();
	}
	else if (modelFilePathExtension == "stl")
	{
		// Read STL file
		stlReader->SetFileName(modelFilePath.toString().toStdString().c_str());
		stlReader->Update();
		inputData = stlReader->GetOutput();
	}
	else if (modelFilePathExtension == "ply")
	{
		plyReader->SetFileName(modelFilePath.toString().toStdString().c_str());
		plyReader->Update();
		inputData = plyReader->GetOutput();
	}
	else if (modelFilePathExtension == "vtk")
	{
		vtkReader->SetFileName(modelFilePath.toString().toStdString().c_str());
		vtkReader->Update();
		inputData = vtkReader->GetOutput();
	}
	else if (modelFilePathExtension == "vtp")
	{
		vtpReader->SetFileName(modelFilePath.toString().toStdString().c_str());
		vtpReader->Update();
		inputData = vtpReader->GetOutput();
	}

	// Preprocess the polydata
	vtkSmartPointer<vtkPolyData> preprocessedPolydata = preprocessPolydata(inputData);

	// Create Model instance and insert it into the vector
	std::shared_ptr<Model> model = std::make_shared<Model>(preprocessedPolydata);

	m_models.push_back(model);
	return m_models.back();
}

const std::shared_ptr<Model> &ProcessingEngine::addModels(const QList<QUrl> &modelFilesPath)
{
	qDebug() << "ProcessingEngine::addModelDatas()";

	vtkSmartPointer<vtkPolyData> inputData;
	vtkSmartPointer<vtkAppendPolyData> appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
	vtkSmartPointer<vtkCleanPolyData> cleanFilter = vtkSmartPointer<vtkCleanPolyData>::New();

	// 把模型合併成一個
	for (int i = 0; i < modelFilesPath.length(); i++)
	{
		QString modelFilePathExtension = QFileInfo(modelFilesPath[i].toString()).suffix().toLower();

		vtkSmartPointer<vtkOBJReader> objReader = vtkSmartPointer<vtkOBJReader>::New();
		vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
		vtkSmartPointer<vtkPLYReader> plyReader = vtkSmartPointer<vtkPLYReader>::New();
		vtkSmartPointer<vtkPolyDataReader> vtkReader = vtkSmartPointer<vtkPolyDataReader>::New();
		vtkSmartPointer<vtkXMLPolyDataReader> vtpReader = vtkSmartPointer<vtkXMLPolyDataReader>::New();

		if (modelFilePathExtension == "obj")
		{
			// Read OBJ file
			objReader->SetFileName(modelFilesPath[i].toString().toStdString().c_str());
			objReader->Update();
			inputData = objReader->GetOutput();
		}
		else if (modelFilePathExtension == "stl")
		{
			// Read STL file
			stlReader->SetFileName(modelFilesPath[i].toString().toStdString().c_str());
			stlReader->Update();
			inputData = stlReader->GetOutput();
		}
		else if (modelFilePathExtension == "ply")
		{
			plyReader->SetFileName(modelFilesPath[i].toString().toStdString().c_str());
			plyReader->Update();
			inputData = plyReader->GetOutput();
		}
		else if (modelFilePathExtension == "vtk")
		{
			vtkReader->SetFileName(modelFilesPath[i].toString().toStdString().c_str());
			vtkReader->Update();
			inputData = vtkReader->GetOutput();
		}
		else if (modelFilePathExtension == "vtp")
		{
			vtpReader->SetFileName(modelFilesPath[i].toString().toStdString().c_str());
			vtpReader->Update();
			inputData = vtpReader->GetOutput();
		}
		appendFilter->AddInputData(inputData);
	}

	cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
	cleanFilter->Update();
	// Preprocess the polydata
	vtkSmartPointer<vtkPolyData> preprocessedPolydata = preprocessPolydata(cleanFilter->GetOutput());

	// Create Model instance and insert it into the vector
	std::shared_ptr<Model> model = std::make_shared<Model>(preprocessedPolydata);

	m_models.push_back(model);
	return m_models.back();
}

const std::shared_ptr<Model> &ProcessingEngine::createCube(const double x, const double y, const double z)
{
	// 生成立方體
	qDebug() << "ProcessingEngine::createCube()";
	vtkSmartPointer<vtkCubeSource> cube = vtkSmartPointer<vtkCubeSource>::New();
	cube->SetXLength(x);
	cube->SetYLength(y);
	cube->SetZLength(z);
	cube->Update();
	vtkSmartPointer<vtkPolyData> preprocessedPolydata = preprocessPolydata(cube->GetOutput());
	std::shared_ptr<Model> model = std::make_shared<Model>(preprocessedPolydata);
	qDebug() << "ProcessingEngine::createCube() model" << &model;
	m_models.push_back(model);
	return m_models.back();
}

const std::shared_ptr<Model> &ProcessingEngine::createSphare(const double radius)
{
	// 生成球體
	qDebug() << "ProcessingEngine::createSphare()";
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetRadius(radius);
	sphereSource->SetCenter(0, 0, radius);
	sphereSource->Update();
	vtkSmartPointer<vtkPolyData> preprocessedPolydata = preprocessPolydata(sphereSource->GetOutput());
	std::shared_ptr<Model> model = std::make_shared<Model>(preprocessedPolydata);
	qDebug() << "ProcessingEngine::createCube() model" << &model;
	m_models.push_back(model);
	return m_models.back();
}

vtkSmartPointer<vtkPolyData> ProcessingEngine::preprocessPolydata(const vtkSmartPointer<vtkPolyData> inputData) const
{
	// 預處理PolyData
	// 設定模型位置
	// Center the polygon
	double center[3];
	inputData->GetCenter(center);

	vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
	translation->Translate(-center[0], -center[1], -center[2]);

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(inputData);
	transformFilter->SetTransform(translation);
	transformFilter->Update();

	// Normals - For the Gouraud interpolation to work
	vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
	normals->SetInputData(transformFilter->GetOutput());
	normals->ComputePointNormalsOn();
	normals->Update();

	return normals->GetOutput();
}

void ProcessingEngine::placeModel(Model &model) const
{
	qDebug() << "ProcessingEngine::placeModel()";
	// 將模型設定在原點0,0
	model.translateToPosition(0, 0);
}

// 舊功能函數，原作者使用的，沒刪除
void ProcessingEngine::setModelsRepresentation(const int modelsRepresentationOption) const
{
	for (const std::shared_ptr<Model>& model : m_models)
	{
		model->getModelActor()->GetProperty()->SetRepresentation(modelsRepresentationOption);
	}
}

void ProcessingEngine::setModelsOpacity(const double modelsOpacity) const
{
	for (const std::shared_ptr<Model>& model : m_models)
	{
		model->getModelActor()->GetProperty()->SetOpacity(modelsOpacity);
	}
}

void ProcessingEngine::setModelsGouraudInterpolation(const bool enableGouraudInterpolation) const
{
	for (const std::shared_ptr<Model>& model : m_models)
	{
		if (enableGouraudInterpolation)
		{
			model->getModelActor()->GetProperty()->SetInterpolationToGouraud();
		}
		else
		{
			model->getModelActor()->GetProperty()->SetInterpolationToFlat();
		}
	}
}

void ProcessingEngine::updateModelsColor() const
{
	for (const std::shared_ptr<Model>& model : m_models)
	{
		model->updateModelColor();
	}
}

std::vector<std::shared_ptr<Model>> ProcessingEngine::getAllModels() const
{
	return m_models;
}

std::shared_ptr<Model> ProcessingEngine::getModelFromActor(const vtkSmartPointer<vtkActor> modelActor) const
{
	for (const std::shared_ptr<Model> &model : m_models)
	{
		if (model->getModelActor() == modelActor)
		{
			return model;
		}
	}

	// Raise exception instead
	return nullptr;
}