#ifndef MODELSTRUCT_H
#define MODELSTRUCT_H

#include <QMetaType>
#include <qstring.h>

struct ModelInfo
{
	int colorR{ 255 };
	int colorG{ 255 };
	int colorB{ 255 };
};
Q_DECLARE_METATYPE(ModelInfo);

#endif MODELSTRUCT_H