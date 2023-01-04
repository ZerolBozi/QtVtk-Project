#ifndef COMMANDMODELSAVE_H
#define COMMANDMODELSAVE_H
#include <memory>

#include <QUrl>
#include "CommandModel.h"

class Model;
class QVTKFramebufferObjectRenderer;

class CommandModelSave : public CommandModel
{
public:
	CommandModelSave(QVTKFramebufferObjectRenderer *vtkFboRenderer, const std::shared_ptr<Model> model, QUrl modelPath);

	bool isReady() const override;
	void execute() override;

private:
	void saveModel();
	std::shared_ptr<Model> m_model;
	QUrl m_modelPath;
};

#endif
