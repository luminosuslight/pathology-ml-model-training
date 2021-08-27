

HEADERS += \
    $$PWD/blocks/actions/ClusteringBlock.h \
    $$PWD/blocks/actions/CsvExportBlock.h \
    $$PWD/blocks/actions/DbscanBlock.h \
    $$PWD/blocks/actions/DimensionalityReductionBlock.h \
    $$PWD/blocks/actions/FeatureSetterBlock.h \
    $$PWD/blocks/actions/HdbscanBlock.h \
    $$PWD/blocks/actions/UmapBlock.h \
    $$PWD/blocks/ai/AutoAiSegmentationBlock.h \
    $$PWD/blocks/ai/AutoencoderInferenceBlock.h \
    $$PWD/blocks/ai/AutoencoderTrainingBlock.h \
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
    $$PWD/blocks/basic/ClassesListBlock.h \
    $$PWD/blocks/basic/DataViewBlock.h \
    $$PWD/blocks/basic/TissueImageBlock.h \
    $$PWD/blocks/formats/FolderViewBlock.h \
    $$PWD/blocks/formats/ImageListBlock.h \
    $$PWD/blocks/selection/FeatureSelectionBlock.h \
    $$PWD/blocks/selection/RectangularAreaBlock.h \
    $$PWD/dbscan/dbscan.hpp \
    $$PWD/dbscan/nanoflann.hpp \
    $$PWD/dkm/dkm.h \
    $$PWD/dkm/dkm_parallel.h \
    $$PWD/hdbscan/EuclideanDistance.hpp \
    $$PWD/hdbscan/ManhattanDistance.hpp \
    $$PWD/hdbscan/bitSet.hpp \
    $$PWD/hdbscan/cluster.hpp \
    $$PWD/hdbscan/hdbscanAlgorithm.hpp \
    $$PWD/hdbscan/hdbscanConstraint.hpp \
    $$PWD/hdbscan/hdbscanParameters.hpp \
    $$PWD/hdbscan/hdbscanResult.hpp \
    $$PWD/hdbscan/hdbscanRunner.hpp \
    $$PWD/hdbscan/outlierScore.hpp \
    $$PWD/hdbscan/undirectedGraph.hpp \
    $$PWD/manager/BackendManager.h \
    $$PWD/manager/ViewManager.h \
    $$PWD/multicore_tsne/splittree.h \
    $$PWD/multicore_tsne/tsne.h \
    $$PWD/multicore_tsne/vptree.h

SOURCES += \
    $$PWD/blocks/actions/ClusteringBlock.cpp \
    $$PWD/blocks/actions/CsvExportBlock.cpp \
    $$PWD/blocks/actions/DbscanBlock.cpp \
    $$PWD/blocks/actions/DimensionalityReductionBlock.cpp \
    $$PWD/blocks/actions/FeatureSetterBlock.cpp \
    $$PWD/blocks/actions/HdbscanBlock.cpp \
    $$PWD/blocks/actions/UmapBlock.cpp \
    $$PWD/blocks/ai/AutoAiSegmentationBlock.cpp \
    $$PWD/blocks/ai/AutoencoderInferenceBlock.cpp \
    $$PWD/blocks/ai/AutoencoderTrainingBlock.cpp \
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
    $$PWD/blocks/basic/ClassesListBlock.cpp \
    $$PWD/blocks/basic/DataViewBlock.cpp \
    $$PWD/blocks/basic/TissueImageBlock.cpp \
    $$PWD/blocks/formats/FolderViewBlock.cpp \
    $$PWD/blocks/formats/ImageListBlock.cpp \
    $$PWD/blocks/selection/FeatureSelectionBlock.cpp \
    $$PWD/blocks/selection/RectangularAreaBlock.cpp \
    $$PWD/dbscan/dbscan.cpp \
    $$PWD/hdbscan/EuclideanDistance.cpp \
    $$PWD/hdbscan/ManhattanDistance.cpp \
    $$PWD/hdbscan/bitSet.cpp \
    $$PWD/hdbscan/cluster.cpp \
    $$PWD/hdbscan/hdbscanAlgorithm.cpp \
    $$PWD/hdbscan/hdbscanConstraint.cpp \
    $$PWD/hdbscan/hdbscanResult.cpp \
    $$PWD/hdbscan/hdbscanRunner.cpp \
    $$PWD/hdbscan/outlierScore.cpp \
    $$PWD/hdbscan/undirectedGraph.cpp \
    $$PWD/manager/BackendManager.cpp \
    $$PWD/manager/ViewManager.cpp \
    $$PWD/multicore_tsne/splittree.cpp \
    $$PWD/multicore_tsne/tsne.cpp

RESOURCES += \
    $$PWD/microscopy.qrc
