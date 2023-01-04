#ifndef COMMMANDMODELACTOR_H
#define COMMMANDMODELACTOR_H

#include <memory>

#include <QColor>
#include "CommandModel.h"


class Model;
class QVTKFramebufferObjectRenderer;

class CommandModelActor : public CommandModel
{
public:
	typedef struct
	{
		std::shared_ptr<Model> model;
		std::string mode;
		int valueI{ 0 };
		double valueD{ 0 };
		float valueF{ 0 };
		bool valueB = false;
		QColor color;

	} ActorParams_t;

	CommandModelActor(QVTKFramebufferObjectRenderer *vtkFboRenderer, const ActorParams_t & actorVector);

	bool isReady() const override;
	void execute() override;

private:
	void setActor();

	ActorParams_t m_actorParams;
};

#endif