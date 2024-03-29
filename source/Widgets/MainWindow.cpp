#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "FriendWidget.h"
#include "IncomingRequestWidget.h"
#include "SqlInterface.h"
#include "OutcomingRequestWidget.h"
#include <QScrollBar>

STATE MainWindow::currentState = AUTHORIZATION;

ChatWidget *MainWindow::currentChat = nullptr;

UserInfo *MainWindow::currentUser;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    // Open database connection
    dbConnect(DEFAULT_DATABASE);

    // Initialize Main Window variables
    avatarEditor = new AvatarEditor(this);
    currentState = AUTHORIZATION;
    (currentChat = new ChatWidget(this))->hide(); // TODO parent?
    chatDialog = new ChatDialog(this);
    databaseDialog = new DatabaseChooserDialog(this);

    // Event filter for chat info dialog
    chatDialog->installEventFilter(this);

    // Setting up interface and show default page (authorization)
    ui->setupUi(this);

    ui->app_stacked_widget->setCurrentIndex(AUTHENTICATION_PAGE);
    ui->authentification_stacked_widget->setCurrentIndex(AUTHORIZATION_PAGE);
    ui->main_stacked_widget->setCurrentIndex(PERSONAL_CHAT_LIST_PAGE);

    // Establish default size of text edit in chat and speed of vertical scrolling
    int defaultRowNumber = 1;
    ui->message_text_edit->setFixedHeight(getNewEditTextHeight(ui->message_text_edit->document()->size(),
                                                               ui->message_text_edit, defaultRowNumber));
    ui->messageList->verticalScrollBar()->setSingleStep(10);

    // If we hide the widget, layout won't be resized. Thus, other widgets will keep their positions
    QSizePolicy sizePolicy = ui->additional_info_list->sizePolicy();
    sizePolicy.setRetainSizeWhenHidden(true);
    ui->additional_info_list->setSizePolicy(sizePolicy);
    ui->additional_info_list->hide(); // Don't delete this widget. I love it

    QSizePolicy retain = ui->auth_register_error_label->sizePolicy();
    retain.setRetainSizeWhenHidden(true);
    ui->auth_register_error_label->setSizePolicy(retain);
    ui->auth_register_error_label->hide();

    // Put DEFAULT_DATABASE name into labels
    ui->current_database_name_label->setText(QString("Текущая БД: %1").arg(DEFAULT_DATABASE));
    ui->current_database_name_label->setToolTip(DEFAULT_DATABASE);

    // Connect all main window widgets to the functions
    // Authentication
    connect(ui->auth_password_edit, SIGNAL(returnPressed()), this, SLOT(sign_in_button_released()));
    connect(ui->auth_email_edit, SIGNAL(returnPressed()), this, SLOT(sign_in_button_released()));
    connect(ui->register_username_edit, SIGNAL(returnPressed()), this, SLOT(register_button_released()));
    connect(ui->register_password_edit, SIGNAL(returnPressed()), this, SLOT(register_button_released()));
    connect(ui->register_email_edit, SIGNAL(returnPressed()), this, SLOT(register_button_released()));
    connect(ui->sign_in_button, SIGNAL(released()), this, SLOT(sign_in_button_released()));
    connect(ui->register_button, SIGNAL(released()), this, SLOT(register_button_released()));
    connect(ui->switch_auth_button, SIGNAL(released()), this, SLOT(switch_auth_button_released()));
    connect(ui->switch_register_button, SIGNAL(released()), this, SLOT(switch_register_button_released()));
    connect(ui->authentification_admin_button, SIGNAL(released()), this, SLOT(open_admin_dialog()));
    connect(ui->authentification_choose_database_button, SIGNAL(released()), this, SLOT(open_choose_database_dialog()));

    // App
    connect(ui->chats_button, SIGNAL(released()), this, SLOT(chats_button_released()));
    connect(ui->profile_button, SIGNAL(released()), this, SLOT(profile_button_released()));
    connect(ui->friends_button, SIGNAL(released()), this, SLOT(friends_button_released()));
    connect(ui->settings_button, SIGNAL(released()), this, SLOT(settings_button_released()));
    connect(ui->chat_creation_button, SIGNAL(released()), this, SLOT(chat_creation_open_ui()));
    connect(ui->chat_creation_name_edit, SIGNAL(returnPressed()), this, SLOT(group_chat_create()));
    connect(ui->chat_creation_create_button, SIGNAL(released()), this, SLOT(group_chat_create()));
    connect(ui->refresh_label, SIGNAL(released()), this, SLOT(repeat_sql_request()));
    connect(ui->unused_switch_personal_chats, &QPushButton::released, this, [this]() {
        ui->unused_switch_personal_chats->setChecked(true);
    });
    connect(ui->unused_switch_group_chats, &QPushButton::released, this, [this]() {
        ui->unused_switch_group_chats->setChecked(true);
    });
    connect(ui->switch_group_chats, &QPushButton::released, this, [this]() {
        ui->main_stacked_widget->setCurrentIndex(GROUP_CHAT_LIST_PAGE);
    });
    connect(ui->switch_personal_chats, &QPushButton::released, this, [this]() {
        ui->main_stacked_widget->setCurrentIndex(PERSONAL_CHAT_LIST_PAGE);
    });
    connect(ui->chat_creation_avatar, &ClickableLabel::released, this, [this]() {
        chatDialog->openFileChooser();
    });

    // Chat interface
    connect(ui->chat_name_label, SIGNAL(released()), this, SLOT(chat_name_label_released()));
    connect(ui->chat_back_button, SIGNAL(released()), this, SLOT(chats_button_released()));
    connect(ui->send_message_button, SIGNAL(released()), this, SLOT(sendMessage()));
    connect(ui->search_people_button, SIGNAL(released()), this, SLOT(search_people()));
    connect(ui->search_people_line, SIGNAL(returnPressed()), this, SLOT(search_people()));
    connect(ui->message_text_edit, SIGNAL(returnKeyPressedEvent()), this, SLOT(sendMessage()));
    connect(ui->chat_search_label, &ClickableLabel::released, this, [this]() {
        ui->chat_bar_search_edit->clear();
        ui->chat_bar_search_edit->setFocus();
        ui->chat_bar_stacked_widget->setCurrentIndex(1);
    });
    connect(ui->chat_bar_search_cancel, &QPushButton::released, this, [this]() {
        ui->chat_bar_stacked_widget->setCurrentIndex(0);
    });
    connect(ui->chat_bar_search_button, SIGNAL(released()), this, SLOT(loadSearchInterface()));
    connect(ui->chat_bar_search_edit, &QLineEdit::returnPressed, this, [this]() {
        if (ui->chat_bar_search_edit->text().isEmpty())
            return;
        loadSearchInterface();
    });
    connect(ui->message_text_edit->document()->documentLayout(), SIGNAL(documentSizeChanged(QSizeF)),
            this, SLOT(messageTextChanged(QSizeF)));

    // Search messages
    connect(ui->messages_search_button, SIGNAL(released()), this, SLOT(searchMessages()));
    connect(ui->messages_search_edit, SIGNAL(returnPressed()), this, SLOT(searchMessages()));
    connect(ui->messages_search_remove_messages, SIGNAL(released()), this, SLOT(deleteMessagesInSearch()));
    connect(ui->messages_search_cancel, &QPushButton::released, this, [this]() {
        ui->chat_bar_stacked_widget->setCurrentIndex(0);
        ui->main_stacked_widget->setCurrentIndex(MESSAGES_PAGE);
        currentState = MESSAGES;
    });

    // Friends
    connect(ui->switch_actual_friends, &QPushButton::released, this, [this]() {
        switch_friends_page(ACTUAL_FRIENDS_PAGE);
    });
    connect(ui->switch_incoming_requests, &QPushButton::released, this, [this]() {
        switch_friends_page(INCOMING_REQUESTS_PAGE);
    });
    connect(ui->switch_outcoming_requests, &QPushButton::released, this, [this]() {
        switch_friends_page(OUTCOMING_REQUESTS_PAGE);
    });
    connect(ui->switch_search_people, &QPushButton::released, this, [this]() {
        switch_friends_page(SEARCH_PEOPLE_PAGE);
    });

    // Settings
    connect(ui->settings_save_button, SIGNAL(released()), this, SLOT(settings_save_button_released()));
    connect(ui->profile_avatar, &ClickableLabel::released, this, [this]() {
        chatDialog->openFileChooser();
    });
    connect(ui->settings_change_password_button, SIGNAL(released()), this, SLOT(change_password_return_pressed()));
    connect(ui->settings_change_username_button, SIGNAL(released()), this, SLOT(change_username()));
    connect(ui->settings_old_password_edit, SIGNAL(returnPressed()), this, SLOT(change_password_return_pressed()));
    connect(ui->settings_new_password_edit, SIGNAL(returnPressed()), this, SLOT(change_password_return_pressed()));
    connect(ui->settings_repeat_password_edit, SIGNAL(returnPressed()), this, SLOT(change_password_return_pressed()));
    connect(ui->settings_username_edit, SIGNAL(returnPressed()), this, SLOT(change_username()));
    ui->settings_password_error_label->hide();
    ui->settings_username_error_label->hide();
}

MainWindow::~MainWindow() {
    delete ui;
    delete currentUser;

    // Close database connection
    dbClose();
}

// This filter monitor event of focus switching when dealing with chat info dialog. Ask Korostast about this mega stupid feature
bool MainWindow::eventFilter(QObject *object, QEvent *event) {
    if (event->type() == QEvent::WindowDeactivate && isActiveWindow() && chatDialog == object) {
        qDebug() << "Focus lost!";
        chatDialog->hide();

        return true;
    }
    return false;
}

// Debug functions behavior
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
        case QtDebugMsg:
            fprintf(stderr, "DEBUG: %s (in func '%s')\n\n", localMsg.constData(), function);
            break;
        case QtInfoMsg:
            fprintf(stderr, "INFO: %s\n\n", localMsg.constData());
            break;
        case QtWarningMsg:
            fprintf(stderr, "WARNING: %s (%s:%u, %s)\n\n", localMsg.constData(), file, context.line, function);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "CRITICAL: %s (%s:%u, %s)\n\n", localMsg.constData(), file, context.line, function);
            break;
        case QtFatalMsg:
            fprintf(stderr, "FATAL: %s (%s:%u, %s)\n\n", localMsg.constData(), file, context.line, function);
            break;
    }
}

void MainWindow::repeat_sql_request() {
    switch (currentState) {
        case CHATS: {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            qInfo() << "Refreshing chats..";

            int curPage = ui->main_stacked_widget->currentIndex();
            chats_button_released();
            ui->main_stacked_widget->setCurrentIndex(curPage);

            QApplication::restoreOverrideCursor();
            break;
        }
        case MESSAGES: {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            qInfo() << "Refreshing messages..";

            openChat();

            QApplication::restoreOverrideCursor();
            break;
        }
        case PROFILE: {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            qInfo() << "Refreshing profile..";

            UserInfo user = sqlLoadProfile(ui->profile_avatar->getPositiveIntData());
            showProfile(&user);

            QApplication::restoreOverrideCursor();
            break;
        }
        case MY_PROFILE: {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            qInfo() << "Refreshing profile..";

            UserInfo user = sqlLoadProfile(currentUser->getID());
            showProfile(&user);

            QApplication::restoreOverrideCursor();
            break;
        }
        case FRIENDS: {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            qInfo() << "Refreshing friends..";

            int currentId = currentUser->getID();
            QList<UserInfo> friends = sqlLoadFriends(currentId);
            ui->actual_friends_list->clear();
            ui->switch_actual_friends->setText(QString("Друзья (%1)").arg(friends.count()));
            for (const auto &fr: friends)
                addToList<FriendWidget>(fr.getID(), fr.getUsername(), fr.getAvatar(), ui->actual_friends_list);

            QApplication::restoreOverrideCursor();
            break;
        }
        case INCOMING_REQUESTS: {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            qInfo() << "Refreshing incoming requests..";

            int currentId = currentUser->getID();
            QList<UserInfo> incomingRequests = sqlLoadIncomingRequests(currentId);
            ui->incoming_requests_list->clear();
            ui->switch_incoming_requests->setText(QString("Входящие (%1)").arg(incomingRequests.count()));
            for (const auto &in: incomingRequests)
                addToList<IncomingRequestWidget>(in.getID(), in.getUsername(), in.getAvatar(),
                                                 ui->incoming_requests_list);

            QApplication::restoreOverrideCursor();
            break;
        }
        case OUTCOMING_REQUESTS: {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            qInfo() << "Refreshing outcoming requests..";

            int currentId = currentUser->getID();
            QList<UserInfo> outcomingRequests = sqlLoadOutgoingRequests(currentId);
            ui->outcoming_requests_list->clear();
            ui->switch_outcoming_requests->setText(QString("Исходящие (%1)").arg(outcomingRequests.count()));
            for (const auto &out: outcomingRequests)
                addToList<OutcomingRequestWidget>(out.getID(), out.getUsername(), out.getAvatar(),
                                                  ui->outcoming_requests_list);

            QApplication::restoreOverrideCursor();
            break;
        }
        case SEARCH_PEOPLE: {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            qInfo() << "Refreshing outcoming requests..";

            friends_button_released();
            switch_friends_page(SEARCH_PEOPLE_PAGE);

            QApplication::restoreOverrideCursor();
            break;
        }
        case SETTINGS: {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            qInfo() << "Refreshing settings..";

            settings_button_released();

            QApplication::restoreOverrideCursor();
            break;
        }
        default:
            qWarning() << "There is no sql request for current state "
                          "(nothing to update or you should do update by pressing specific button)";
    }
}