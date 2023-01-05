#include "CommandModelSave.h"
#include "Model.h"

CommandModelSave::CommandModelSave(QVTKFramebufferObjectRenderer *vtkFboRenderer, const std::shared_ptr<Model> model, QUrl modelPath)
	: m_model{model}
	, m_modelPath{modelPath}
{
	m_vtkFboRenderer = vtkFboRenderer;
}

bool CommandModelSave::isReady() const
{
	return true;
}

void CommandModelSave::saveModel()
{
	// 呼叫Model存檔
	qDebug() << m_modelPath;
	m_model->saveModel(m_modelPath);
}

void CommandModelSave::execute()
{
	this->saveModel();
}