#include <QFileDialog>
#include "../include/AvatarEditor.h"
#include "ui_avatareditor.h"
#include "MainWindow.h"
#include "SqlInterface.h"

AvatarEditor::AvatarEditor(QWidget *parent)
        : QDialog(parent), ui(new Ui::AvatarEditor), scaleFactor(0), image(new QImage),
          ellipseItem(new GraphicsEllipseItem) {

    ui->setupUi(this);
    ui->graphicsView->setScene(new QGraphicsScene(this));

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(saveImage()));
    connect(ui->avatar_editor_choose_file_button, SIGNAL(clicked()), this, SLOT(editorFileChooserOpen()));
}

AvatarEditor::~AvatarEditor() {
    delete ui;
}

// Set graphics view scene: upload an image and selecting ellipse to the scene
void AvatarEditor::loadImageIntoScene(const QString &pathToImage) {
    image->load(pathToImage);

    // Calculate scale factor
    {
        qreal viewWidth = ui->graphicsView->width();
        qreal viewHeight = ui->graphicsView->height();
        qreal imageWidth = image->width();
        if (viewWidth < imageWidth) {
            scaleFactor = viewWidth / imageWidth;
        }
        qreal newHeight = image->height() * scaleFactor;
        if (viewHeight < newHeight) {
            scaleFactor *= viewHeight / newHeight;
        }
        qDebug() << "Scale factor =" << scaleFactor;
    }

    // Convert image into GraphicsItem and upload it to the scene
    {
        auto *imageItem = new GraphicsItem(QPixmap::fromImage(*image), nullptr);
        imageItem->setScale(scaleFactor);
        // Accurate smooth scaling, high quality
        imageItem->setTransformationMode(Qt::SmoothTransformation);
        ui->graphicsView->scene()->addItem(imageItem);
    }

    // Draw an ellipse and display it
    {
        // TODO leak memory here probably
        ellipseItem = new GraphicsEllipseItem(QRect(0, 0, RESULT_IMAGE_SIZE, RESULT_IMAGE_SIZE), nullptr,
                                              image->width() * scaleFactor - RESULT_IMAGE_SIZE,
                                              image->height() * scaleFactor - RESULT_IMAGE_SIZE);
        ellipseItem->setAcceptHoverEvents(true);
        ellipseItem->setFlag(QGraphicsItem::ItemIsMovable);

        // Construct a black semi-transparent circle
        QPen blackPen(Qt::black);
        blackPen.setWidth(6);
        ellipseItem->setPen(blackPen);
        ellipseItem->setOpacity(0.5);
        ui->graphicsView->scene()->addItem(ellipseItem);
        ellipseItem->renderCroppedImage();
    }
}

// Round the picture and scale it to size 'size'
QPixmap AvatarEditor::getCircularPixmap(const QImage &targetImage, const int size) {
    QPixmap currentPixmap(QPixmap::fromImage(targetImage));
    currentPixmap = currentPixmap.scaled(size, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QPixmap resultPixmap(QSize(size, size));
    resultPixmap.fill(Qt::transparent);

    QPainter painter(&resultPixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath p = QPainterPath();
    p.addRoundedRect(0, 0, size, size, size / 2.0, size / 2.0);
    painter.setClipPath(p);
    painter.drawPixmap(0, 0, currentPixmap);
    return resultPixmap;
}

// Place image into preview label
void AvatarEditor::updatePreview() const {
    qreal reversedScaleFactor = 1 / scaleFactor;
    QRect rect((int) (ellipseItem->x() * reversedScaleFactor), (int) (ellipseItem->y() * reversedScaleFactor),
               (int) (ellipseItem->rect().width() * reversedScaleFactor * ellipseItem->scale()),
               (int) (ellipseItem->rect().height() * reversedScaleFactor * ellipseItem->scale()));

    QImage result = image->copy(rect).scaled(RESULT_IMAGE_SIZE, RESULT_IMAGE_SIZE,
                                             Qt::KeepAspectRatio, Qt::SmoothTransformation);

    ui->preview_label->setPixmap(getCircularPixmap(result, RESULT_IMAGE_SIZE));
}

// Cut image and update avatar
void AvatarEditor::saveImage() {
    qDebug() << "Saving image";
    qreal reversedScaleFactor = 1 / scaleFactor;
    qDebug() << reversedScaleFactor << ellipseItem->scale();
    QRect rect((int) (ellipseItem->x() * reversedScaleFactor), (int) (ellipseItem->y() * reversedScaleFactor),
               (int) (ellipseItem->rect().width() * reversedScaleFactor * ellipseItem->scale()),
               (int) (ellipseItem->rect().height() * reversedScaleFactor * ellipseItem->scale()));

    QImage result = image->copy(rect).scaled(RESULT_IMAGE_SIZE, RESULT_IMAGE_SIZE,
                                             Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // TODO database work here

    auto *mainWindow = qobject_cast<MainWindow *>(this->parentWidget());

    if (MainWindow::currentState == MESSAGES) {
        QString path("../resources/images/chats/%1.png");
        if (!result.save(QString(path).arg(MainWindow::currentChat->getId()), "png")) {
            qCritical() << "Error: can't save image";
            return;
        }
        // TODO database update chat avatar
        sqlUpdateChatAvatar(MainWindow::currentChat->getId(), result);

        mainWindow->updateChat(MainWindow::currentChat->getId(), MainWindow::currentChat->getName(),
                               result, MainWindow::currentChat->getRole());
        QString content = QString("Пользователь %1 изменил аватарку беседы").arg(
                MainWindow::currentUser->getUsername());

        // TODO database send message
        MessageInfo message(-1, content, nullptr, SYSTEM_MESSAGE, MainWindow::currentChat->getId(),
                            MainWindow::currentUser->getId());
        sqlSendMessage(message);

        mainWindow->addMessage(MainWindow::currentChat->getId(), 11, "", "", QImage(), content, SYSTEM_MESSAGE);
    } else {  // if (currentState == USER)
        // TODO user avatar
        QString path("../resources/images/users/%1.png");
        if (!result.save(QString(path).arg(MainWindow::currentUser->getId()), "png")) {
            qCritical() << "Error: can't save image";
        }
    }

    close();
}

// Switch stacked widget to file choosing page
void AvatarEditor::setChooseFilePage() const {
    ui->avatar_editor_stacked_widget->setCurrentIndex(0);
}

// Let user choose an image
void AvatarEditor::editorFileChooserOpen() {
    qDebug() << "Open file chooser";
    QString fileName = QFileDialog::getOpenFileName(this, "Open a file", "/", "Image Files (*.png)");
    if (fileName != nullptr) { // TODO refactor
        qDebug() << "File chosen:" << fileName;
        loadImageIntoScene(fileName);
        auto *mainWindow = qobject_cast<MainWindow *>(this->parentWidget());
        auto windowCentreX = mainWindow->x() + mainWindow->width() / 2 - EDITOR_PAGE_WIDTH / 2;
        auto windowCentreY = mainWindow->y() + mainWindow->height() / 2 - EDITOR_PAGE_HEIGHT / 2;
        mainWindow->avatarEditor->setGeometry(windowCentreX, windowCentreY,
                                              EDITOR_PAGE_WIDTH, EDITOR_PAGE_HEIGHT);
        mainWindow->avatarEditor->setFixedSize(EDITOR_PAGE_WIDTH, EDITOR_PAGE_HEIGHT);
        ui->avatar_editor_stacked_widget->setCurrentIndex(1); // TODO ?
    } else {
        qDebug() << "Cancel choosing";
    }

}

// Clear scene when we have finished
void AvatarEditor::closeEvent(QCloseEvent *e) {
    QDialog::closeEvent(e);
    ui->graphicsView->scene()->clear();
    qDebug() << "Editor closed";
}