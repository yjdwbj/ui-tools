#ifndef CONFIG_H
#define CONFIG_H
#include <QtCore>

static QString X = "X:";
static QString Y = "Y:";
static QString W = "宽度:";
static QString H = "高度:";
static QString WIDTH = "width";
static QString HEIGHT="height";
static QString COMOBJ = "comObj";
static QString IMGOBJ = "imgObj";
static QString COMGRPLYT = "ComGpBLyt";
static QString IMGGRPLYT = "ImgGpBLyt";
static QString DEFAULT = "default";
static QString LIST = "list";
static QString GEOMETRY ="geometry";
static QString ENUM = "enum";
static QString IMAGE = "image";
static QString NAME = "-name";
static QString WIDGET ="widget";
static QString CLASS = "-class";
static QString LAYOUT ="layout";
static QString PROPERTY = "property";
static QString ITEM="item";
static QString SIZE = "size";
static QString KEY_RECT ="rect";
static QString QFRAME = "QFrame";
static QString QLABEL = "QLabel";
static QString CAPTION = "caption";
static QString MAX = "max";
static QString MIN = "min";
static QString LISTIMAGE = "ListImage";
static QString ICON = "icon";
static QString NUMBER ="number";
static QString TEXT = "text";
static QString MAXLEN = "maxlength";
static QString ACTPAGE = "activePage";
static QString PAGES = "pages";
static QString UID = "id";
static QString COLOR = "color";
static QString BkIMAGE = "background-image";


static const char  DKEY_CAPTION[] = "caption";

static const char DKEY_IMGIDX[] = "ImageIndex";
static const char DKEY_IMAGELST[] = "ImageList";
static const char DKEY_UID[] = "uid";
static const char DKEY_DYN[]= "dynProperty";
static const char DKEY_PRONAME[] = "PropertyOwner";
static const char DKEY_CLSNAME[] = "clsName";
static const char DKEY_DYNVAL[] = "dynValues";
static const char DKEY_VALTYPE[] = "valueType";
static const char DKEY_SHOT[] = "screenshot";
static const char DKEY_TXT[] = "ViewName";
static const char DKEY_CATEGORY[] = "Category";
static const char DKEY_LOCALSEQ[] = "LocalSeq";

/* Class Name */
static const QString CN_NEWLAYOUT = "NewLayout";
static const QString CN_NEWFRAME = "NewFrame";
static const QString CN_NEWLABEL = "NewLabel";
static const QString CN_NEWLAYER = "NewLayer";
static const QString CN_SSNAME = "ScenesScreen";

/*  ini format */
static const QString INI_PRJSIZE = "Project/Size";
static const QString INI_PRJBAKIMG = "Project/Background";
static const QString INI_PRJLAST = "Project/LastOpen";
static const QString INI_PRJDIR = "Project/Dir";


const QMargins propertyMarg = QMargins(2,12,2,50);

const int labHeight = 25;


enum { SELECTION_HANDLE_SIZE = 6, SELECTION_MARGIN = 6 };
enum SelectionHandleState { SelectionHandleOff, SelectionHandleInactive, SelectionHandleActive };

static QSize MARGIN_SIZE = QSize(SELECTION_MARGIN * 2, SELECTION_MARGIN *2);



static const QString WIN_TITLE = "图片编辑工具";


#endif // CONFIG_H
