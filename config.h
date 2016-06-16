#ifndef CONFIG_H
#define CONFIG_H
#include <QtCore>

static QString X = "X:";
static QString Y = "Y:";
static QString W = "宽度:";
static QString H = "高度:";
static QString COMOBJ = "comObj";
static QString IMGOBJ = "imgObj";
static QString COMGRPLYT = "ComGpBLyt";
static QString IMGGRPLYT = "ImgGpBLyt";
static QString DEFAULT = "default";
static QString LIST = "list";
static QString GEOMETRY ="geometry";
static QString ENUM = "enum";

static QString NAME = "-name";
static QString WIDGET ="widget";
static QString CLASS = "-class";
static QString LAYOUT ="layout";
static QString PROPERTY = "property";
static QString ITEM="item";
static QString RECT ="rect";
static QString QFRAME = "QFrame";
static QString QLABEL = "QLabel";
static QString CAPTION = "caption";

static const char* DKEY_CAPTION = "caption";

static char * DKEY_IMGIDX = "ImageIndex";
static char * DKEY_IMAGELST = "ImageList";
static char * DKEY_UID= "uid";
static char * DKEY_DYN= "dynProperty";
static char *DKEY_PRONAME = "PropertyOwner";
const QMargins propertyMarg = QMargins(2,12,2,50);

const int labHeight = 25;

#endif // CONFIG_H