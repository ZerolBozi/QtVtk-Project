#include "CommandModel.h"
#include "CommandModelAdd.h"
#include "Model.h"
#include "ProcessingEngine.h"
#include "QVTKFramebufferObjectItem.h"
#include "QVTKFramebufferObjectRenderer.h"


QVTKFramebufferObjectItem::QVTKFramebufferObjectItem()
{
	// Class 入口函數

	// 滑鼠事件指標
	m_lastMouseLeftButton = std::make_shared<QMouseEvent>(QEvent::None, QPointF(0,0), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
	m_lastMouseButton = std::make_shared<QMouseEvent>(QEvent::None, QPointF(0,0), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
	m_lastMouseMove = std::make_shared<QMouseEvent>(QEvent::None, QPointF(0,0), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
	m_lastMouseWheel = std::make_shared<QWheelEvent>(QPointF(0,0), 0, Qt::NoButton, Qt::NoModifier, Qt::Vertical);

	this->setMirrorVertically(true); // QtQuick and OpenGL have opposite Y-Axis directions

	setAcceptedMouseButtons(Qt::RightButton);
}


QQuickFramebufferObject::Renderer *QVTKFramebufferObjectItem::createRenderer() const
{
	// 返回智能指標
	return new QVTKFramebufferObjectRenderer();
}

void QVTKFramebufferObjectItem::setVtkFboRenderer(QVTKFramebufferObjectRenderer* renderer)
{
	// 參數:窗口指標
	qDebug() << "QVTKFramebufferObjectItem::setVtkFboRenderer";

	m_vtkFboRenderer = renderer;

	// 建立信號連線
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelSelectedChanged, this, &QVTKFramebufferObjectItem::isModelSelectedChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::selectedModelPositionXChanged, this, &QVTKFramebufferObjectItem::selectedModelPositionXChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::selectedModelPositionYChanged, this, &QVTKFramebufferObjectItem::selectedModelPositionYChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelRepresentationChanged, this, &QVTKFramebufferObjectItem::isModelRepresentationChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelDecreasePolygonsChanged, this, &QVTKFramebufferObjectItem::isModelDecreasePolygonsChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelIncreasePolygonsChanged, this, &QVTKFramebufferObjectItem::isModelIncreasePolygonsChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelOpacityChanged, this, &QVTKFramebufferObjectItem::isModelOpacityChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelSmoothChanged, this, &QVTKFramebufferObjectItem::isModelSmoothChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelGIChanged, this, &QVTKFramebufferObjectItem::isModelGIChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelColorRChanged, this, &QVTKFramebufferObjectItem::isModelColorRChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelColorGChanged, this, &QVTKFramebufferObjectItem::isModelColorGChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelColorBChanged, this, &QVTKFramebufferObjectItem::isModelColorBChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelPolygonsChanged, this, &QVTKFramebufferObjectItem::isModelPolygonsChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelPointsChanged, this, &QVTKFramebufferObjectItem::isModelPointsChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelFileNameChanged, this, &QVTKFramebufferObjectItem::isModelFileNameChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelVolumeChanged, this, &QVTKFramebufferObjectItem::isModelVolumeChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelSurfaceAreaChanged, this, &QVTKFramebufferObjectItem::isModelSurfaceAreaChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelMaxAreaOfCellChanged, this, &QVTKFramebufferObjectItem::isModelMaxAreaOfCellChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelMinAreaOfCellChanged, this, &QVTKFramebufferObjectItem::isModelMinAreaOfCellChanged);
	connect(m_vtkFboRenderer, &QVTKFramebufferObjectRenderer::isModelCountChanged, this, &QVTKFramebufferObjectItem::isModelCountChanged);

	// 設定處理引擎
	m_vtkFboRenderer->setProcessingEngine(m_processingEngine);
}

bool QVTKFramebufferObjectItem::isInitialized() const
{
	// 初始化畫面，返回初始化是否成功
	return (m_vtkFboRenderer != nullptr);
}

void QVTKFramebufferObjectItem::setProcessingEngine(const std::shared_ptr<ProcessingEngine> processingEngine)
{
	// ProcessingEngine指標
	m_processingEngine = std::shared_ptr<ProcessingEngine>(processingEngine);
}


// Model releated functions

bool QVTKFramebufferObjectItem::isModelSelected() const
{
	if (this == nullptr)
	{
		return false;
	}
	return m_vtkFboRenderer->isModelSelected();
}

// begin--------from render get the value then ret to handler--------

double QVTKFramebufferObjectItem::getSelectedModelPositionX() const
{
	return m_vtkFboRenderer->getSelectedModelPositionX();
}

double QVTKFramebufferObjectItem::getSelectedModelPositionY() const
{
	return m_vtkFboRenderer->getSelectedModelPositionY();
}

int QVTKFramebufferObjectItem::getSelecteModelRepresentation() const
{
	return m_vtkFboRenderer->getSelecteModelRepresentation();
}

double QVTKFramebufferObjectItem::getSelecteModelDecreasePolygons() const
{
	return m_vtkFboRenderer->getSelecteModelDecreasePolygons();
}

int QVTKFramebufferObjectItem::getSelecteModelIncreasePolygons() const
{
	return m_vtkFboRenderer->getSelecteModelIncreasePolygons();
}

double QVTKFramebufferObjectItem::getSelecteModelOpacity() const
{
	return m_vtkFboRenderer->getSelecteModelOpacity();
}

int QVTKFramebufferObjectItem::getSelecteModelSmooth() const
{
	return m_vtkFboRenderer->getSelecteModelSmooth();
}

bool QVTKFramebufferObjectItem::getSelecteModelGI() const
{
	return m_vtkFboRenderer->getSelecteModelGI();
}

int QVTKFramebufferObjectItem::getSelectedModelColorR() const
{
	return m_vtkFboRenderer->getSelectedModelColorR();
}

int QVTKFramebufferObjectItem::getSelectedModelColorG() const
{
	return m_vtkFboRenderer->getSelectedModelColorG();
}

int QVTKFramebufferObjectItem::getSelectedModelColorB() const
{
	return m_vtkFboRenderer->getSelectedModelColorB();
}

int QVTKFramebufferObjectItem::getSelecteModelPolygons() const
{
	return m_vtkFboRenderer->getSelecteModelPolygons();
}

int QVTKFramebufferObjectItem::getSelecteModelPoints() const
{
	return m_vtkFboRenderer->getSelecteModelPoints();
}

QString QVTKFramebufferObjectItem::getSelecteModelFileName() const
{
	return m_vtkFboRenderer->getSelecteModelFileName();
}

double QVTKFramebufferObjectItem::getSelecteModelVolume() const
{
	return m_vtkFboRenderer->getSelecteModelVolume();
}

double QVTKFramebufferObjectItem::getSelecteModelSurfaceArea() const
{
	return m_vtkFboRenderer->getSelecteModelSurfaceArea();
}

double QVTKFramebufferObjectItem::getSelecteModelMaxAreaOfCell() const
{
	return m_vtkFboRenderer->getSelecteModelMaxAreaOfCell();
}

double QVTKFramebufferObjectItem::getSelecteModelMinAreaOfCell() const
{
	return m_vtkFboRenderer->getSelecteModelMinAreaOfCell();
}

int QVTKFramebufferObjectItem::getModelCount() const
{
	if (this == nullptr)
	{
		return 0;
	}
	return m_vtkFboRenderer->getModelCount();
}

// end.

void QVTKFramebufferObjectItem::showPlatform(const bool checked)
{
	// to renderer set the switch
	m_vtkFboRenderer->showPlatform(checked);
	// reflash the render
	update();
}

void QVTKFramebufferObjectItem::showAxes(const bool checked)
{
	// to renderer set the switch
	m_vtkFboRenderer->showAxes(checked);
	// reflash the render
	update();
}

void QVTKFramebufferObjectItem::createCube(const double x, const double y, const double z)
{
	std::shared_ptr<Model> model = nullptr;
	model = m_processingEngine->createCube(x,y,z);
	m_processingEngine->placeModel(*model);
	m_vtkFboRenderer->addModelActor(model);
	m_vtkFboRenderer->setModelFilePathToMap(QUrl("Cube.tmp"));
	// reflash the render
	update();
}

void QVTKFramebufferObjectItem::createSphare(const double radius)
{
	std::shared_ptr<Model> model = nullptr;
	model = m_processingEngine->createSphare(radius);
	m_processingEngine->placeModel(*model);
	m_vtkFboRenderer->addModelActor(model);
	m_vtkFboRenderer->setModelFilePathToMap(QUrl("Sphare.tmp"));
	// reflash the render
	update();
}

void QVTKFramebufferObjectItem::selectModel(const int screenX, const int screenY)
{
	m_lastMouseLeftButton = std::make_shared<QMouseEvent>(QEvent::None, QPointF(screenX, screenY), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	m_lastMouseLeftButton->ignore();
	// reflash the render
	update();
}

void QVTKFramebufferObjectItem::resetModelSelection()
{
	m_lastMouseLeftButton = std::make_shared<QMouseEvent>(QEvent::None, QPointF(-1, -1), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
	m_lastMouseLeftButton->ignore();
	// reflash the render
	update();
}

void QVTKFramebufferObjectItem::addModelFromFile(const QUrl &modelPath)
{
	qDebug() << "QVTKFramebufferObjectItem::addModelFromFile";

	QList<QUrl> modelPaths;
	modelPaths.append(modelPath);

	// 單個模型導入函數
	CommandModelAdd *command = new CommandModelAdd(m_vtkFboRenderer, m_processingEngine, modelPaths);

	// 建立連線
	connect(command, &CommandModelAdd::ready, this, &QVTKFramebufferObjectItem::update);
	connect(command, &CommandModelAdd::done, this, &QVTKFramebufferObjectItem::addModelFromFileDone);

	command->start();

	// 新增導入模型任務
	this->addCommand(command);
}

void QVTKFramebufferObjectItem::addModelFromFiles(const QList<QUrl> &modelPaths)
{
	qDebug() << "QVTKFramebufferObjectItem::addModelFromFiles";

	// 單個模型導入並合併函數
	CommandModelAdd *command = new CommandModelAdd(m_vtkFboRenderer, m_processingEngine, modelPaths);

	// 建立連線
	connect(command, &CommandModelAdd::ready, this, &QVTKFramebufferObjectItem::update);
	connect(command, &CommandModelAdd::done, this, &QVTKFramebufferObjectItem::addModelFromFileDone);

	command->start();

	// 新增導入模型並合併的任務
	this->addCommand(command);
}

void QVTKFramebufferObjectItem::saveModel(const QUrl &modelPath)
{
	std::shared_ptr<Model> m_model = m_vtkFboRenderer->getSelectedModel();
	// 新增保存檔案任務
	this->addCommand(new CommandModelSave(m_vtkFboRenderer, m_model, modelPath));
}

void QVTKFramebufferObjectItem::closeModel()
{
	m_vtkFboRenderer->closeModel();
	// reflash the render
	update();
}

void QVTKFramebufferObjectItem::closeAllModel()
{
	m_vtkFboRenderer->closeAllModel(m_processingEngine->getAllModels());
	// reflash the render
	update();
}

void QVTKFramebufferObjectItem::translateModel(CommandModelTranslate::TranslateParams_t & translateData, const bool inTransition)
{
	if (translateData.model == nullptr)
	{
		// If no model selected yet, try to select one
		translateData.model = m_vtkFboRenderer->getSelectedModel();

		if (translateData.model == nullptr)
		{
			return;
		}
	}
	// 新增模型移動任務
	this->addCommand(new CommandModelTranslate(m_vtkFboRenderer, translateData, inTransition));
}

void QVTKFramebufferObjectItem::actorModel(CommandModelActor::ActorParams_t & actorData)
{
	// 檢查模型是否被選擇
	if (actorData.model == nullptr)
	{
		// If no model selected yet, try to select one
		actorData.model = m_vtkFboRenderer->getSelectedModel();

		if (actorData.model == nullptr)
		{
			return;
		}
	}
	// 新增任務 類型=Actor
	this->addCommand(new CommandModelActor(m_vtkFboRenderer, actorData));
}

void QVTKFramebufferObjectItem::addCommand(CommandModel *command)
{
	// 互斥鎖 鎖上後不可被更改，直到解鎖
	// 將要執行的任務壓入堆疊
	m_commandsQueueMutex.lock();
	m_commandsQueue.push(command);
	m_commandsQueueMutex.unlock();
	// 刷新渲染器同時，渲染器的函數會去調用並執行任務
	update();
}


// Camera related functions

// C++內滑鼠事件重載，是設定右鍵的滑鼠事件，左鍵事件寫在qml內

void QVTKFramebufferObjectItem::wheelEvent(QWheelEvent *e)
{
	// 滾輪事件 參數 (滾輪事件指標)
	m_lastMouseWheel = std::make_shared<QWheelEvent>(*e);
	m_lastMouseWheel->ignore();
	e->accept();
	update();
}

void QVTKFramebufferObjectItem::mousePressEvent(QMouseEvent *e)
{
	// 滑鼠按下事件 (滑鼠事件指標)
	if (e->buttons() & Qt::RightButton)
	{
		m_lastMouseButton = std::make_shared<QMouseEvent>(*e);
		m_lastMouseButton->ignore();
		e->accept();
		update();
	}
}

void QVTKFramebufferObjectItem::mouseReleaseEvent(QMouseEvent *e)
{
	// 滑鼠鬆開事件 (滑鼠事件指標)
	m_lastMouseButton = std::make_shared<QMouseEvent>(*e);
	m_lastMouseButton->ignore();
	e->accept();
	update();
}

void QVTKFramebufferObjectItem::mouseMoveEvent(QMouseEvent *e)
{
	// 滑鼠移動事件 (滑鼠事件指標)
	if (e->buttons() & Qt::RightButton)
	{
		*m_lastMouseMove = *e;
		m_lastMouseMove->ignore();
		e->accept();
		update();
	}
}

QMouseEvent *QVTKFramebufferObjectItem::getLastMouseLeftButton()
{
	return m_lastMouseLeftButton.get();
}

QMouseEvent *QVTKFramebufferObjectItem::getLastMouseButton()
{
	return m_lastMouseButton.get();
}

QMouseEvent *QVTKFramebufferObjectItem::getLastMoveEvent()
{
	return m_lastMouseMove.get();
}

QWheelEvent *QVTKFramebufferObjectItem::getLastWheelEvent()
{
	return m_lastMouseWheel.get();
}

void QVTKFramebufferObjectItem::resetCamera()
{
	m_vtkFboRenderer->resetCamera();
	update();
}

// 返回模型設定值的相關函數

int QVTKFramebufferObjectItem::getModelsRepresentation() const
{
	return m_modelsRepresentationOption;
}

double QVTKFramebufferObjectItem::getModelsOpacity() const
{
	return m_modelsOpacity;
}

bool QVTKFramebufferObjectItem::getGourauInterpolation() const
{
	return m_gouraudInterpolation;
}

int QVTKFramebufferObjectItem::getModelColorR() const
{
	return m_modelColorR;
}

int QVTKFramebufferObjectItem::getModelColorG() const
{
	return m_modelColorG;
}

int QVTKFramebufferObjectItem::getModelColorB() const
{
	return m_modelColorB;
}

void QVTKFramebufferObjectItem::setModelsRepresentation(const int representationOption)
{
	if (m_modelsRepresentationOption != representationOption)
	{
		m_modelsRepresentationOption = representationOption;
		update();
	}
}

void QVTKFramebufferObjectItem::setModelsOpacity(const double opacity)
{
	if (m_modelsOpacity != opacity)
	{
		m_modelsOpacity = opacity;
		update();
	}
}

void QVTKFramebufferObjectItem::setGouraudInterpolation(const bool gouraudInterpolation)
{
	if (m_gouraudInterpolation != gouraudInterpolation)
	{
		m_gouraudInterpolation = gouraudInterpolation;
		update();
	}
}

void QVTKFramebufferObjectItem::setModelColorR(const int colorR)
{
	if (m_modelColorR != colorR)
	{
		m_modelColorR = colorR;
		update();
	}
}

void QVTKFramebufferObjectItem::setModelColorG(const int colorG)
{
	if (m_modelColorG != colorG)
	{
		m_modelColorG = colorG;
		update();
	}
}

void QVTKFramebufferObjectItem::setModelColorB(const int colorB)
{
	if (m_modelColorB != colorB)
	{
		m_modelColorB = colorB;
		update();
	}
}

CommandModel *QVTKFramebufferObjectItem::getCommandsQueueFront() const
{
	return m_commandsQueue.front();
}

void QVTKFramebufferObjectItem::commandsQueuePop()
{
	m_commandsQueue.pop();
}

bool QVTKFramebufferObjectItem::isCommandsQueueEmpty() const
{
	return m_commandsQueue.empty();
}

void QVTKFramebufferObjectItem::lockCommandsQueueMutex()
{
	m_commandsQueueMutex.lock();
}

void QVTKFramebufferObjectItem::unlockCommandsQueueMutex()
{
	m_commandsQueueMutex.unlock();
}

