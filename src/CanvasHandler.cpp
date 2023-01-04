#include <QApplication>
#include <QDebug>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "Model.h"
#include "ProcessingEngine.h"
#include "QVTKFramebufferObjectItem.h"
#include "QVTKFramebufferObjectRenderer.h"
#include "CanvasHandler.h"


CanvasHandler::CanvasHandler(int argc, char **argv)
{
	QApplication app(argc, argv);
	QQmlApplicationEngine engine;

	app.setApplicationName("QtVTK");
	app.setWindowIcon(QIcon(":/resources/bq.ico"));

	// Register QML types
	qmlRegisterType<QVTKFramebufferObjectItem>("QtVTK", 1, 0, "VtkFboItem");

	// Create classes instances
	m_processingEngine = std::shared_ptr<ProcessingEngine>(new ProcessingEngine());

	// Expose C++ classes to QML
	QQmlContext* ctxt = engine.rootContext();

	ctxt->setContextProperty("canvasHandler", this);

	QQuickStyle::setStyle("Material");

	// Load main QML file
	engine.load(QUrl("qrc:/resources/main.qml"));

	// Get reference to the QVTKFramebufferObjectItem created in QML
	// We cannot use smart pointers because this object must be deleted by QML
	QObject *rootObject = engine.rootObjects().first();
	m_vtkFboItem = rootObject->findChild<QVTKFramebufferObjectItem*>("vtkFboItem");

	// Give the vtkFboItem reference to the CanvasHandler
	if (m_vtkFboItem)
	{
		qDebug() << "CanvasHandler::CanvasHandler: setting vtkFboItem to CanvasHandler";

		m_vtkFboItem->setProcessingEngine(m_processingEngine);

		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::rendererInitialized, this, &CanvasHandler::startApplication);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelSelectedChanged, this, &CanvasHandler::isModelSelectedChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::selectedModelPositionXChanged, this, &CanvasHandler::selectedModelPositionXChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::selectedModelPositionYChanged, this, &CanvasHandler::selectedModelPositionYChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelRepresentationChanged, this, &CanvasHandler::isModelRepresentationChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelDecreasePolygonsChanged, this, &CanvasHandler::isModelDecreasePolygonsChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelIncreasePolygonsChanged, this, &CanvasHandler::isModelIncreasePolygonsChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelOpacityChanged, this, &CanvasHandler::isModelOpacityChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelSmoothChanged, this, &CanvasHandler::isModelSmoothChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelGIChanged, this, &CanvasHandler::isModelGIChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelColorRChanged, this, &CanvasHandler::isModelColorRChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelColorGChanged, this, &CanvasHandler::isModelColorGChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelColorBChanged, this, &CanvasHandler::isModelColorBChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelPolygonsChanged, this, &CanvasHandler::isModelPolygonsChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelPointsChanged, this, &CanvasHandler::isModelPointsChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelFileNameChanged, this, &CanvasHandler::isModelFileNameChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelVolumeChanged, this, &CanvasHandler::isModelVolumeChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelSurfaceAreaChanged, this, &CanvasHandler::isModelSurfaceAreaChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelMaxAreaOfCellChanged, this, &CanvasHandler::isModelMaxAreaOfCellChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelMinAreaOfCellChanged, this, &CanvasHandler::isModelMinAreaOfCellChanged);
		connect(m_vtkFboItem, &QVTKFramebufferObjectItem::isModelCountChanged, this, &CanvasHandler::isModelCountChanged);
	}
	else
	{
		qCritical() << "CanvasHandler::CanvasHandler: Unable to get vtkFboItem instance";
		return;
	}

	int rc = app.exec();

	qDebug() << "CanvasHandler::CanvasHandler: Execution finished with return code:" << rc;
}


void CanvasHandler::startApplication() const
{
	qDebug() << "CanvasHandler::startApplication()";

	disconnect(m_vtkFboItem, &QVTKFramebufferObjectItem::rendererInitialized, this, &CanvasHandler::startApplication);
}

void CanvasHandler::showPlatform(const bool checked) const
{
	m_vtkFboItem->showPlatform(checked);
}

void CanvasHandler::showAxes(const bool checked) const
{
	m_vtkFboItem->showAxes(checked);
}

void CanvasHandler::createCube(const QString x, const QString y, const QString z) const
{
	m_vtkFboItem->createCube(x.toDouble(), y.toDouble(), z.toDouble());
}

void CanvasHandler::createSphare(const QString radius) const
{
	m_vtkFboItem->createSphare(radius.toDouble());
}

void CanvasHandler::openModel(const QList<QUrl> &paths) const
{
	qDebug() << "CanvasHandler::openModel():" << paths;

	for (int i = 0; i < paths.length(); i++) 
	{
		QUrl localFilePath;

		if (paths[i].isLocalFile())
		{
			// Remove the "file:///" if present
			localFilePath = paths[i].toLocalFile();
		}
		else
		{
			localFilePath = paths[i];
		}

		m_vtkFboItem->addModelFromFile(localFilePath);
		Sleep(20);
	}
}

void CanvasHandler::openModels(const QList<QUrl> &paths) const
{
	QList<QUrl> filePaths;
	for (int i = 0; i < paths.length(); i++) 
	{
		if (paths[i].isLocalFile())
		{
			// Remove the "file:///" if present
			filePaths.append(paths[i].toLocalFile());
		}
		else
		{
			filePaths.append(paths[i]);
		}
	}
	m_vtkFboItem->addModelFromFiles(filePaths);
}

void CanvasHandler::saveModel(const QUrl &path) const
{
	qDebug() << "CanvasHandler::saveModel():" << path;

	QUrl localFilePath;

	if (path.isLocalFile())
	{
		// Remove the "file:///" if present
		localFilePath = path.toLocalFile();
	}
	else
	{
		localFilePath = path;
	}

	m_vtkFboItem->saveModel(localFilePath);
}

void CanvasHandler::closeModel() const
{
	if (!m_vtkFboItem->isModelSelected())
	{
		return;
	}
	m_vtkFboItem->closeModel();
}

void CanvasHandler::closeAllModel() const
{
	m_vtkFboItem->closeAllModel();
}

bool CanvasHandler::isModelExtensionValid(const QUrl &modelPath) const
{
	if (modelPath.toString().toLower().endsWith(".stl") || modelPath.toString().toLower().endsWith(".obj"))
	{
		return true;
	}

	return false;
}


void CanvasHandler::mousePressEvent(const int button, const int screenX, const int screenY) const
{
	qDebug() << "CanvasHandler::mousePressEvent()";

	m_vtkFboItem->selectModel(screenX, screenY);
}

void CanvasHandler::mouseMoveEvent(const int button, const int screenX, const int screenY)
{
	if (!m_vtkFboItem->isModelSelected())
	{
		return;
	}

	if (!m_draggingMouse)
	{
		m_draggingMouse = true;

		m_previousWorldX = m_vtkFboItem->getSelectedModelPositionX();
		m_previousWorldY = m_vtkFboItem->getSelectedModelPositionY();
	}

	CommandModelTranslate::TranslateParams_t translateParams;

	translateParams.screenX = screenX;
	translateParams.screenY = screenY;

	m_vtkFboItem->translateModel(translateParams, true);
}

void CanvasHandler::mouseReleaseEvent(const int button, const int screenX, const int screenY)
{
	qDebug() << "CanvasHandler::mouseReleaseEvent()";

	if (!m_vtkFboItem->isModelSelected())
	{
		return;
	}

	if (m_draggingMouse)
	{
		m_draggingMouse = false;

		CommandModelTranslate::TranslateParams_t translateParams;

		translateParams.screenX = screenX;
		translateParams.screenY = screenY;
		translateParams.previousPositionX = m_previousWorldX;
		translateParams.previousPositionY = m_previousWorldY;

		m_vtkFboItem->translateModel(translateParams, false);
	}
}

void CanvasHandler::resetModelPositionEvent()
{
	if (!m_vtkFboItem->isModelSelected())
	{
		return;
	}

	CommandModelTranslate::TranslateParams_t translateParams;
	translateParams.reset = true;
	m_vtkFboItem->translateModel(translateParams, false);
}
bool CanvasHandler::getIsModelSelected() const
{
	// QVTKFramebufferObjectItem might not be initialized when QML loads
	if (!m_vtkFboItem)
	{
		return 0;
	}

	return m_vtkFboItem->isModelSelected();
}

double CanvasHandler::getSelectedModelPositionX() const
{
	// QVTKFramebufferObjectItem might not be initialized when QML loads
	if (!m_vtkFboItem)
	{
		return 0;
	}

	return m_vtkFboItem->getSelectedModelPositionX();
}

double CanvasHandler::getSelectedModelPositionY() const
{
	// QVTKFramebufferObjectItem might not be initialized when QML loads
	if (!m_vtkFboItem)
	{
		return 0;
	}

	return m_vtkFboItem->getSelectedModelPositionY();
}

int CanvasHandler::getSelecteModelRepresentation() const
{
	if (!m_vtkFboItem)
	{
		return 2;
	}

	return m_vtkFboItem->getSelecteModelRepresentation();
}

double CanvasHandler::getSelecteModelDecreasePolygons() const
{
	if (!m_vtkFboItem)
	{
		return 0.0;
	}

	return m_vtkFboItem->getSelecteModelDecreasePolygons();
}

int CanvasHandler::getSelecteModelIncreasePolygons() const
{
	if (!m_vtkFboItem)
	{
		return 0;
	}

	return m_vtkFboItem->getSelecteModelIncreasePolygons();
}

double CanvasHandler::getSelecteModelOpacity() const
{
	if (!m_vtkFboItem)
	{
		return 1.0;
	}

	return m_vtkFboItem->getSelecteModelOpacity();
}

int CanvasHandler::getSelecteModelSmooth() const
{
	if (!m_vtkFboItem)
	{
		return 0;
	}

	return m_vtkFboItem->getSelecteModelSmooth();
}

bool CanvasHandler::getSelecteModelGI() const
{
	if (!m_vtkFboItem)
	{
		return false;
	}

	return m_vtkFboItem->getSelecteModelGI();
}

int CanvasHandler::getSelecteModelColorR() const
{
	if (!m_vtkFboItem)
	{
		return 255;
	}

	return m_vtkFboItem->getSelectedModelColorR();
}

int CanvasHandler::getSelecteModelColorG() const
{
	if (!m_vtkFboItem)
	{
		return 255;
	}

	return m_vtkFboItem->getSelectedModelColorG();
}

int CanvasHandler::getSelecteModelColorB() const
{
	if (!m_vtkFboItem)
	{
		return 255;
	}

	return m_vtkFboItem->getSelectedModelColorB();
}

int CanvasHandler::getSelecteModelPolygons() const
{
	if (!m_vtkFboItem)
	{
		return 0;
	}

	return m_vtkFboItem->getSelecteModelPolygons();
}


int CanvasHandler::getSelecteModelPoints() const
{
	if (!m_vtkFboItem)
	{
		return 0;
	}

	return m_vtkFboItem->getSelecteModelPoints();
}

QString CanvasHandler::getSelecteModelFileName() const
{
	if (!m_vtkFboItem)
	{
		return 0;
	}

	return m_vtkFboItem->getSelecteModelFileName();
}

double CanvasHandler::getSelecteModelVolume() const
{
	if (!m_vtkFboItem)
	{
		return 0;
	}

	return m_vtkFboItem->getSelecteModelVolume();
}

double CanvasHandler::getSelecteModelSurfaceArea() const
{
	if (!m_vtkFboItem)
	{
		return 0;
	}

	return m_vtkFboItem->getSelecteModelSurfaceArea();
}

double CanvasHandler::getSelecteModelMaxAreaOfCell() const
{
	if (!m_vtkFboItem)
	{
		return 0;
	}

	return m_vtkFboItem->getSelecteModelMaxAreaOfCell();
}

double CanvasHandler::getSelecteModelMinAreaOfCell() const
{
	if (!m_vtkFboItem)
	{
		return 0;
	}

	return m_vtkFboItem->getSelecteModelMinAreaOfCell();
}

int CanvasHandler::getModelCount() const
{
	return m_vtkFboItem->getModelCount();
}


void CanvasHandler::setModelRepresentation(const int representationOption)
{
	if (!m_vtkFboItem->isModelSelected())
	{
		return;
	}

	CommandModelActor::ActorParams_t actorParams;
	actorParams.mode = "setModelRepresentation";
	actorParams.valueI = representationOption;
	m_vtkFboItem->actorModel(actorParams);
}

void CanvasHandler::setModelDecreasePolygons(const double polygons)
{
	if (!m_vtkFboItem->isModelSelected())
	{
		return;
	}

	CommandModelActor::ActorParams_t actorParams;
	actorParams.mode = "setModelDecreasePolygons";
	actorParams.valueD = polygons;
	m_vtkFboItem->actorModel(actorParams);
}

void CanvasHandler::setModelIncreasePolygons(const int iterations)
{
	if (!m_vtkFboItem->isModelSelected())
	{
		return;
	}

	CommandModelActor::ActorParams_t actorParams;
	actorParams.mode = "setModelIncreasePolygons";
	actorParams.valueI = iterations;
	m_vtkFboItem->actorModel(actorParams);
}

void CanvasHandler::setModelOpacity(const double opacity)
{
	if (!m_vtkFboItem->isModelSelected())
	{
		return;
	}

	CommandModelActor::ActorParams_t actorParams;
	actorParams.mode = "setModelOpacity";
	actorParams.valueD = opacity;
	m_vtkFboItem->actorModel(actorParams);
}

void CanvasHandler::setGouraudInterpolation(const bool gouraudInterpolation)
{
	if (!m_vtkFboItem->isModelSelected())
	{
		return;
	}

	CommandModelActor::ActorParams_t actorParams;
	actorParams.mode = "setModelGouraudInterpolation";
	actorParams.valueB = gouraudInterpolation;
	m_vtkFboItem->actorModel(actorParams);
}

void CanvasHandler::setModelColorR(const int colorR)
{
	m_modelColorR = colorR;
	setModelColor(m_modelColorR, m_modelColorG, m_modelColorB);
}

void CanvasHandler::setModelColorG(const int colorG)
{
	m_modelColorG = colorG;
	setModelColor(m_modelColorR, m_modelColorG, m_modelColorB);
}

void CanvasHandler::setModelColorB(const int colorB)
{
	m_modelColorB = colorB;
	setModelColor(m_modelColorR, m_modelColorG, m_modelColorB);
}

void CanvasHandler::setModelColor(const int colorR, const int colorG, const int colorB)
{
	if (!m_vtkFboItem->isModelSelected())
	{
		// 沒有選擇模型就返回
		return;
	}

	CommandModelActor::ActorParams_t actorParams; // 宣告變數 類型 = ActorParams_t
	actorParams.mode = "setModelColor";
	actorParams.color = QColor(colorR, colorG, colorB);
	m_vtkFboItem->actorModel(actorParams);
}

void CanvasHandler::setModelSmooth(const int value)
{
	if (!m_vtkFboItem->isModelSelected())
	{
		// 沒有選擇模型就返回
		return;
	}
	// 自定義的ActorParams Struct
	// 一個結構體，用於模型功能的
	// std::shared_ptr<Model> model; 指定的模型
	// std::string mode; 功能類型
	// int valueI{ 0 }; 整數數值
	// double valueD{ 0 }; 雙精度浮點數值
	// float valueF{ 0 }; 單精度浮點數值
	// bool valueB = false; 布林值
	// QColor color; 顏色數值
	CommandModelActor::ActorParams_t actorParams;
	actorParams.mode = "setModelSmooth";
	actorParams.valueI = value;
	m_vtkFboItem->actorModel(actorParams);
}
