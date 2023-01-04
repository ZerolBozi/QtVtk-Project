#include "CommandModelActor.h"
#include "Model.h"
#include "QVTKFramebufferObjectRenderer.h"

CommandModelActor::CommandModelActor(QVTKFramebufferObjectRenderer *vtkFboRenderer, const ActorParams_t & actorData)
	: m_actorParams{ actorData }
{
	m_vtkFboRenderer = vtkFboRenderer;
}

bool CommandModelActor::isReady() const
{
	return true;
}

void CommandModelActor::setActor()
{
	if (m_actorParams.mode == "setModelRepresentation")
	{
		m_actorParams.model->setModelRepresentation(m_actorParams.valueI);
	}
	else if (m_actorParams.mode == "setModelDecreasePolygons")
	{
		m_actorParams.model->setModelDecreasePolygons(m_actorParams.valueD);
	}
	else if (m_actorParams.mode == "setModelIncreasePolygons")
	{
		m_actorParams.model->setModelIncreasePolygons(m_actorParams.valueI);
	}
	else if (m_actorParams.mode == "setModelOpacity")
	{
		m_actorParams.model->setModelOpacity(m_actorParams.valueD);
	}
	else if (m_actorParams.mode == "setModelGouraudInterpolation")
	{
		m_actorParams.model->setModelGouraudInterpolation(m_actorParams.valueB);
	}
	else if (m_actorParams.mode == "setModelColor")
	{
		m_actorParams.model->setModelColor(m_actorParams.color);
	}
	else if (m_actorParams.mode == "setModelSmooth")
	{
		m_actorParams.model->setModelSmooth(m_actorParams.valueI);
	}
}

void CommandModelActor::execute()
{
	this->setActor();
}