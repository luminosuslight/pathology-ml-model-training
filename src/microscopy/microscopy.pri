

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = ../core/ui

HEADERS += \
    $$PWD/blocks/ArtificialTrainingDataBlock.h \
    $$PWD/blocks/NucleiDataBlock.h \
    $$PWD/blocks/NucleiPlotBlock.h \
    $$PWD/blocks/NucleiVisualizationBlock.h \
    $$PWD/blocks/TissueChannelBlock.h \
    $$PWD/blocks/TissueViewerBlock.h \
    $$PWD/blocks/TrainingDataOldBlock.h \
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
    $$PWD/blocks/basic/CellFeatureVisualizationBlock.h \
    $$PWD/blocks/basic/CellVisualizationBlock.h \
    $$PWD/blocks/basic/TissueImageBlock.h \
    $$PWD/blocks/basic/TissueViewBlock.h \
    $$PWD/blocks/selection/AreaSelectionFreeformBlock.h \
    $$PWD/blocks/selection/AreaSelectionRectangularBlock.h \
    $$PWD/blocks/selection/FeatureSelectionBlock.h \
    $$PWD/blocks/selection/PlotSelectionBlock.h \
    $$PWD/manager/ViewManager.h

SOURCES += \
    $$PWD/blocks/ArtificialTrainingDataBlock.cpp \
    $$PWD/blocks/NucleiDataBlock.cpp \
    $$PWD/blocks/NucleiPlotBlock.cpp \
    $$PWD/blocks/NucleiVisualizationBlock.cpp \
    $$PWD/blocks/TissueChannelBlock.cpp \
    $$PWD/blocks/TissueViewerBlock.cpp \
    $$PWD/blocks/TrainingDataOldBlock.cpp \
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
    $$PWD/blocks/basic/CellFeatureVisualizationBlock.cpp \
    $$PWD/blocks/basic/CellVisualizationBlock.cpp \
    $$PWD/blocks/basic/TissueImageBlock.cpp \
    $$PWD/blocks/basic/TissueViewBlock.cpp \
    $$PWD/blocks/selection/AreaSelectionFreeformBlock.cpp \
    $$PWD/blocks/selection/AreaSelectionRectangularBlock.cpp \
    $$PWD/blocks/selection/FeatureSelectionBlock.cpp \
    $$PWD/blocks/selection/PlotSelectionBlock.cpp \
    $$PWD/manager/ViewManager.cpp

RESOURCES += \
    $$PWD/microscopy.qrc
