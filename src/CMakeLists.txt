cmake_minimum_required (VERSION 2.6)

project (DogeMarkets)

set (CMAKE_LIBRARY_PATH "C:\\Program Files (x86)\\Windows Kits\\8.0\\Lib\\win8\\um\\x64")

cmake_policy(SET CMP0020 OLD)

find_package(Qt5 REQUIRED Core Gui Widgets WinExtras Network)

set(QT_UIC_EXECUTABLE ${Qt5Widgets_UIC_EXECUTABLE})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

#qt_wrap_cpp(DogeMarkets DogeMarkets_moc mainwindow.h)
qt5_wrap_ui(DogeMarkets_ui_h mainwindow.ui recentorderswidget.ui)

qt5_add_resources(DogeMarkets_res resources.qrc)

set(DogeMarkets_src
	mainwindow.cpp
	main.cpp
	exchangequery.cpp
	recentorderswidget.cpp
	volumegraphwidget.cpp
	)
set(DogeMarkets_h
	mainwindow.h
	exchangequery.h
	recentorderswidget.h
	volumegraphwidget.h
	)
add_executable(DogeMarkets ${DogeMarkets_res} ${DogeMarkets_ui_h} ${DogeMarkets_moc} ${DogeMarkets_src} ${DogeMarkets_h})
#target_include_directories(DogeMarkets PUBLIC ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5WinExtras_INCLUDE_DIRS})
#target_link_libraries(DogeMarkets ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5WinExtras_LIBRARIES})
qt5_use_modules(DogeMarkets Core Gui Widgets WinExtras Network)

set(INSTALL_DIR ${CMAKE_BINARY_DIR}/../bin)

macro(_installDLL QT_MODULE)
	get_target_property(DLL_LOCATION Qt5::${QT_MODULE} IMPORTED_LOCATION_DEBUG)
	install(FILES ${DLL_LOCATION} DESTINATION ${INSTALL_DIR} CONFIGURATIONS DEBUG)
	get_target_property(DLL_LOCATION Qt5::${QT_MODULE} IMPORTED_LOCATION_RELEASE)
	install(FILES ${DLL_LOCATION} DESTINATION ${INSTALL_DIR} CONFIGURATIONS RELEASE RELWITHDEBINFO)
endmacro()

_installDLL(Core)
_installDLL(Gui)
_installDLL(Widgets)
_installDLL(WinExtras)
_installDLL(Network)

set(QTDIR $ENV{QTDIR})
STRING(REGEX REPLACE "\\\\" "/" QTDIR ${QTDIR}) 

install(FILES "${QTDIR}/bin/icuin51.dll" DESTINATION ${INSTALL_DIR})
install(FILES "${QTDIR}/bin/icuuc51.dll" DESTINATION ${INSTALL_DIR})
install(FILES "${QTDIR}/bin/icudt51.dll" DESTINATION ${INSTALL_DIR})

install(TARGETS DogeMarkets RUNTIME DESTINATION ${INSTALL_DIR})

#add_executable(DogeMarkets mainwindow.cpp)
