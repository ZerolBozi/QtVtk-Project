#ifndef PROCESSINGENGINE_H
#define PROCESSINGENGINE_H

#include <array>
#include <cstdint>
#include <vector>
#include <mutex>
#include <memory>

#include <QUrl>

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkCubeSource.h>
#include <vtkSphereSource.h>


class Model;

class ProcessingEngine
{
	public:
		ProcessingEngine();

		const std::shared_ptr<Model>& addModel(const QUrl &modelFilePath);
		const std::shared_ptr<Model>& addModels(const QList<QUrl> &modelFilesPath);

		const std::shared_ptr<Model> &createCube(const double x, const double y, const double z);
		const std::shared_ptr<Model> &createSphare(const double radius);

		void placeModel(Model &model) const;

		void setModelsRepresentation(const int modelsRepresentationOption) const;
		void setModelsOpacity(const double modelsOpacity) const;
		void setModelsGouraudInterpolation(const bool enableGouraudInterpolation) const;
		void updateModelsColor() const;

		std::vector<std::shared_ptr<Model>> getAllModels() const;
		std::shared_ptr<Model> getModelFromActor(const vtkSmartPointer<vtkActor> modelActor) const;

	private:
		vtkSmartPointer<vtkPolyData> preprocessPolydata(const vtkSmartPointer<vtkPolyData> inputData) const;

		std::vector<std::shared_ptr<Model>> m_models;
};

#endif // PROCESSINGENGINE_H
