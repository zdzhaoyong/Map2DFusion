
QT       += core gui network sql declarative opengl svg
CONFIG   += shared
TEMPLATE  = lib

TARGET    = opmapwidget

DESTDIR      = ../../libs
BUILD_DIR    = ../../build/Thirdparty/$$TARGET
UI_DIR       = $$BUILD_DIR
MOC_DIR      = $$BUILD_DIR
OBJECTS_DIR  = $$BUILD_DIR

INCLUDEPATH += . ./core ./internals ./internals/projections ./mapwidget

DEFINES     += OPMAPWIDGET_LIBRARY EXTERNAL_USE


HEADERS += \
    opmapcontrol.h \
    ./core/accessmode.h \
    ./core/alllayersoftype.h \
    ./core/cache.h \
    ./core/cacheitemqueue.h \
    ./core/debugheader.h \
    ./core/diagnostics.h \
    ./core/geodecoderstatus.h \
    ./core/kibertilecache.h \
    ./core/languagetype.h \
    ./core/maptype.h \
    ./core/memorycache.h \
    ./core/opmaps.h \
    ./core/placemark.h \
    ./core/point.h \
    ./core/providerstrings.h \
    ./core/pureimage.h \
    ./core/pureimagecache.h \
    ./core/rawtile.h \
    ./core/size.h \
    ./core/tilecachequeue.h \
    ./core/urlfactory.h \
    ./internals/copyrightstrings.h \
    ./internals/core.h \
    ./internals/debugheader.h \
    ./internals/loadtask.h \
    ./internals/mousewheelzoomtype.h \
    ./internals/pointlatlng.h \
    ./internals/pureprojection.h \
    ./internals/rectangle.h \
    ./internals/rectlatlng.h \
    ./internals/sizelatlng.h \
    ./internals/tile.h \
    ./internals/tilematrix.h \
    ./mapwidget/gpsitem.h \
    ./mapwidget/homeitem.h \
    ./mapwidget/gcsitem.h \
    ./mapwidget/distanceMeasureItem.h \
    ./mapwidget/mapgraphicitem.h \
    ./mapwidget/mapripform.h \
    ./mapwidget/mapripper.h \
    ./mapwidget/opmapwidget.h \
    ./mapwidget/trailitem.h \
    ./mapwidget/traillineitem.h \
    ./mapwidget/uavitem.h \
    ./mapwidget/uavmapfollowtype.h \
    ./mapwidget/uavtrailtype.h \
    ./mapwidget/waypointitem.h \
    ./mapwidget/waypointlineitem.h \
    ./internals/projections/lks94projection.h \
    ./internals/projections/mercatorprojection.h \
    ./internals/projections/mercatorprojectionyandex.h \
    ./internals/projections/platecarreeprojection.h \
    ./internals/projections/platecarreeprojectionpergo.h \
    mapwidget/guidedTargetItem.h \
    mapwidget/circularFlightItem.h


SOURCES += \
    ./core/alllayersoftype.cpp \
    ./core/cache.cpp \
    ./core/cacheitemqueue.cpp \
    ./core/diagnostics.cpp \
    ./core/kibertilecache.cpp \
    ./core/languagetype.cpp \
    ./core/memorycache.cpp \
    ./core/opmaps.cpp \
    ./core/placemark.cpp \
    ./core/point.cpp \
    ./core/providerstrings.cpp \
    ./core/pureimage.cpp \
    ./core/pureimagecache.cpp \
    ./core/rawtile.cpp \
    ./core/size.cpp \
    ./core/tilecachequeue.cpp \
    ./core/urlfactory.cpp \
    ./internals/core.cpp \
    ./internals/loadtask.cpp \
    ./internals/MouseWheelZoomType.cpp \
    ./internals/pointlatlng.cpp \
    ./internals/pureprojection.cpp \
    ./internals/rectangle.cpp \
    ./internals/rectlatlng.cpp \
    ./internals/sizelatlng.cpp \
    ./internals/tile.cpp \
    ./internals/tilematrix.cpp \
    ./mapwidget/configuration.cpp \
    ./mapwidget/gpsitem.cpp \
    ./mapwidget/homeitem.cpp \
    ./mapwidget/gcsitem.cpp \
    ./mapwidget/distanceMeasureItem.cpp \
    ./mapwidget/mapgraphicitem.cpp \
    ./mapwidget/mapripform.cpp \
    ./mapwidget/mapripper.cpp \
    ./mapwidget/opmapwidget.cpp \
    ./mapwidget/trailitem.cpp \
    ./mapwidget/traillineitem.cpp \
    ./mapwidget/uavitem.cpp \
    ./mapwidget/waypointitem.cpp \
    ./mapwidget/waypointlineitem.cpp \
    ./internals/projections/lks94projection.cpp \
    ./internals/projections/mercatorprojection.cpp \
    ./internals/projections/mercatorprojectionyandex.cpp \
    ./internals/projections/platecarreeprojection.cpp \
    ./internals/projections/platecarreeprojectionpergo.cpp \
    mapwidget/guidedTargetItem.cpp \
    mapwidget/circularFlightItem.cpp

    
FORMS       += ./mapwidget/mapripform.ui
RESOURCES   += ./mapwidget/mapresources.qrc

