#include <queue>

#include <QQuickWindow>

#include <vtkBoundedPlanePointPlacer.h>
#include <vtkCamera.h>
#include <vtkCaptionActor2D.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLight.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyLine.h>
#include <vtkSTLReader.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#include "CommandModel.h"
#include "Model.h"
#include "ProcessingEngine.h"
#include "QVTKFramebufferObjectItem.h"
#include "QVTKFramebufferObjectRenderer.h"

QVTKFramebufferObjectRenderer::QVTKFramebufferObjectRenderer()
{
	// Renderer
	QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
	m_vtkRenderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
	m_renderer = vtkSmartPointer<vtkRenderer>::New();
	m_vtkRenderWindow->AddRenderer(m_renderer);

	// Interactor
	m_vtkRenderWindowInteractor = vtkSmartPointer<vtkGenericRenderWindowInteractor>::New();
	m_vtkRenderWindowInteractor->EnableRenderOff();
	m_vtkRenderWindow->SetInteractor(m_vtkRenderWindowInteractor);

	// Initialize the OpenGL context for the renderer
	m_vtkRenderWindow->OpenGLInitContext();

	// Interactor Style
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	style->SetDefaultRenderer(m_renderer);
	style->SetMotionFactor(10.0);
	m_vtkRenderWindowInteractor->SetInteractorStyle(style);

	// Picker
	m_picker = vtkSmartPointer<vtkCellPicker>::New();
	m_picker->SetTolerance(0.0);

	update();
}

void QVTKFramebufferObjectRenderer::setProcessingEngine(const std::shared_ptr<ProcessingEngine> processingEngine)
{
	m_processingEngine = std::shared_ptr<ProcessingEngine>(processingEngine);
}

void QVTKFramebufferObjectRenderer::synchronize(QQuickFramebufferObject *item)
{
	// For the first synchronize
	if (!m_vtkFboItem)
	{
		m_vtkFboItem = static_cast<QVTKFramebufferObjectItem *>(item);
	}

	if (!m_vtkFboItem->isInitialized())
	{
		m_vtkFboItem->setVtkFboRenderer(this);

		emit m_vtkFboItem->rendererInitialized();
	}

	int *rendererSize = m_vtkRenderWindow->GetSize();

	if (m_vtkFboItem->width() != rendererSize[0] || m_vtkFboItem->height() != rendererSize[1])
	{
		m_vtkRenderWindow->SetSize(m_vtkFboItem->width(), m_vtkFboItem->height());
	}

	// Copy mouse events
	if (!m_vtkFboItem->getLastMouseLeftButton()->isAccepted())
	{
		m_mouseLeftButton = std::make_shared<QMouseEvent>(*m_vtkFboItem->getLastMouseLeftButton());
		m_vtkFboItem->getLastMouseLeftButton()->accept();
	}

	if (!m_vtkFboItem->getLastMouseButton()->isAccepted())
	{
		m_mouseEvent = std::make_shared<QMouseEvent>(*m_vtkFboItem->getLastMouseButton());
		m_vtkFboItem->getLastMouseButton()->accept();
	}

	if (!m_vtkFboItem->getLastMoveEvent()->isAccepted())
	{
		m_moveEvent = std::make_shared<QMouseEvent>(*m_vtkFboItem->getLastMoveEvent());
		m_vtkFboItem->getLastMoveEvent()->accept();
	}

	if (!m_vtkFboItem->getLastWheelEvent()->isAccepted())
	{
		m_wheelEvent = std::make_shared<QWheelEvent>(*m_vtkFboItem->getLastWheelEvent());
		m_vtkFboItem->getLastWheelEvent()->accept();
	}
	
	// Get extra data
	//m_modelsRepresentationOption = m_vtkFboItem->getModelsRepresentation();
	//m_modelsOpacity = m_vtkFboItem->getModelsOpacity();
	//m_modelsGouraudInterpolation = m_vtkFboItem->getGourauInterpolation();
	//Model::setSelectedModelColor(QColor(m_vtkFboItem->getModelColorR(), m_vtkFboItem->getModelColorG(), m_vtkFboItem->getModelColorB()));
}

void QVTKFramebufferObjectRenderer::render()
{
	m_vtkRenderWindow->PushState();
	this->openGLInitState();
	m_vtkRenderWindow->Start();

	if (m_firstRender)
	{
		this->initScene();
		m_firstRender = false;
	}

	// Process camera related commands

	// Process mouse event
	if (m_mouseEvent && !m_mouseEvent->isAccepted())
	{
		m_vtkRenderWindowInteractor->SetEventInformationFlipY(m_mouseEvent->x(), m_mouseEvent->y(),
															  (m_mouseEvent->modifiers() & Qt::ControlModifier) > 0 ? 1 : 0,
															  (m_mouseEvent->modifiers() & Qt::ShiftModifier) > 0 ? 1 : 0, 0,
															  m_mouseEvent->type() == QEvent::MouseButtonDblClick ? 1 : 0);

		if (m_mouseEvent->type() == QEvent::MouseButtonPress)
		{
			// 把右鍵改為原本左鍵的功能
			m_vtkRenderWindowInteractor->InvokeEvent(vtkCommand::LeftButtonPressEvent, m_mouseEvent.get());
		}
		else if (m_mouseEvent->type() == QEvent::MouseButtonRelease)
		{
			// 把右鍵改為原本左鍵的功能
			m_vtkRenderWindowInteractor->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, m_mouseEvent.get());
		}

		m_mouseEvent->accept();
	}

	// Process move event
	if (m_moveEvent && !m_moveEvent->isAccepted())
	{
		if (m_moveEvent->type() == QEvent::MouseMove && m_moveEvent->buttons() & Qt::RightButton)
		{
			m_vtkRenderWindowInteractor->SetEventInformationFlipY(m_moveEvent->x(), m_moveEvent->y(),
																  (m_moveEvent->modifiers() & Qt::ControlModifier) > 0 ? 1 : 0,
																  (m_moveEvent->modifiers() & Qt::ShiftModifier) > 0 ? 1 : 0, 0,
																  m_moveEvent->type() == QEvent::MouseButtonDblClick ? 1 : 0);

			// 把右鍵移動改為原本左鍵移動的功能
			m_vtkRenderWindowInteractor->InvokeEvent(vtkCommand::MouseMoveEvent, m_moveEvent.get());
		}

		m_moveEvent->accept();
	}

	// Process wheel event
	if (m_wheelEvent && !m_wheelEvent->isAccepted())
	{
		if (m_wheelEvent->delta() > 0)
		{
			m_vtkRenderWindowInteractor->InvokeEvent(vtkCommand::MouseWheelForwardEvent, m_wheelEvent.get());
		}
		else if (m_wheelEvent->delta() < 0)
		{
			m_vtkRenderWindowInteractor->InvokeEvent(vtkCommand::MouseWheelBackwardEvent, m_wheelEvent.get());
		}

		m_wheelEvent->accept();
	}

	// Process model related commands

	// Select model

	if (m_mouseLeftButton && !m_mouseLeftButton->isAccepted())
	{
		this->selectModel(m_mouseLeftButton->x(), m_mouseLeftButton->y());
		m_mouseLeftButton->accept();
	}
	
	// Model transformations

	CommandModel *command;
	while (!m_vtkFboItem->isCommandsQueueEmpty())
	{
		m_vtkFboItem->lockCommandsQueueMutex();

		command = m_vtkFboItem->getCommandsQueueFront();
		if (!command->isReady())
		{
			m_vtkFboItem->unlockCommandsQueueMutex();
			break;
		}
		m_vtkFboItem->commandsQueuePop();

		m_vtkFboItem->unlockCommandsQueueMutex();

		command->execute();
	}

	// Reset the view-up vector. This improves the interaction of the camera with the plate.
	m_renderer->GetActiveCamera()->SetViewUp(0.0, 0.0, 1.0);

	// Extra actions
	//m_processingEngine->setModelsRepresentation(m_modelsRepresentationOption);
	//m_processingEngine->setModelsOpacity(m_modelsOpacity);
	//m_processingEngine->setModelsGouraudInterpolation(m_modelsGouraudInterpolation);
	//m_processingEngine->updateModelsColor();

	// Render
	m_vtkRenderWindow->Render();
	m_vtkRenderWindow->PopState();

	m_vtkFboItem->window()->resetOpenGLState();
}

void QVTKFramebufferObjectRenderer::openGLInitState()
{
	m_vtkRenderWindow->OpenGLInitState();
	m_vtkRenderWindow->MakeCurrent();
	QOpenGLFunctions::initializeOpenGLFunctions();
	QOpenGLFunctions::glUseProgram(0);
}

QOpenGLFramebufferObject *QVTKFramebufferObjectRenderer::createFramebufferObject(const QSize &size)
{
	QSize macSize = QSize(size.width() / 2, size.height() / 2);

	QOpenGLFramebufferObjectFormat format;
	format.setAttachment(QOpenGLFramebufferObject::Depth);

#ifdef Q_OS_MAC
	std::unique_ptr<QOpenGLFramebufferObject> framebufferObject(new QOpenGLFramebufferObject(macSize, format));
#else
	std::unique_ptr<QOpenGLFramebufferObject> framebufferObject(new QOpenGLFramebufferObject(size, format));
#endif
	m_vtkRenderWindow->SetBackLeftBuffer(GL_COLOR_ATTACHMENT0);
	m_vtkRenderWindow->SetFrontLeftBuffer(GL_COLOR_ATTACHMENT0);
	m_vtkRenderWindow->SetBackBuffer(GL_COLOR_ATTACHMENT0);
	m_vtkRenderWindow->SetFrontBuffer(GL_COLOR_ATTACHMENT0);
	m_vtkRenderWindow->SetSize(framebufferObject->size().width(), framebufferObject->size().height());
	m_vtkRenderWindow->SetOffScreenRendering(true);
	m_vtkRenderWindow->Modified();

	return framebufferObject.release();
}

void QVTKFramebufferObjectRenderer::initScene()
{
	qDebug() << "QVTKFramebufferObjectRenderer::initScene()";

	m_vtkRenderWindow->SetOffScreenRendering(true);

	// Top background color
	double r2 = 245 / 255.0;
	double g2 = 245 / 255.0;
	double b2 = 245 / 255.0;

	// Bottom background color
	double r1 = 170 / 255.0;
	double g1 = 170 / 255.0;
	double b1 = 170 / 255.0;

	m_renderer->SetBackground(r2, g2, b2);
	m_renderer->SetBackground2(r1, g1, b1);
	m_renderer->GradientBackgroundOn();

	// Axes
	m_axes = vtkSmartPointer<vtkAxesActor>::New();
	double axes_length = 20.0;
	int16_t axes_label_font_size = 20;
	m_axes->SetTotalLength(axes_length, axes_length, axes_length);
	m_axes->GetXAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
	m_axes->GetYAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
	m_axes->GetZAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
	m_axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(axes_label_font_size);
	m_axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(axes_label_font_size);
	m_axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetFontSize(axes_label_font_size);
	m_renderer->AddActor(m_axes);

	// Platform
	this->generatePlatform();

	// Initial camera position
	this->resetCamera();
}

void QVTKFramebufferObjectRenderer::generatePlatform()
{
	qDebug() << "QVTKFramebufferObjectRenderer::generatePlatform()";

	// Platform Model
	vtkSmartPointer<vtkPolyDataMapper> platformModelMapper = vtkSmartPointer<vtkPolyDataMapper>::New();

	m_platformModel = vtkSmartPointer<vtkCubeSource>::New();
	platformModelMapper->SetInputConnection(m_platformModel->GetOutputPort());

	m_platformModelActor = vtkSmartPointer<vtkActor>::New();
	m_platformModelActor->SetMapper(platformModelMapper);
	m_platformModelActor->GetProperty()->SetColor(1, 1, 1);
	m_platformModelActor->GetProperty()->LightingOn();
	m_platformModelActor->GetProperty()->SetOpacity(1);
	m_platformModelActor->GetProperty()->SetAmbient(0.45);
	m_platformModelActor->GetProperty()->SetDiffuse(0.4);

	m_platformModelActor->PickableOff();
	m_renderer->AddActor(m_platformModelActor);

	// Platform Grid
	m_platformGrid = vtkSmartPointer<vtkPolyData>::New();

	vtkSmartPointer<vtkPolyDataMapper> platformGridMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	platformGridMapper->SetInputData(m_platformGrid);

	m_platformGridActor = vtkSmartPointer<vtkActor>::New();
	m_platformGridActor->SetMapper(platformGridMapper);
	m_platformGridActor->GetProperty()->LightingOff();
	m_platformGridActor->GetProperty()->SetColor(0.45, 0.45, 0.45);
	m_platformGridActor->GetProperty()->SetOpacity(1);
	m_platformGridActor->PickableOff();
	m_renderer->AddActor(m_platformGridActor);

	this->updatePlatform();
}

void QVTKFramebufferObjectRenderer::updatePlatform()
{
	qDebug() << "QVTKFramebufferObjectRenderer::updatePlatform()";

	// Platform Model

	if (m_platformModel)
	{
		m_platformModel->SetXLength(m_platformWidth);
		m_platformModel->SetYLength(m_platformDepth);
		m_platformModel->SetZLength(m_platformThickness);
		m_platformModel->SetCenter(0.0, 0.0, -m_platformThickness / 2);
	}

	// Platform Grid
	vtkSmartPointer<vtkPoints> gridPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> gridCells = vtkSmartPointer<vtkCellArray>::New();

	for (int16_t i = -m_platformWidth / 2; i <= m_platformWidth / 2; i += m_gridSize)
	{
		createLine(i, -m_platformDepth / 2, m_gridBottomHeight, i, m_platformDepth / 2, m_gridBottomHeight, gridPoints, gridCells);
	}

	for (int16_t i = -m_platformDepth / 2; i <= m_platformDepth / 2; i += m_gridSize)
	{
		createLine(-m_platformWidth / 2, i, m_gridBottomHeight, m_platformWidth / 2, i, m_gridBottomHeight, gridPoints, gridCells);
	}

	m_platformGrid->SetPoints(gridPoints);
	m_platformGrid->SetLines(gridCells);
}

void QVTKFramebufferObjectRenderer::createLine(const double x1, const double y1, const double z1, const double x2, const double y2, const double z2, vtkSmartPointer<vtkPoints> points, vtkSmartPointer<vtkCellArray> cells)
{
	vtkSmartPointer<vtkPolyLine> line;
	line = vtkSmartPointer<vtkPolyLine>::New();
	line->GetPointIds()->SetNumberOfIds(2);

	vtkIdType id_1, id_2;
	id_1 = points->InsertNextPoint(x1, y1, z1);
	id_2 = points->InsertNextPoint(x2, y2, z2);

	line->GetPointIds()->SetId(0, id_1);
	line->GetPointIds()->SetId(1, id_2);

	cells->InsertNextCell(line);
}

void QVTKFramebufferObjectRenderer::addModelActor(const std::shared_ptr<Model> model)
{
	m_renderer->AddActor(model->getModelActor());
	m_tmpAddActor = model->getModelActor();
	m_modelCount += 1;
	emit isModelCountChanged(m_modelCount);
	qDebug() << "QVTKFramebufferObjectRenderer::addModelActor(): Model added " << model.get();
}

void QVTKFramebufferObjectRenderer::selectModel(const int16_t x, const int16_t y)
{
	qDebug() << "QVTKFramebufferObjectRenderer::selectModel()";

	// Compensate the y-axis flip for the picking
	m_picker->Pick(x, m_renderer->GetSize()[1] - y, 0, m_renderer);

	// Get pick position
	double clickPosition[3];
	m_picker->GetPickPosition(clickPosition);
	m_clickPositionZ = clickPosition[2];

	// 模型坐標位置
	//qDebug() << "QVTKFramebufferObjectRenderer::selectModel() " << clickPosition[0] << clickPosition[1] << clickPosition[2];

	if (m_selectedActor == m_picker->GetActor())
	{
		if (m_selectedModel)
		{
			m_selectedModel->setMouseDeltaXY(clickPosition[0] - m_selectedModel->getPositionX(), clickPosition[1] - m_selectedModel->getPositionY());
		}
		return;
	}

	// Disconnect signals
	if (m_selectedModel)
	{
		this->clearSelectedModel(0);
	}

	// Pick the new actor
	m_selectedActor = m_picker->GetActor();

	m_selectedModel = this->getSelectedModelNoLock();

	if (m_selectedActor)
	{
		qDebug() << "QVTKFramebufferObjectRenderer::selectModel(): picked actor" << m_selectedActor;

		auto iter = m_fileNameMap.find(m_selectedActor);
		if (iter != m_fileNameMap.end())
		{
			qDebug() << "QVTKFramebufferObjectRenderer::selectModel() picked model path " << iter->second;
			if (m_modelPath != iter->second.toString())
			{
				m_modelPath = iter->second.toString();
				if (m_modelPath.indexOf(";") == -1)
				{
					m_modelPath = m_modelPath.left(1).toUpper().append(m_modelPath.mid(1, -1));
				}
				else
				{
					QList<QString> tmp_list = m_modelPath.split(";");
					tmp_list.removeLast();
					m_modelPath = "";
					for (int i = 0; i < tmp_list.length(); i++)
					{
						tmp_list[i].remove(QRegExp("^ +\\s*"));
						m_modelPath += tmp_list[i].left(1).toUpper().append(tmp_list[i].mid(1, -1));
						m_modelPath += "; ";
					}
					m_modelPath.remove(QRegExp("\\s* +$"));
				}
				emit isModelFileNameChanged(m_modelPath);
			}
		}
		
		m_selectedModel->setSelected(true);

		m_selectedModel->getModelData();

		// Connect signals
		connect(m_selectedModel.get(), &Model::positionXChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelPositionX);
		connect(m_selectedModel.get(), &Model::positionYChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelPositionY);
		connect(m_selectedModel.get(), &Model::modelRepresentationChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelRepresentation);
		connect(m_selectedModel.get(), &Model::modelDecreasePolygonsChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelDecreasePolygons);
		connect(m_selectedModel.get(), &Model::modelIncreasePolygonsChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelIncreasePolygons);
		connect(m_selectedModel.get(), &Model::modelOpacityChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelOpacity);
		connect(m_selectedModel.get(), &Model::modelSmoothChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelSmooth);
		connect(m_selectedModel.get(), &Model::modelGIChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelGI);
		connect(m_selectedModel.get(), &Model::modelColorRChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelColorR);
		connect(m_selectedModel.get(), &Model::modelColorGChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelColorG);
		connect(m_selectedModel.get(), &Model::modelColorBChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelColorB);
		connect(m_selectedModel.get(), &Model::modelPolygonsChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelPolygons);
		connect(m_selectedModel.get(), &Model::modelPointsChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelPoints);

		this->setSelectedModelPositionX(m_selectedModel->getPositionX());
		this->setSelectedModelPositionY(m_selectedModel->getPositionY());
		this->setSelectedModelRepresentation(m_selectedModel->getModelRepresentation());
		this->setSelectedModelDecreasePolygons(m_selectedModel->getModelTargetReduction());
		this->setSelectedModelIncreasePolygons(m_selectedModel->getModelIncreasePolygonsIterations());
		this->setSelectedModelOpacity(m_selectedModel->getModelOpacity());
		this->setSelectedModelSmooth(m_selectedModel->getModelSmooth());
		this->setSelectedModelGI(m_selectedModel->getModelGI());
		this->setSelectedModelColorR(m_selectedModel->getModelColorR());
		this->setSelectedModelColorG(m_selectedModel->getModelColorG());
		this->setSelectedModelColorB(m_selectedModel->getModelColorB());
		this->setSelectedModelPolygons(m_selectedModel->getModelPolygons());
		this->setSelectedModelPoints(m_selectedModel->getModelPoints());
		this->setSelectedModelPath(m_modelPath);
		this->setSelectedModelVolume(m_selectedModel->getModelVolume());
		this->setSelectedModelSurfaceArea(m_selectedModel->getModelSurfaceArea());
		this->setSelectedModelMaxAreaOfCell(m_selectedModel->getModelMaxAreaOfCell());
		this->setSelectedModelMinAreaOfCell(m_selectedModel->getModelMinAreaOfCell());

		m_renderer->AddActor(m_selectedModel->getModelOutlineActor());

		this->setIsModelSelected(true);

		// Set mouse click delta from center position
		m_selectedModel->setMouseDeltaXY(clickPosition[0] - m_selectedModel->getPositionX(), clickPosition[1] - m_selectedModel->getPositionY());
	}
	else
	{
		this->setIsModelSelected(false);
	}

	qDebug() << "QVTKFramebufferObjectRenderer::selectModel() end";
}

void QVTKFramebufferObjectRenderer::clearSelectedModel(int param)
{
	m_renderer->RemoveActor(m_selectedModel->getModelOutlineActor());

	m_selectedModel->setSelected(false);

	disconnect(m_selectedModel.get(), &Model::positionXChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelPositionX);
	disconnect(m_selectedModel.get(), &Model::positionYChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelPositionY);	
	disconnect(m_selectedModel.get(), &Model::modelRepresentationChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelRepresentation);
	disconnect(m_selectedModel.get(), &Model::modelDecreasePolygonsChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelDecreasePolygons);
	disconnect(m_selectedModel.get(), &Model::modelIncreasePolygonsChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelIncreasePolygons);
	disconnect(m_selectedModel.get(), &Model::modelOpacityChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelOpacity);
	disconnect(m_selectedModel.get(), &Model::modelSmoothChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelSmooth);
	disconnect(m_selectedModel.get(), &Model::modelGIChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelGI);
	disconnect(m_selectedModel.get(), &Model::modelColorRChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelColorR);
	disconnect(m_selectedModel.get(), &Model::modelColorGChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelColorG);
	disconnect(m_selectedModel.get(), &Model::modelColorBChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelColorB);
	disconnect(m_selectedModel.get(), &Model::modelPolygonsChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelPolygons);
	disconnect(m_selectedModel.get(), &Model::modelPointsChanged, this, &QVTKFramebufferObjectRenderer::setSelectedModelPoints);

	if (!param)
	{
		m_selectedModel = nullptr;
		m_selectedActor = nullptr;
	}
}

void QVTKFramebufferObjectRenderer::showPlatform(const bool checked)
{
	if (checked)
	{
		m_renderer->AddActor(m_platformModelActor);
		m_renderer->AddActor(m_platformGridActor);
		this->updatePlatform();
	}
	else
	{
		m_renderer->RemoveActor(m_platformModelActor);
		m_renderer->RemoveActor(m_platformGridActor);
	}
}

void QVTKFramebufferObjectRenderer::showAxes(const bool checked)
{
	if (checked)
	{
		m_renderer->AddActor(m_axes);
	}
	else
	{
		m_renderer->RemoveActor(m_axes);
	}
}

void QVTKFramebufferObjectRenderer::closeModel()
{
	if (m_selectedModel != nullptr && m_selectedActor != nullptr)
	{
		m_renderer->RemoveActor(m_selectedModel->getModelOutlineActor());
		size_t delete_s = m_fileNameMap.erase(m_selectedActor);
		qDebug() << "QVTKFramebufferObjectRenderer::closeModel() delete map code: " << delete_s;
		this->setIsModelSelected(false);
		this->clearSelectedModel(1);
		m_renderer->RemoveActor(m_selectedActor);
		m_selectedModel = nullptr;
		m_selectedActor = nullptr;
		m_modelCount -= 1;
		emit isModelCountChanged(m_modelCount);
	}
}

void QVTKFramebufferObjectRenderer::closeAllModel(std::vector<std::shared_ptr<Model>> models)
{
	this->closeModel();
	for (const std::shared_ptr<Model> &model : models)
	{
		m_renderer->RemoveActor(model->getModelActor());
		size_t delete_s = m_fileNameMap.erase(model->getModelActor());
		qDebug() << "QVTKFramebufferObjectRenderer::closeAllModel() delete map code: " << delete_s;
		m_modelCount -= 1;
		emit isModelCountChanged(m_modelCount);
	}
}

void QVTKFramebufferObjectRenderer::setIsModelSelected(const bool isModelSelected)
{
	if (m_isModelSelected != isModelSelected)
	{
		qDebug() << "QVTKFramebufferObjectRenderer::setIsModelSelected():" << isModelSelected;
		m_isModelSelected = isModelSelected;
		emit isModelSelectedChanged();
	}
}

bool QVTKFramebufferObjectRenderer::isModelSelected() const
{
	return m_isModelSelected;
}

const vtkSmartPointer<vtkRenderer>& QVTKFramebufferObjectRenderer::getRenderer() const
{
	return m_renderer;
}

std::shared_ptr<Model> QVTKFramebufferObjectRenderer::getSelectedModel() const
{
	std::shared_ptr<Model> selectedModel = this->getSelectedModelNoLock();

	return selectedModel;
}

std::shared_ptr<Model> QVTKFramebufferObjectRenderer::getSelectedModelNoLock() const
{
	return m_processingEngine->getModelFromActor(m_selectedActor);
}

void QVTKFramebufferObjectRenderer::setSelectedModelPositionX(const double positionX)
{
	if (m_selectedModelPositionX != positionX)
	{
		m_selectedModelPositionX = positionX;
		emit selectedModelPositionXChanged();
	}
}

void QVTKFramebufferObjectRenderer::setSelectedModelPositionY(const double positionY)
{
	if (m_selectedModelPositionY != positionY)
	{
		m_selectedModelPositionY = positionY;
		emit selectedModelPositionYChanged();
	}
}

double QVTKFramebufferObjectRenderer::getSelectedModelPositionX() const
{
	return m_selectedModelPositionX;
}

double QVTKFramebufferObjectRenderer::getSelectedModelPositionY() const
{
	return m_selectedModelPositionY;
}

int QVTKFramebufferObjectRenderer::getSelecteModelRepresentation() const
{
	return m_modelRepresentation;
}

double QVTKFramebufferObjectRenderer::getSelecteModelDecreasePolygons() const
{
	return m_modelDecreasePolygons;
}

int QVTKFramebufferObjectRenderer::getSelecteModelIncreasePolygons() const
{
	return m_modelIncreasePolygons;
}

double QVTKFramebufferObjectRenderer::getSelecteModelOpacity() const
{
	return m_modelOpacity;
}

int QVTKFramebufferObjectRenderer::getSelecteModelSmooth() const
{
	return m_modelSmooth;
}

bool QVTKFramebufferObjectRenderer::getSelecteModelGI() const
{
	return m_modelGI;
}

int QVTKFramebufferObjectRenderer::getSelectedModelColorR() const
{
	return m_modelColorR;
}

int QVTKFramebufferObjectRenderer::getSelectedModelColorG() const
{
	return m_modelColorG;
}

int QVTKFramebufferObjectRenderer::getSelectedModelColorB() const
{
	return m_modelColorB;
}

int QVTKFramebufferObjectRenderer::getSelecteModelPolygons() const
{
	return m_modelPolygons;
}

int QVTKFramebufferObjectRenderer::getSelecteModelPoints() const
{
	return m_modelPoints;
}

QString QVTKFramebufferObjectRenderer::getSelecteModelFileName() const
{
	return m_modelPath;
}

double QVTKFramebufferObjectRenderer::getSelecteModelVolume() const
{
	return m_modelVolume;
}

double QVTKFramebufferObjectRenderer::getSelecteModelSurfaceArea() const
{
	return m_modelSurfaceArea;
}

double QVTKFramebufferObjectRenderer::getSelecteModelMaxAreaOfCell() const
{
	return m_modelMaxAreaOfCell;
}

double QVTKFramebufferObjectRenderer::getSelecteModelMinAreaOfCell() const
{
	return m_modelMinAreaOfCell;
}

int QVTKFramebufferObjectRenderer::getModelCount() const
{
	return m_modelCount;
}

void QVTKFramebufferObjectRenderer::setSelectedModelRepresentation(const int option)
{
	if (m_modelRepresentation != option)
	{
		m_modelRepresentation = option;
		emit isModelRepresentationChanged();
	}
}

void QVTKFramebufferObjectRenderer::setSelectedModelDecreasePolygons(const double polygons)
{
	if (m_modelDecreasePolygons != polygons)
	{
		m_modelDecreasePolygons = polygons;
		emit isModelDecreasePolygonsChanged();
	}
}

void QVTKFramebufferObjectRenderer::setSelectedModelIncreasePolygons(const int iterations)
{
	if (m_modelIncreasePolygons != iterations)
	{
		m_modelIncreasePolygons = iterations;
		emit isModelIncreasePolygonsChanged();
	}
}

void QVTKFramebufferObjectRenderer::setSelectedModelOpacity(const double opacity)
{
	if (m_modelOpacity != opacity)
	{
		m_modelOpacity = opacity;
		emit isModelOpacityChanged();
	}
}

void QVTKFramebufferObjectRenderer::setSelectedModelSmooth(const int smooth)
{
	if (m_modelSmooth != smooth)
	{
		m_modelSmooth = smooth;
		emit isModelSmoothChanged();
	}
}

void QVTKFramebufferObjectRenderer::setSelectedModelGI(const bool checked)
{
	if (m_modelGI != checked)
	{
		m_modelGI = checked;
		emit isModelGIChanged();
	}
}

void QVTKFramebufferObjectRenderer::setSelectedModelColorR(const int colorR)
{
	if (m_modelColorR != colorR)
	{
		m_modelColorR = colorR;
		emit isModelColorRChanged();
	}
}

void QVTKFramebufferObjectRenderer::setSelectedModelColorG(const int colorG)
{
	if (m_modelColorG != colorG)
	{
		m_modelColorG = colorG;
		emit isModelColorGChanged();
	}
}

void QVTKFramebufferObjectRenderer::setSelectedModelColorB(const int colorB)
{
	if (m_modelColorB != colorB)
	{
		m_modelColorB = colorB;
		emit isModelColorBChanged();
	}
}

void QVTKFramebufferObjectRenderer::setSelectedModelPolygons(const int polygons)
{
	if (m_modelPolygons != polygons)
	{
		m_modelPolygons = polygons;
		emit isModelPolygonsChanged();
	}
}

void QVTKFramebufferObjectRenderer::setSelectedModelPoints(const int points)
{
	if (m_modelPoints != points)
	{
		m_modelPoints = points;
		emit isModelPointsChanged();
	}
}

void QVTKFramebufferObjectRenderer::setSelectedModelPath(const QString path)
{
	if (m_modelPath != path)
	{
		m_modelPath = path;
		emit isModelFileNameChanged(m_modelPath);
	}
}

void QVTKFramebufferObjectRenderer::setSelectedModelVolume(const double volume)
{
	if (m_modelVolume != volume)
	{
		m_modelVolume = volume;
		emit isModelVolumeChanged(m_modelVolume);
	}
}

void QVTKFramebufferObjectRenderer::setSelectedModelSurfaceArea(const double surfaceArea)
{
	if (m_modelSurfaceArea != surfaceArea)
	{
		m_modelSurfaceArea = surfaceArea;
		emit isModelSurfaceAreaChanged(m_modelSurfaceArea);
	}
}

void QVTKFramebufferObjectRenderer::setSelectedModelMaxAreaOfCell(const double maxAreaOfCell)
{
	if (m_modelMaxAreaOfCell != maxAreaOfCell)
	{
		m_modelMaxAreaOfCell = maxAreaOfCell;
		emit isModelMaxAreaOfCellChanged(m_modelMaxAreaOfCell);
	}
}

void QVTKFramebufferObjectRenderer::setSelectedModelMinAreaOfCell(const double minAreaOfCell)
{
	if (m_modelMinAreaOfCell != minAreaOfCell)
	{
		m_modelMinAreaOfCell = minAreaOfCell;
		emit isModelMinAreaOfCellChanged(m_modelMinAreaOfCell);
	}
}

const bool QVTKFramebufferObjectRenderer::screenToWorld(const int16_t screenX, const int16_t screenY, double worldPos[])
{
	//Create bounding planes for projection plane
	vtkSmartPointer<vtkPlane> boundingPlanes[4];

	boundingPlanes[0] = vtkSmartPointer<vtkPlane>::New();
	boundingPlanes[0]->SetOrigin(0.0, 1000.0, 0.0);
	boundingPlanes[0]->SetNormal(0.0, -1.0, 0.0);

	boundingPlanes[1] = vtkSmartPointer<vtkPlane>::New();
	boundingPlanes[1]->SetOrigin(0.0, -1000.0, 0.0);
	boundingPlanes[1]->SetNormal(0.0, 1.0, 0.0);

	boundingPlanes[2] = vtkSmartPointer<vtkPlane>::New();
	boundingPlanes[2]->SetOrigin(1000.0, 0.0, 0.0);
	boundingPlanes[2]->SetNormal(-1.0, 0.0, 0.0);

	boundingPlanes[3] = vtkSmartPointer<vtkPlane>::New();
	boundingPlanes[3]->SetOrigin(-1000.0, 0.0, 0.0);
	boundingPlanes[3]->SetNormal(1.0, 0.0, 0.0);

	// Create projection plane parallel platform and Z coordinate from clicked position in model
	vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
	plane->SetOrigin(0.0, 0.0, m_clickPositionZ);
	plane->SetNormal(0.0, 0.0, 1.0);

	// Set projection and bounding planes to placer
	vtkSmartPointer<vtkBoundedPlanePointPlacer> placer = vtkSmartPointer<vtkBoundedPlanePointPlacer>::New();
	placer->SetObliquePlane(plane);
	placer->SetProjectionNormalToOblique();

	placer->AddBoundingPlane(boundingPlanes[0].Get());
	placer->AddBoundingPlane(boundingPlanes[1].Get());
	placer->AddBoundingPlane(boundingPlanes[2].Get());
	placer->AddBoundingPlane(boundingPlanes[3].Get());

	double screenPos[2];
	double worldOrient[9];

	screenPos[0] = screenX;
	// Compensate the y-axis flip for the picking
	screenPos[1] = m_renderer->GetSize()[1] - screenY;

	int16_t withinBounds;
	withinBounds = placer->ComputeWorldPosition(m_renderer, screenPos, worldPos, worldOrient);

	return withinBounds;
}

void QVTKFramebufferObjectRenderer::setModelFilePathToMap(QUrl path)
{
	if (m_tmpAddActor != nullptr)
	{
		auto iter = m_fileNameMap.find(m_tmpAddActor);
		if (iter == m_fileNameMap.end())
		{
			m_fileNameMap[m_tmpAddActor] = path;
		}
	}
}

void QVTKFramebufferObjectRenderer::resetCamera()
{
	// Seting the clipping range here messes with the opacity of the actors prior to moving the camera
	m_camPositionX = -237.885;
	m_camPositionY = -392.348;
	m_camPositionZ = 369.477;
	m_renderer->GetActiveCamera()->SetPosition(m_camPositionX, m_camPositionY, m_camPositionZ);
	m_renderer->GetActiveCamera()->SetFocalPoint(0.0, 0.0, 0.0);
	m_renderer->GetActiveCamera()->SetViewUp(0.0, 0.0, 1.0);
	m_renderer->ResetCameraClippingRange();
}
