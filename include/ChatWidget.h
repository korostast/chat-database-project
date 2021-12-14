#ifndef CHATDATABASEPROJECT_CHATWIDGET_H
#define CHATDATABASEPROJECT_CHATWIDGET_H

#include <QWidget>
#include "Defines.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class ChatWidget;
}
QT_END_NAMESPACE

class ChatWidget : public QWidget {
Q_OBJECT

private:
    int id;

    QImage avatar;

    QString name;

    bool group;

    int countMembers;

    ROLE role;

    int friendId;

public:
    int getFriendId() const;

    void setFriendId(int friendId);

public:

    int getId() const;

    const QImage &getAvatar() const;

    const QString &getName() const;

    int getCountMembers() const;

    void setCountMembers(int countMembers);

    ROLE getRole() const;

    void setRole(ROLE role);

    void setId(int id);

    bool isGroup() const;

    void setGroup(bool group);

    void setName(const QString &name);

public:
    Ui::ChatWidget *ui;

    explicit ChatWidget(QWidget *parent = nullptr);

    ~ChatWidget() override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void setAvatar(const QImage &image);
};

#endif //CHATDATABASEPROJECT_CHATWIDGET_H
