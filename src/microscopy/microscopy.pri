

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = ../core/ui

HEADERS += \
    $$PWD/blocks/actions/ClusteringBlock.h \
    $$PWD/blocks/actions/CsvExportBlock.h \
    $$PWD/blocks/actions/DimensionalityReductionBlock.h \
    $$PWD/blocks/actions/FeatureSetterBlock.h \
    $$PWD/blocks/ai/AutoAiSegmentationBlock.h \
    $$PWD/blocks/ai/CellAreaAverageBlock.h \
    $$PWD/blocks/ai/CellDatabaseComparison.h \
    $$PWD/blocks/ai/CellRendererBlock.h \
    $$PWD/blocks/ai/CnnInferenceBlock.h \
    $$PWD/blocks/ai/CnnModelBlock.h \
    $$PWD/blocks/ai/CnnTrainingBlock.h \
    $$PWD/blocks/ai/FindCentersBlock.h \
    $$PWD/blocks/ai/MarkerBasedRegionGrowBlock.h \
    $$PWD/blocks/ai/RandomCellGeneratorBlock.h \
    $$PWD/blocks/ai/TrainingDataBlock.h \
    $$PWD/blocks/ai/TrainingDataPreprocessingBlock.h \
    $$PWD/blocks/basic/CellDatabaseBlock.h \
    $$PWD/blocks/basic/CellVisualizationBlock.h \
    $$PWD/blocks/basic/DataViewBlock.h \
    $$PWD/blocks/basic/TissueImageBlock.h \
    $$PWD/blocks/formats/FolderViewBlock.h \
    $$PWD/blocks/selection/FeatureSelectionBlock.h \
    $$PWD/blocks/selection/RectangularAreaBlock.h \
    $$PWD/manager/BackendManager.h \
    $$PWD/manager/ViewManager.h

SOURCES += \
    $$PWD/blocks/actions/ClusteringBlock.cpp \
    $$PWD/blocks/actions/CsvExportBlock.cpp \
    $$PWD/blocks/actions/DimensionalityReductionBlock.cpp \
    $$PWD/blocks/actions/FeatureSetterBlock.cpp \
    $$PWD/blocks/ai/AutoAiSegmentationBlock.cpp \
    $$PWD/blocks/ai/CellAreaAverageBlock.cpp \
    $$PWD/blocks/ai/CellDatabaseComparison.cpp \
    $$PWD/blocks/ai/CellRendererBlock.cpp \
    $$PWD/blocks/ai/CnnInferenceBlock.cpp \
    $$PWD/blocks/ai/CnnModelBlock.cpp \
    $$PWD/blocks/ai/CnnTrainingBlock.cpp \
    $$PWD/blocks/ai/FindCentersBlock.cpp \
    $$PWD/blocks/ai/MarkerBasedRegionGrowBlock.cpp \
    $$PWD/blocks/ai/RandomCellGeneratorBlock.cpp \
    $$PWD/blocks/ai/TrainingDataBlock.cpp \
    $$PWD/blocks/ai/TrainingDataPreprocessingBlock.cpp \
    $$PWD/blocks/basic/CellDatabaseBlock.cpp \
    $$PWD/blocks/basic/CellVisualizationBlock.cpp \
    $$PWD/blocks/basic/DataViewBlock.cpp \
    $$PWD/blocks/basic/TissueImageBlock.cpp \
    $$PWD/blocks/formats/FolderViewBlock.cpp \
    $$PWD/blocks/selection/FeatureSelectionBlock.cpp \
    $$PWD/blocks/selection/RectangularAreaBlock.cpp \
    $$PWD/manager/BackendManager.cpp \
    $$PWD/manager/ViewManager.cpp

RESOURCES += \
    $$PWD/microscopy.qrc
