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

// 入口函數
CanvasHandler::CanvasHandler(int argc, char **argv)
{
	// QT應用程式的變數，應該算是對象
	QApplication app(argc, argv);
	// QT應用程式引擎
	QQmlApplicationEngine engine;

	// 設定應用程式名稱
	app.setApplicationName("QtVTK");
	// 設定icon
	app.setWindowIcon(QIcon(":/resources/bq.ico"));

	// Register QML types
	// Register QML types
	// (QML import 名稱, 主版本號, 次版本號, QML class的名稱 )
	// 第四個參數首字必須大寫，否則會報錯
	// 主要是main.qml調用C++的class，連接C++及QML的一個函數
	qmlRegisterType<QVTKFramebufferObjectItem>("QtVTK", 1, 0, "VtkFboItem");

	// Create classes instances
	// 創建一個processing engine class
	m_processingEngine = std::shared_ptr<ProcessingEngine>(new ProcessingEngine());

	// Expose C++ classes to QML
	// 把C++ class 共享給 QML
	QQmlContext* ctxt = engine.rootContext();

	// 這裡是main.qml裡面的所使用的對象名稱，在main.qml中需要使用該名稱來呼叫class的函數
	ctxt->setContextProperty("canvasHandler", this);

	// QT的一種窗口類型風格，所看到的按鈕那些都是套用該風格
	QQuickStyle::setStyle("Material");

	// Load main QML file
	// 載入main QML
	engine.load(QUrl("qrc:/resources/main.qml"));

	// Get reference to the QVTKFramebufferObjectItem created in QML
	// We cannot use smart pointers because this object must be deleted by QML
	QObject *rootObject = engine.rootObjects().first();
	m_vtkFboItem = rootObject->findChild<QVTKFramebufferObjectItem*>("vtkFboItem");

	// Give the vtkFboItem reference to the CanvasHandler
	if (m_vtkFboItem)
	{
		qDebug() << "CanvasHandler::CanvasHandler: setting vtkFboItem to CanvasHandler";

		// 設定ProcessingEngine，將processing engine class傳入
		m_vtkFboItem->setProcessingEngine(m_processingEngine);

		// 信號與槽的機制是QT的核心
		// 信號(Signal)：簡單來說就是介面的物件做出事件時，發送信號至C++內做交互的動作，如PushButton的Clicked事件
		// 槽(Slot)：回調函數，信號發送後接收的函數，當信號發送時，關聯的函數會被自動執行
		// connect函數是QT中連接信號與槽的函數，參數(發送信號的對象地址, 發送信號的函數, 接收函數對象的地址, 接收的函數)

		// 底下為建立連線，原作者將每一個檔案互相串連，最後由Renderer或Model去emit信號
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

	// app啟動，返回應該是一個窗口句柄或是進程句柄? 底下debug只有說是code，但應該是那兩個其中一個吧?
	int rc = app.exec();

	qDebug() << "CanvasHandler::CanvasHandler: Execution finished with return code:" << rc;
}


void CanvasHandler::startApplication() const
{
	qDebug() << "CanvasHandler::startApplication()";

	disconnect(m_vtkFboItem, &QVTKFramebufferObjectItem::rendererInitialized, this, &CanvasHandler::startApplication);
}

// begin-----------以下為介面開關、打開 關閉 合併模型等功能事件函數-----------

void CanvasHandler::showPlatform(const bool checked) const
{
	// to item to renderer
	m_vtkFboItem->showPlatform(checked);
}

void CanvasHandler::showAxes(const bool checked) const
{
	// to item to renderer
	m_vtkFboItem->showAxes(checked);
}

void CanvasHandler::createCube(const QString x, const QString y, const QString z) const
{
	// to item to processing engine
	m_vtkFboItem->createCube(x.toDouble(), y.toDouble(), z.toDouble());
}

void CanvasHandler::createSphare(const QString radius) const
{
	// to item to processing engine
	m_vtkFboItem->createSphare(radius.toDouble());
}

void CanvasHandler::openModel(const QList<QUrl> &paths) const
{
	qDebug() << "CanvasHandler::openModel():" << paths;

	// 支持多檔案不同類型導入
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

		// 使用原作者提供的addModelFromFile each model to add
		m_vtkFboItem->addModelFromFile(localFilePath);
		// 電腦太快會造成記憶體崩潰 加個Sleep讓他休息一下
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
	// 基於原作者提供的addModelFromFile進行改良，將導入的模型進行合併
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
	// to item to my save class
	m_vtkFboItem->saveModel(localFilePath);
}

void CanvasHandler::closeModel() const
{
	if (!m_vtkFboItem->isModelSelected())
	{
		return;
	}
	// to item to renderer
	m_vtkFboItem->closeModel();
}

void CanvasHandler::closeAllModel() const
{
	// to item to renderer
	m_vtkFboItem->closeAllModel();
}

// end.

bool CanvasHandler::isModelExtensionValid(const QUrl &modelPath) const
{
	// 檢查文件是否合法，但沒使用到
	if (modelPath.toString().toLower().endsWith(".stl") || modelPath.toString().toLower().endsWith(".obj"))
	{
		return true;
	}

	return false;
}

void CanvasHandler::mousePressEvent(const int button, const int screenX, const int screenY) const
{
	// 滑鼠按下事件
	qDebug() << "CanvasHandler::mousePressEvent()";
	// to item to mouse event
	// 抓滑鼠位置，實際選擇模型的函數不是在這裡，是在renderer內有個pick事件
	m_vtkFboItem->selectModel(screenX, screenY);
}

void CanvasHandler::mouseMoveEvent(const int button, const int screenX, const int screenY)
{
	// 滑鼠移動事件
	if (!m_vtkFboItem->isModelSelected())
	{
		return;
	}

	if (!m_draggingMouse)
	{
		m_draggingMouse = true;

		// 模型原本的位置
		m_previousWorldX = m_vtkFboItem->getSelectedModelPositionX();
		m_previousWorldY = m_vtkFboItem->getSelectedModelPositionY();
	}

	// 一個結構體，用於模型移動的
	// std::shared_ptr<Model> model; 指定的模型
	// int screenX{ 0 }; 視窗的座標X
	// int screenY{ 0 }; 視窗的座標Y
	// double previousPositionX{ 0 }; 模型原本的位置X
	// double previousPositionY{ 0 }; 模型原本的位置Y
	// double targetPositionX{ 0 }; 真實位置X
	// double targetPositionY{ 0 }; 真實位置Y
	// bool reset = false; 是否重置到原點

	CommandModelTranslate::TranslateParams_t translateParams;

	translateParams.screenX = screenX;
	translateParams.screenY = screenY;

	m_vtkFboItem->translateModel(translateParams, true);
}

void CanvasHandler::mouseReleaseEvent(const int button, const int screenX, const int screenY)
{
	// 滑鼠鬆開事件
	qDebug() << "CanvasHandler::mouseReleaseEvent()";

	if (!m_vtkFboItem->isModelSelected())
	{
		return;
	}

	if (m_draggingMouse)
	{
		m_draggingMouse = false;

		CommandModelTranslate::TranslateParams_t translateParams;
		// 視窗上的座標
		translateParams.screenX = screenX;
		translateParams.screenY = screenY;
		// 模型原本的位置
		translateParams.previousPositionX = m_previousWorldX;
		translateParams.previousPositionY = m_previousWorldY;

		// TranslateParams_t:
			// std::shared_ptr<Model> model; 指定的模型
			// int screenX{ 0 }; 視窗的座標X
			// int screenY{ 0 }; 視窗的座標Y
			// double previousPositionX{ 0 }; 模型原本的位置X
			// double previousPositionY{ 0 }; 模型原本的位置Y
			// double targetPositionX{ 0 }; 真實位置X
			// double targetPositionY{ 0 }; 真實位置Y
			// bool reset = false; 是否重置到原點

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
	// 基於原結構體上，新增一個reset功能
	// 這裡已經先在其他地方寫好了，直接把reset設定為true，傳入即可重置位置
	translateParams.reset = true;
	m_vtkFboItem->translateModel(translateParams, false);
}

// begin-----------以下為C++傳輸qml數據用的函數-----------
// handler call item call renderer
// renderer return item return handler
// qml get the value

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
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelectedModelPositionX();
}

double CanvasHandler::getSelectedModelPositionY() const
{
	// QVTKFramebufferObjectItem might not be initialized when QML loads
	if (!m_vtkFboItem)
	{
		return 0;
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelectedModelPositionY();
}

int CanvasHandler::getSelecteModelRepresentation() const
{
	if (!m_vtkFboItem)
	{
		// 默認值:下拉式選單第三個(面)
		return 2;
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelecteModelRepresentation();
}

double CanvasHandler::getSelecteModelDecreasePolygons() const
{
	if (!m_vtkFboItem)
	{
		// 默認值:0
		return 0.0;
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelecteModelDecreasePolygons();
}

int CanvasHandler::getSelecteModelIncreasePolygons() const
{
	if (!m_vtkFboItem)
	{
		// 默認值:0
		return 0;
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelecteModelIncreasePolygons();
}

double CanvasHandler::getSelecteModelOpacity() const
{
	if (!m_vtkFboItem)
	{
		// 默認值:1 不透明
		return 1.0;
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelecteModelOpacity();
}

int CanvasHandler::getSelecteModelSmooth() const
{
	if (!m_vtkFboItem)
	{
		// 默認值:0
		return 0;
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelecteModelSmooth();
}

bool CanvasHandler::getSelecteModelGI() const
{
	if (!m_vtkFboItem)
	{
		// 默認值:關閉
		return false;
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelecteModelGI();
}

int CanvasHandler::getSelecteModelColorR() const
{
	if (!m_vtkFboItem)
	{
		// 默認值:紅色255
		return 255;
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelectedModelColorR();
}

int CanvasHandler::getSelecteModelColorG() const
{
	if (!m_vtkFboItem)
	{
		// 默認值:綠色255
		return 255;
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelectedModelColorG();
}

int CanvasHandler::getSelecteModelColorB() const
{
	if (!m_vtkFboItem)
	{
		// 默認值:藍色255
		return 255;
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelectedModelColorB();
}

int CanvasHandler::getSelecteModelPolygons() const
{
	if (!m_vtkFboItem)
	{
		return 0;
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelecteModelPolygons();
}


int CanvasHandler::getSelecteModelPoints() const
{
	if (!m_vtkFboItem)
	{
		return 0;
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelecteModelPoints();
}

QString CanvasHandler::getSelecteModelFileName() const
{
	if (!m_vtkFboItem)
	{
		return "";
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelecteModelFileName();
}

double CanvasHandler::getSelecteModelVolume() const
{
	if (!m_vtkFboItem)
	{
		return 0;
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelecteModelVolume();
}

double CanvasHandler::getSelecteModelSurfaceArea() const
{
	if (!m_vtkFboItem)
	{
		return 0;
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelecteModelSurfaceArea();
}

double CanvasHandler::getSelecteModelMaxAreaOfCell() const
{
	if (!m_vtkFboItem)
	{
		return 0;
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelecteModelMaxAreaOfCell();
}

double CanvasHandler::getSelecteModelMinAreaOfCell() const
{
	if (!m_vtkFboItem)
	{
		return 0;
	}
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getSelecteModelMinAreaOfCell();
}

int CanvasHandler::getModelCount() const
{
	// return render return item return handler then qml will got the value
	return m_vtkFboItem->getModelCount();
}

// end.
// begin-----------以下為qml觸發事件所調用的函數-----------
// qml按下按鈕、切換功能之類的事件
// to item then item call command to execute my actor class

// ActorParams_t:
	// 一個結構體，用於模型功能設定
	// std::shared_ptr<Model> model; 指定的模型
	// std::string mode{ 0 }; 功能模式
	// int valueI{ 0 }; 整數數值
	// double valueD{ 0 }; 雙精度浮點數值
	// float valueF{ 0 }; 單精度浮點數值
	// bool valueB; 布林值
	// QColor color; 顏色值

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
	// record
	m_modelColorR = colorR;
	setModelColor(m_modelColorR, m_modelColorG, m_modelColorB);
}

void CanvasHandler::setModelColorG(const int colorG)
{
	// record
	m_modelColorG = colorG;
	setModelColor(m_modelColorR, m_modelColorG, m_modelColorB);
}

void CanvasHandler::setModelColorB(const int colorB)
{
	// record
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

	CommandModelActor::ActorParams_t actorParams;
	actorParams.mode = "setModelSmooth";
	actorParams.valueI = value;
	m_vtkFboItem->actorModel(actorParams);
}
