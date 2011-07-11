find_package (Qt4 4.4 REQUIRED)

include_directories(../include ${AZURE2_SOURCE_DIR}/include)

set (AZURESETUP_SOURCES AddECLineDialog.cpp ChooseFileButton.cpp PairsModel.cpp AddLevelDialog.cpp EditChecksDialog.cpp PairsTab.cpp AddPairDialog.cpp EditDirsDialog.cpp RichTextDelegate.cpp AddSegDataDialog.cpp EditOptionsDialog.cpp RunTab.cpp AddSegTestDialog.cpp ExternalCaptureModel.cpp SegmentsDataModel.cpp AddTargetIntDialog.cpp ExternalCaptureTab.cpp SegmentsTab.cpp AZURESetup.cpp LevelsModel.cpp SegmentsTestModel.cpp ChannelDetails.cpp LevelsTab.cpp TargetIntModel.cpp ChannelsModel.cpp main.cpp TargetIntTab.cpp)

set (AZURESETUP_HEADERS ../include/AddECLineDialog.h ../include/AddLevelDialog.h ../include/AddPairDialog.h ../include/AddSegDataDialog.h ../include/AddSegTestDialog.h ../include/AddTargetIntDialog.h ../include/AZUREMainThread.h ../include/AZURESetup.h ../include/ChannelDetails.h ../include/ChannelsModel.h ../include/ChooseFileButton.h ../include/EditChecksDialog.h ../include/EditDirsDialog.h ../include/EditOptionsDialog.h ../include/ElementMap.h ../include/ExternalCaptureModel.h ../include/ExternalCaptureTab.h ../include/FilteredTextEdit.h ../include/LevelsModel.h ../include/LevelsTab.h ../include/PairsModel.h ../include/PairsTab.h ../include/RichTextDelegate.h ../include/RunTab.h ../include/SegmentsDataModel.h ../include/SegmentsTab.h ../include/SegmentsTestModel.h ../include/TargetIntModel.h ../include/TargetIntTab.h ../include/TextEditBuffer.h)

include (${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

QT4_WRAP_CPP(AZURESETUP_HEADERS_MOC ${AZURESETUP_HEADERS})

add_library(AZURESetup ${AZURESETUP_SOURCES} ${AZURESETUP_HEADERS_MOC})

set (TEMP_VAR AZURESetup ${QT_LIBRARIES} gobject-2.0)
set (AZURESETUP_LIBRARIES ${TEMP_VAR} PARENT_SCOPE)
