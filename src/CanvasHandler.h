#ifndef CANVASHANDLER_H
#define CANVASHANDLER_H

#include <memory>

#include <QObject>
#include <QUrl>

class ProcessingEngine;
class QVTKFramebufferObjectItem;

class CanvasHandler : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool showFileDialog MEMBER m_showFileDialog NOTIFY showFileDialogChanged)
	Q_PROPERTY(bool showSaveFileDialog MEMBER m_showSaveFileDialog NOTIFY showFileDialogChanged)
	Q_PROPERTY(bool showFilesDialog MEMBER m_showFilesDialog NOTIFY showFileDialogChanged)
	Q_PROPERTY(bool isModelSelected READ getIsModelSelected NOTIFY isModelSelectedChanged)
	Q_PROPERTY(double modelPositionX READ getSelectedModelPositionX NOTIFY selectedModelPositionXChanged)
	Q_PROPERTY(double modelPositionY READ getSelectedModelPositionY NOTIFY selectedModelPositionYChanged)
	Q_PROPERTY(int modelRepresentation READ getSelecteModelRepresentation NOTIFY isModelRepresentationChanged)
	Q_PROPERTY(double modelDecreasePolygons READ getSelecteModelDecreasePolygons NOTIFY isModelDecreasePolygonsChanged)
	Q_PROPERTY(int modelIncreasePolygons READ getSelecteModelIncreasePolygons NOTIFY isModelIncreasePolygonsChanged)
	Q_PROPERTY(double modelOpacity READ getSelecteModelOpacity NOTIFY isModelOpacityChanged)
	Q_PROPERTY(int modelSmooth READ getSelecteModelSmooth NOTIFY isModelSmoothChanged)
	Q_PROPERTY(bool modelGouraud READ getSelecteModelGI NOTIFY isModelGIChanged)
	Q_PROPERTY(int modelColorR READ getSelecteModelColorR NOTIFY isModelColorRChanged)
	Q_PROPERTY(int modelColorG READ getSelecteModelColorG NOTIFY isModelColorGChanged)
	Q_PROPERTY(int modelColorB READ getSelecteModelColorB NOTIFY isModelColorBChanged)
	Q_PROPERTY(int modelPolygons READ getSelecteModelPolygons NOTIFY isModelPolygonsChanged)
	Q_PROPERTY(int modelPoints READ getSelecteModelPoints NOTIFY isModelPointsChanged)
	Q_PROPERTY(QString modelFileName READ getSelecteModelFileName NOTIFY isModelFileNameChanged)
	Q_PROPERTY(double modelVolume READ getSelecteModelVolume NOTIFY isModelVolumeChanged)
	Q_PROPERTY(double modelSurfaceArea READ getSelecteModelSurfaceArea NOTIFY isModelSurfaceAreaChanged)
	Q_PROPERTY(double modelMaxAreaOfCell READ getSelecteModelMaxAreaOfCell NOTIFY isModelMaxAreaOfCellChanged)
	Q_PROPERTY(double modelMinAreaOfCell READ getSelecteModelMinAreaOfCell NOTIFY isModelMinAreaOfCellChanged)
	Q_PROPERTY(int modelCount READ getModelCount NOTIFY isModelCountChanged)

public:
	CanvasHandler(int argc, char **argv);

	Q_INVOKABLE void showPlatform(const bool checked) const;
	Q_INVOKABLE void showAxes(const bool checked) const;
	Q_INVOKABLE void createCube(const QString x, const QString y, const QString z) const;
	Q_INVOKABLE void createSphare(const QString radius) const;
	Q_INVOKABLE void openModel(const QList<QUrl> &paths) const;
	Q_INVOKABLE void openModels(const QList<QUrl> &paths) const;
	Q_INVOKABLE void saveModel(const QUrl &path) const;
	Q_INVOKABLE void closeModel() const;
	Q_INVOKABLE void closeAllModel() const;

	Q_INVOKABLE void mousePressEvent(const int button, const int mouseX, const int mouseY) const;
	Q_INVOKABLE void mouseMoveEvent(const int button, const int mouseX, const int mouseY);
	Q_INVOKABLE void mouseReleaseEvent(const int button, const int mouseX, const int mouseY);

	Q_INVOKABLE void resetModelPositionEvent();

	bool getIsModelSelected() const;
	double getSelectedModelPositionX() const;
	double getSelectedModelPositionY() const;
	int getSelecteModelRepresentation() const;
	double getSelecteModelDecreasePolygons() const;
	int getSelecteModelIncreasePolygons() const;
	double getSelecteModelOpacity() const;
	int getSelecteModelSmooth() const;
	bool getSelecteModelGI() const;
	int getSelecteModelColorR() const; 
	int getSelecteModelColorG() const;
	int getSelecteModelColorB() const;
	int getSelecteModelPolygons() const;
	int getSelecteModelPoints() const;
	QString getSelecteModelFileName() const;
	double getSelecteModelVolume() const;
	double getSelecteModelSurfaceArea() const;
	double getSelecteModelMaxAreaOfCell() const;
	double getSelecteModelMinAreaOfCell() const;
	int getModelCount() const;

	Q_INVOKABLE void setModelRepresentation(const int representationOption);
	Q_INVOKABLE void setModelDecreasePolygons(const double polygons);
	Q_INVOKABLE void setModelIncreasePolygons(const int iterations);
	Q_INVOKABLE void setModelOpacity(const double opacity);
	Q_INVOKABLE void setGouraudInterpolation(const bool gouraudInterpolation);
	Q_INVOKABLE void setModelColorR(const int colorR);
	Q_INVOKABLE void setModelColorG(const int colorG);
	Q_INVOKABLE void setModelColorB(const int colorB);
	Q_INVOKABLE void setModelSmooth(const int value);

public slots:
	void startApplication() const;

signals:
	void showFileDialogChanged();

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

private:
	bool isModelExtensionValid(const QUrl &modelPath) const;

	void setModelColor(const int colorR, const int colorG, const int colorB);

	std::shared_ptr<ProcessingEngine> m_processingEngine;
	QVTKFramebufferObjectItem *m_vtkFboItem = nullptr;

	double m_previousWorldX = 0;
	double m_previousWorldY = 0;
	bool m_draggingMouse = false;
	bool m_showFileDialog = false;
	bool m_showSaveFileDialog = false;
	bool m_showFilesDialog = false;

	int m_modelColorR = 255;
	int m_modelColorG = 255;
	int m_modelColorB = 255;
};

#endif // CANVASHANDLER_H
