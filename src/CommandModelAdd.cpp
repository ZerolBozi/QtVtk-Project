#include "CommandModelAdd.h"
#include "Model.h"
#include "ProcessingEngine.h"
#include "QVTKFramebufferObjectRenderer.h"


CommandModelAdd::CommandModelAdd(QVTKFramebufferObjectRenderer *vtkFboRenderer, std::shared_ptr<ProcessingEngine> processingEngine, QList<QUrl> modelPaths)
	: m_processingEngine{processingEngine}
	, m_modelPaths{modelPaths}
{
	m_vtkFboRenderer = vtkFboRenderer;
}


void CommandModelAdd::run()
{
	qDebug() << "CommandModelAdd::run()";

	if (m_modelPaths.length() == 1)
	{
		m_model = m_processingEngine->addModel(m_modelPaths[0]);
	}
	else if (m_modelPaths.length() > 1)
	{
		m_model = m_processingEngine->addModels(m_modelPaths);
	}

	m_processingEngine->placeModel(*m_model);

	m_ready = true;
	emit ready();
}


bool CommandModelAdd::isReady() const
{
	return m_ready;
}

void CommandModelAdd::execute()
{
	qDebug() << "CommandModelAdd::execute()";

	m_vtkFboRenderer->addModelActor(m_model);

	if (m_modelPaths.length() == 1)
	{
		m_vtkFboRenderer->setModelFilePathToMap(m_modelPaths[0]);
	}
	else if (m_modelPaths.length() > 1)
	{
		QString tmpPath;
		for (int i = 0; i < m_modelPaths.length(); i++)
		{
			tmpPath += m_modelPaths[i].toString();
			tmpPath += "; ";
		}
		m_vtkFboRenderer->setModelFilePathToMap(QUrl(tmpPath));
	}
	

	emit done();
}
