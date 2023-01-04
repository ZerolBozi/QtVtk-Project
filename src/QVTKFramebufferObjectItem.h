#ifndef QVTKFRAMEBUFFEROBJECTITEM_H
#define QVTKFRAMEBUFFEROBJECTITEM_H

#include <memory>
#include <queue>
#include <mutex>

#include <QtQuick/QQuickFramebufferObject>

#include "CommandModelTranslate.h"
#include "CommandModelActor.h"
#include "CommandModelSave.h"


class CommandModel;
class Model;
class ProcessingEngine;
class QVTKFramebufferObjectRenderer;

class QVTKFramebufferObjectItem : public QQuickFramebufferObject
{
	Q_OBJECT

public:
	QVTKFramebufferObjectItem();

	Renderer *createRenderer() const Q_DECL_OVERRIDE;
	void setVtkFboRenderer(QVTKFramebufferObjectRenderer*);
	bool isInitialized() const;
	void setProcessingEngine(const std::shared_ptr<ProcessingEngine> processingEngine);

	// Model releated functions
	bool isModelSelected() const;

	double getSelectedModelPositionX() const;
	double getSelectedModelPositionY() const;
	int getSelecteModelRepresentation() const;
	double getSelecteModelDecreasePolygons() const;
	int getSelecteModelIncreasePolygons() const;
	double getSelecteModelOpacity() const;
	int getSelecteModelSmooth() const;
	bool getSelecteModelGI() const;
	int getSelectedModelColorR() const;
	int getSelectedModelColorG() const;
	int getSelectedModelColorB() const;
	int getSelecteModelPolygons() const;
	int getSelecteModelPoints() const;
	QString getSelecteModelFileName() const;
	double getSelecteModelVolume() const;
	double getSelecteModelSurfaceArea() const;
	double getSelecteModelMaxAreaOfCell() const;
	double getSelecteModelMinAreaOfCell() const;
	int getModelCount() const;

	void showPlatform(const bool checked);
	void showAxes(const bool checked);
	void createCube(const double x, const double y, const double z);
	void createSphare(const double radius);
	void selectModel(const int screenX, const int screenY);
	void resetModelSelection();
	void addModelFromFile(const QUrl &modelPath);
	void addModelFromFiles(const QList<QUrl> &modelPaths);
	void saveModel(const QUrl &modelPath);
	void closeModel();
	void closeAllModel();

	void translateModel(CommandModelTranslate::TranslateParams_t &translateData, const bool inTransition);
	void actorModel(CommandModelActor::ActorParams_t &acotrData);

	// Camera related functions
	void wheelEvent(QWheelEvent *e) override;
	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;
	void mouseMoveEvent(QMouseEvent *e) override;

	QMouseEvent *getLastMouseLeftButton();
	QMouseEvent *getLastMouseButton();
	QMouseEvent *getLastMoveEvent();
	QWheelEvent *getLastWheelEvent();

	void resetCamera();

	int getModelsRepresentation() const;
	double getModelsOpacity() const;
	bool getGourauInterpolation() const;
	int getModelColorR() const;
	int getModelColorG() const;
	int getModelColorB() const;

	void setModelsRepresentation(const int representationOption);
	void setModelsOpacity(const double opacity);
	void setGouraudInterpolation(const bool gouraudInterpolation);
	void setModelColorR(const int colorR);
	void setModelColorG(const int colorG);
	void setModelColorB(const int colorB);

	CommandModel* getCommandsQueueFront() const;
	void commandsQueuePop();
	bool isCommandsQueueEmpty() const;
	void lockCommandsQueueMutex();
	void unlockCommandsQueueMutex();

signals:
	void rendererInitialized();

	void isModelSelectedChanged();
	void selectedModelPositionXChanged();
	void selectedModelPositionYChanged();

	void isModelRepresentationChanged();
	void isModelDecreasePolygonsChanged();
	void isModelIncreasePolygonsChanged();
	void isModelOpacityChanged();
	void isModelSmoothChanged();
	void isModelGIChanged();
	void isModelColorRChanged();
	void isModelColorGChanged();
	void isModelColorBChanged();
	void isModelPolygonsChanged();
	void isModelPointsChanged();
	void isModelFileNameChanged();
	void isModelVolumeChanged();
	void isModelSurfaceAreaChanged();
	void isModelMaxAreaOfCellChanged();
	void isModelMinAreaOfCellChanged();
	void isModelCountChanged();

	void addModelFromFileDone();
	void addModelFromFileError(QString error);

private:
	void addCommand(CommandModel* command);

	QVTKFramebufferObjectRenderer *m_vtkFboRenderer = nullptr;
	std::shared_ptr<ProcessingEngine> m_processingEngine;

	std::queue<CommandModel*> m_commandsQueue;
	std::mutex m_commandsQueueMutex;

	std::shared_ptr<QMouseEvent> m_lastMouseLeftButton;
	std::shared_ptr<QMouseEvent> m_lastMouseButton;
	std::shared_ptr<QMouseEvent> m_lastMouseMove;
	std::shared_ptr<QWheelEvent> m_lastMouseWheel;

	int m_modelsRepresentationOption = 2;
	double m_modelsOpacity = 1.0;
	bool m_gouraudInterpolation = false;
	int m_modelColorR = 3;
	int m_modelColorG = 169;
	int m_modelColorB = 244;
};

#endif // QVTKFRAMEBUFFEROBJECTITEM_H
