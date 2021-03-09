
set(SOURCES
    main.cpp
    initializer.cpp
    Front/mainwindow.cpp
    Front/MainElements/infogb.cpp
    Front/Dictionary/russiandictionary.cpp
    Front/GeneralTextEdit/undoredotext.cpp
    Front/GeneralTextEdit/textnewelement.cpp
    Front/GeneralTextEdit/notemark.cpp
    Front/GeneralTextEdit/gentextedit.cpp
    Front/GeneralTextEdit/gentextedit-write-read.cpp
    Front/GeneralTextEdit/gentextedit-keys-realization.cpp
    Front/GeneralTextEdit/gentextedit-get-set.cpp
    Front/GeneralTextEdit/gentextedit-details.cpp
    Front/GeneralTextEdit/TimeTable/timetabletextedit.cpp
    Front/GeneralTextEdit/TimeTable/timetable-write-read.cpp
    Front/MainElements/EmojiHub/emojibutton.cpp
    Front/MainElements/EmojiHub/emojihub.cpp
    Front/MainElements/addbreekgb.cpp
    Front/MainElements/addbuttonhover.cpp
    Front/MainElements/addtimetableelementgb.cpp
    Front/MainElements/breek.cpp
    Front/MainElements/calendarweek.cpp
    Front/MainElements/daywidget.cpp
    Front/MainElements/deletebreekszonebutton.cpp
    Front/MainElements/deletetimetableelementbutton.cpp
    Front/MainElements/demobreek.cpp
    Front/MainElements/descriptionzonedaybutton.cpp
    Front/MainElements/elementtemplate.cpp
    Front/MainElements/imagehover.cpp
    Front/MainElements/timeedit.cpp
    Front/Mainwindow-zones/mainwidnow-timetable.cpp
    Front/Mainwindow-zones/mainwindow-addbreek.cpp
    Front/Mainwindow-zones/mainwindow-addelement.cpp
    Front/Mainwindow-zones/mainwindow-addtimetableelement.cpp
    Front/Mainwindow-zones/mainwindow-breekszone.cpp
    Front/Mainwindow-zones/mainwindow-effects.cpp
    Front/Mainwindow-zones/mainwindow-image.cpp
    Front/Mainwindow-zones/mainwindow-infobuttons.cpp
    Front/Mainwindow-zones/mainwindow-note.cpp
    Front/Mainwindow-zones/mainwindow-workzone.cpp
    Back/FileSystem/databaseIO.cpp
    Back/FileSystem/filesystem.cpp
    Back/secret-data.cpp
    Back/server-connection.cpp
    Back/utils/utils.cpp
)

set(HEADERS
    Front/mainwindow.h
    Front/MainElements/infogb.h
    Front/datastructures.h
    Front/effects.h
    Front/Dictionary/russiandictionary.h
    Front/GeneralTextEdit/undoredotext.h
    Front/GeneralTextEdit/textnewelement.h
    Front/GeneralTextEdit/notemark.h
    Front/GeneralTextEdit/gentextedit.h
    Front/GeneralTextEdit/TimeTable/timetabletextedit.h
    Front/MainElements/EmojiHub/emojibutton.h
    Front/MainElements/EmojiHub/emojihub.h
    Front/MainElements/addbreekgb.h
    Front/MainElements/addbuttonhover.h
    Front/MainElements/addtimetableelementgb.h
    Front/MainElements/breek.h
    Front/MainElements/calendarweek.h
    Front/MainElements/daywidget.h
    Front/MainElements/deletebreekszonebutton.h
    Front/MainElements/deletetimetableelementbutton.h
    Front/MainElements/demobreek.h
    Front/MainElements/descriptionzonedaybutton.h
    Front/MainElements/elementtemplate.h
    Front/MainElements/imagehover.h
    Front/MainElements/timeedit.h
    Back/filesystem.h
    Back/secret-data.h
    Back/server-connection.h
    Back/utils/utils.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_resources(SOURCES resources.qrc)
    qt_wrap_ui(SOURCES Front/mainwindow.ui)
    qt_add_executable(${CMAKE_PROJECT_NAME}
        ${SOURCES}
    )
else()
    qt5_add_resources(SOURCES resources.qrc)
    qt5_wrap_ui(SOURCES Front/mainwindow.ui)
    add_executable(${CMAKE_PROJECT_NAME}
        ${SOURCES}
    )
endif()

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::QuickWidgets
)

# Only for mainwindow.ui(@todo: Remove me)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    Front
    Front/MainElements
    Front/MainElements/EmojiHub
    Front/GeneralTextEdit
    Front/GeneralTextEdit/TimeTable
)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
    -Wall
    -Wpedantic
    -Wextra
#    -fanalyzer
)

