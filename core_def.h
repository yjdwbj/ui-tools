#ifndef CONFIG_H
#define CONFIG_H
#include <QtCore>

static QString X = "X:";
static QString Y = "Y:";
static QString W = "宽度:";
static QString H = "高度:";
static QString LX = "x";
static QString LY = "y";
static QString LEFT ="left";
static QString TOP = "top";
static QString UP = "dup";
static QString DOWN = "udown";
static QString RIGHT = "right";
static QString BOTTOM="bottom";
static QString WIDTH = "width";
static QString HEIGHT="height";
static QString COMOBJ = "comObj";
static QString IMGOBJ = "imgObj";
static QString COMGRPLYT = "ComGpBLyt";
static QString IMGGRPLYT = "ImgGpBLyt";
static QString DEFAULT = "default";
static QString LIST = "list";
static QString LISTWIDGET = "listwidget";
static QString GRIDWIDGET = "GridWidget";
static QString GEOMETRY ="geometry";
static QString ENUM = "enum";
static QString IMAGE = "image";
static QString NAME = "-name";
static QString WIDGET ="widget";
static QString BORDER="border";
static QString CLASS = "-class";
static QString LAYOUT ="layout";
static QString LAYER = "layer";
static QString COMPOENTS ="compoents";
static QString PROPERTY = "property";
static QString ITEM="item";
static QString KEY_SIZE = "size";
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
static QString ENAME = "ename";
static QString COLOR = "color";
static QString GRAYCOLOR = "gray-color";
static QString STRUCT = "struct";
static QString BAKIMAGE = "background-image";
static QString BAKCOLOR = "background-color";
static QString WTYPE = "-type";
static QString PLUGIN = "plugin";
static QString PIC_TEXT = "text-pic";
static QString MLANG = "language";
static QString ISTRUE = "disable";

static QString  QRIENTATION= "orientation";
static QString  HORIZONTAL = "Horizontal";
static QString  VERTICAL = "Vertical";
static QString  ITEMSPACE = "space";
static QString  ITEMSIZE = "sizehw";



static const char SECTION_CHAR = '|';


static const char  DKEY_CAPTION[] = "caption";
static const char DKEY_INTOLIST[] = "intoList";

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
static const char DKEY_JSONSTR[] = "JsonString";
static const char DKEY_COLOR[] = "btncolor";
static const char DKEY_BORDER_COLOR[] = "border_color";
static const char DKEY_IMGMAP[] = "bkimage_map";
static const char DKEY_EXTMAP[] = "extendMap";
static const char DKEY_ROW[] = "rowProperty";
static const char DKEY_TYPE[] = "DKtypeID";
static const char DKEY_CURVAL[] = "CurrentProperty";
static const char DKEY_EVT_TIME[] = "EventTimestamp";

/* Class Name */
static const QString CN_NEWLAYOUT = "NewLayout";
static const QString CN_NEWFRAME = "NewFrame";
static const QString CN_NEWLABEL = "NewLabel";
static const QString CN_NEWLAYER = "NewLayer";
static const QString CN_NEWLIST = "NewList";
static const QString CN_NEWGRID = "NewGrid";
//static const QString CN_LAYER = "Layer";
//static const QString CN_LAYOUT = "Layout";
static const QString CN_SSNAME = "ScenesScreen";

/*  ini format */
static const QString INI_PRJSIZE = "Project/Size";
static const QString INI_PRJBAKIMG = "Project/Background";
static const QString INI_PRJLAST = "Project/LastOpen";
static const QString INI_PRJDIR = "Project/Dir";
static const QString INI_PRJMLANG = "Project/LangugeFile";
static const QString INI_PRJSTYLE = "Project/Style";
static const QString INI_PRJJSON = "Project/TemplateJson";
static const QString INI_PRJCUSTOM = "Project/CustomTemplateDir";
static const QString INI_PRJIMAGEDIR = "Project/ImageDir";



static const char HIDE_ICON[] = ":/icon/icons/hide-30x30.png";
static const char SHOW_ICON[] = ":/icon/icons/visibility-30x30.png";




const QMargins propertyMarg = QMargins(2,12,2,50);

const int labHeight = 25;


enum { SELECTION_HANDLE_SIZE = 6, SELECTION_MARGIN = 6 };
enum SelectionHandleState { SelectionHandleOff, SelectionHandleInactive, SelectionHandleActive };

static QSize MARGIN_SIZE = QSize(SELECTION_MARGIN * 2, SELECTION_MARGIN *2);



static const QString WIN_TITLE = "图片编辑工具  ";

static QString VERSION = QString("%1 Ver:1.0-r%2 build time:<%3,%4>  ").arg(WIN_TITLE,
                                QString(APP_REVISION).mid(0,6),
                                QDate::currentDate().toString("yyyy-MM-dd"),
                                __TIME__);

#endif // CONFIG_H
