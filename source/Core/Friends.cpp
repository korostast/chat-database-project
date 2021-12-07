#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "FriendWidget.h"
#include "IncomingRequestWidget.h"
#include "OutcomingRequestWidget.h"
#include "SearchPeopleWidget.h"

template<typename T>
void MainWindow::addToList(int friendId, const QString &username, const QImage &avatar, QListWidget *list) {
    auto *item = new QListWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);

    auto *widget = new T(this);
    widget->setFriendId(friendId);
    widget->setUsername(username);

    // TODO avatars
    widget->setAvatar(avatar);

    item->setSizeHint(widget->sizeHint());

    list->insertItem(0, item);
    list->setItemWidget(item, widget);
}

template<typename T>
void MainWindow::removeFromList(int requestId, QListWidget *list) {
    for (int i = 0; i < list->count(); ++i) {
        auto *friendWidget = qobject_cast<T *>(list->itemWidget(list->item(i)));
        if (friendWidget->getFriendId() == requestId) {
            auto *item = list->takeItem(i);
            delete item;
            return;
        }
    }
}

void MainWindow::addPersonInSearch(int personId, const QString &username, const QImage &avatar) {
    auto *item = new QListWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);

    auto *widget = new SearchPeopleWidget(this);
    widget->setFriendId(personId);
    widget->setUsername(username);

    // TODO avatars
    widget->setAvatar(avatar);

    // Check if this person is already in the list of outcoming_requests_list and friends list and incoming_requests_list ?

    item->setSizeHint(widget->sizeHint());

    ui->search_people_list->insertItem(0, item);
    ui->search_people_list->setItemWidget(item, widget);
}

void MainWindow::friends_button_released() const {
    ui->main_stacked_widget->setCurrentIndex(FRIENDS_PAGE);
    ui->friends_stacked_widget->setCurrentIndex(ACTUAL_FRIENDS_PAGE);
    ui->switch_actual_friends->setChecked(true);
    currentChat = nullptr;
    currentState = FRIENDS;
}

void MainWindow::switch_friends_page(int page) const {
    ui->friends_stacked_widget->setCurrentIndex(page);

    ui->switch_actual_friends->setChecked(page == ACTUAL_FRIENDS_PAGE);
    ui->switch_incoming_requests->setChecked(page == INCOMING_REQUESTS_PAGE);
    ui->switch_outcoming_requests->setChecked(page == OUTCOMING_REQUESTS_PAGE);
    ui->switch_search_people->setChecked(page == SEARCH_PEOPLE_PAGE);

    currentState = static_cast<STATE>(FRIENDS + page);
}

void FriendWidget::friend_remove_button_released() {
    // TODO database
    auto *mainWindow = qobject_cast<MainWindow *>(window());
    MainWindow::removeFromList<FriendWidget>(getFriendId(), mainWindow->ui->actual_friends_list);
}

void IncomingRequestWidget::accept_request_button_released() {
    // TODO database
    auto *mainWindow = qobject_cast<MainWindow *>(window());
    MainWindow::removeFromList<IncomingRequestWidget>(getFriendId(), mainWindow->ui->incoming_requests_list);
}

void IncomingRequestWidget::decline_request_button_released() {
    // TODO database
    auto *mainWindow = qobject_cast<MainWindow *>(window());
    MainWindow::removeFromList<IncomingRequestWidget>(getFriendId(), mainWindow->ui->incoming_requests_list);
}

void OutcomingRequestWidget::remove_request_button_released() {
    // TODO database
    auto *mainWindow = qobject_cast<MainWindow *>(window());
    MainWindow::removeFromList<OutcomingRequestWidget>(getFriendId(), mainWindow->ui->outcoming_requests_list);
}

void MainWindow::search_people() {
    QString input(ui->search_people_line->text());

    // TODO database request
    QList<UserInfo> users;

    // TEST
    users.push_back(UserInfo(0, "Lalala", QImage(":chatDefaultImage")));
    users.push_back(UserInfo(1, "Another one", QImage(":chatDefaultImage")));
    users.push_back(UserInfo(2, "Second", QImage(":chatDefaultImage")));
    users.push_back(UserInfo(4, "Kriper2003", QImage(":chatDefaultImage")));

    for (const auto &user : users) {
        addPersonInSearch(user.getId(), user.getUsername(), user.getAvatar());
    }
}

// TODO remove it
void MainWindow::tests() {
    // Test
    for (int i = 0; i < 5; ++i) {
        addChat(i, QString::fromStdString(std::to_string(i)), QImage(":chatDefaultImage"), true, 0, PARTICIPANT);
    }
    addChat(5, "5", QImage(":chatDefaultImage"), true, 0, VIEWER);
    addChat(6, "6", QImage(":chatDefaultImage"), true, 0, ADMIN);
    //updateChat(2, nullptr, QImage(":chatDefaultImage"), PARTICIPANT);

    // Test
    addMessage(0, 0, "Korostast", "2021-03-31 22:10", QImage(":chatDefaultImage"), "Hello world!", USER_MESSAGE);
    addMessage(0, 1, "Korostast", "2021-03-31 22:11", QImage(":chatDefaultImage"), "Hello world!", USER_MESSAGE);
    QString test("Он белый\n"
                 "Пушистый\n"
                 "Мягкий\n"
                 "Падает красиво\n"
                 "Особенно когда в темноте, медленно-медленно и хлопьями, в ресницах застревающими\n"
                 "Он безумно красивый\n"
                 "И из него можно сделать снежок и запустить в какого-нибудь очень хорошего человека");
    addMessage(0, 2, "Korostast", "2021-03-31 23:59", QImage(":chatDefaultImage"), test,
               USER_MESSAGE);

    QString test2("Он белый"
                  "Пушистый"
                  "Мягкий"
                  "Падает красиво"
                  "Особенно когда в темноте, медленно-медленно и хлопьями, в ресницах застревающими"
                  "Он безумно красивый"
                  "И из него можно сделать снежок и запустить в какого-нибудь очень хорошего человека");
    addMessage(0, 3, "Korostast", "2021-03-31 23:59", QImage(":chatDefaultImage"), test2,
               USER_MESSAGE);

    QString test3(
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaa");
    addMessage(0, 3, "Korostast", "2021-03-31 23:59", QImage(":chatDefaultImage"), test3,
               USER_MESSAGE);

    addToList<FriendWidget>(0, "Lalala", QImage(":chatDefaultImage"), ui->actual_friends_list);
    addToList<FriendWidget>(1, "Another one", QImage(":chatDefaultImage"), ui->actual_friends_list);
    addToList<FriendWidget>(2, "Second", QImage(":chatDefaultImage"), ui->actual_friends_list);

    addToList<IncomingRequestWidget>(3, "This_is_request", QImage(":chatDefaultImage"), ui->incoming_requests_list);
    addToList<IncomingRequestWidget>(4, "Kriper2003", QImage(":chatDefaultImage"), ui->incoming_requests_list);

    addToList<OutcomingRequestWidget>(5, "Masha", QImage(":chatDefaultImage"), ui->outcoming_requests_list);
    addToList<OutcomingRequestWidget>(6, "Java", QImage(":chatDefaultImage"), ui->outcoming_requests_list);

    /*addPersonInSearch(0, "Lalala", QImage(":chatDefaultImage"));
    addPersonInSearch(1, "Another one", QImage(":chatDefaultImage"));
    addPersonInSearch(2, "Second", QImage(":chatDefaultImage"));
    addPersonInSearch(3, "This_is_request", QImage(":chatDefaultImage"));
    addPersonInSearch(4, "Kriper2003", QImage(":chatDefaultImage"));*/
}