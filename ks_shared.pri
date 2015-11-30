INCLUDEPATH += $${PWD}

# ks_shared
PATH_KS_SHARED = $${PWD}/ks/shared
INCLUDEPATH += $${PATH_KS_SHARED}/thirdparty

require_glm {
    include($${PATH_KS_SHARED}/thirdparty/glm/glm.pri)
}

include($${PATH_KS_SHARED}/thirdparty/lodepng/lodepng.pri)

HEADERS += \
    $${PATH_KS_SHARED}/KsProperty.hpp \
    $${PATH_KS_SHARED}/KsDeferredProperty.hpp \
    $${PATH_KS_SHARED}/KsDynamicProperty.hpp \
    $${PATH_KS_SHARED}/KsCallbackTimer.hpp \
    $${PATH_KS_SHARED}/KsRecycleIndexList.hpp \
    $${PATH_KS_SHARED}/KsRangeAllocator.hpp \
    $${PATH_KS_SHARED}/KsGraph.hpp \
    $${PATH_KS_SHARED}/KsThreadPool.hpp \
    $${PATH_KS_SHARED}/KsImageBase.hpp \
    $${PATH_KS_SHARED}/KsImagePNG.hpp \
    $${PATH_KS_SHARED}/KsImage.hpp \
    $${PATH_KS_SHARED}/KsBinPackShelf.hpp


SOURCES += \
    $${PATH_KS_SHARED}/KsDynamicProperty.cpp \
    $${PATH_KS_SHARED}/KsCallbackTimer.cpp \
    $${PATH_KS_SHARED}/KsThreadPool.cpp \
    $${PATH_KS_SHARED}/KsBinPackShelf.cpp
