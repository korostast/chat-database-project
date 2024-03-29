cmake_minimum_required(VERSION 3.20)

#Project username
set(TARGET_NAME chatDatabaseProject)
project(${TARGET_NAME})

#Set the CXX standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Make sure CMake will take care of moc for us
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# .ui directory
set(CMAKE_AUTOUIC_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/ui)

#We use Qt 6
set(QT_MAJOR_VERSION 6)

#Qt6 requires setting the CMAKE_PREFIX_PATH
set(CMAKE_PREFIX_PATH $ENV{Qt${QT_MAJOR_VERSION}_HOME})

#find_package should find everything fine so long as the ENV Variable is set or, for linux systems,
#it is in the default install path.
find_package(Qt${QT_MAJOR_VERSION} COMPONENTS Widgets Sql REQUIRED PATHS $ENV{Qt${QT_MAJOR_VERSION}_HOME})

#Add every file into this list, otherwise moc don't find the classes
add_executable(${TARGET_NAME}
        source/Core/main.cpp
        source/Widgets/MainWindow.cpp
        include/MainWindow.h
        include/ChatWidget.h
        source/Widgets/ChatWidget.cpp
        resources/image.qrc
        source/Widgets/UserInfo.cpp
        include/UserInfo.h include/UserMessageWidget.h
        source/Widgets/UserMessageWidget.cpp
        include/ClickableLabel.h
        source/Widgets/ClickableLabel.cpp
        include/ChatDialog.h
        source/Widgets/ChatDialog.cpp
        include/CustomPlainTextEdit.h
        source/Widgets/CustomPlainTextEdit.cpp
        source/Core/Authentication.cpp
        AvatarEditorWidget/source/AvatarEditor.cpp
        AvatarEditorWidget/include/AvatarEditor.h
        AvatarEditorWidget/source/GraphicsItem.cpp
        AvatarEditorWidget/include/GraphicsItem.h
        AvatarEditorWidget/source/GraphicsView.cpp
        AvatarEditorWidget/include/GraphicsView.h
        AvatarEditorWidget/include/Defines.h source/Core/Chats.cpp
        source/Core/Friends.cpp
        source/Core/Profile.cpp
        include/Defines.h
        source/Core/GroupChatInfo.cpp
        include/SystemMessageWidget.h
        source/Widgets/SystemMessageWidget.cpp
        source/Widgets/ChatMemberWidget.cpp
        include/ChatMemberWidget.h
        include/MessageTextEdit.h
        source/Widgets/MessageTextEdit.cpp
        source/Widgets/FriendWidget.cpp
        include/FriendWidget.h
        include/IncomingRequestWidget.h
        source/Widgets/IncomingRequestWidget.cpp
        include/AbstractFriendWidget.h
        include/OutcomingRequestWidget.h
        source/Widgets/OutcomingRequestWidget.cpp
        include/SearchPeopleWidget.h
        source/Widgets/SearchPeopleWidget.cpp
        include/SqlInterface.h
        source/Core/SqlInterface.cpp
        include/ChatCreationFriendWidget.h
        source/Widgets/ChatCreationFriendWidget.cpp
        source/Core/Settings.cpp
        include/DatabaseChooserDialog.h
        source/Widgets/DatabaseChooserDialog.cpp
        source/Core/DatabaseAdminAccess.cpp
        include/DeleteDatabaseMessagebox.h
        source/Widgets/DeleteDatabaseMessagebox.cpp)

#Copy required libraries into work directory
if (WIN32)
    set(QT_INSTALL_PATH "$ENV{Qt${QT_MAJOR_VERSION}_HOME}")
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/imageformats/qgif.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/imageformats/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/imageformats/qgif.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/imageformats/")
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/imageformats/qjpeg.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/imageformats/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/imageformats/qjpeg.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/imageformats/")
    endif ()
    foreach (QT_LIB Core Gui Widgets Sql)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()

target_include_directories(${TARGET_NAME} PRIVATE ${Qt${QT_MAJOR_VERSION}_INCLUDE_DIRS})
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/AvatarEditorWidget/include)

#Add any extra libs to link also.
target_link_libraries(${TARGET_NAME} PRIVATE Qt${QT_MAJOR_VERSION}::Widgets Qt${QT_MAJOR_VERSION}::Sql)