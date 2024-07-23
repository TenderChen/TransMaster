#pragma once

#include <QtWidgets/QMainWindow>
#include <QSettings>
#include <QHash>
#include <QTimer>
#include "ui_TransMaster.h"

struct WindowInfo {
    QString title;
    QString path;
};

class TransMaster : public QMainWindow
{
    Q_OBJECT

public:
    TransMaster(QSettings& settings, QWidget* parent = nullptr);
    ~TransMaster();

protected:

    void closeEvent(QCloseEvent* event) override;
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;

    void onShortCutChanged(const QKeySequence& keySequence, int id);
    void changePath(const QString& path);

private slots:
    void on_comboBox_mode_currentIndexChanged(int index);
    void on_keySequenceEdit_mode_keySequenceChanged(const QKeySequence& keySequence);
    void on_keySequenceEdit_window_keySequenceChanged(const QKeySequence& keySequence);
    void on_spinBox_current_valueChanged(int value);
    void on_spinBox_taskbar_valueChanged(int value);

    void sbCurrentTimer();
    void sbTaskbarTimer();

    //遍历窗口 最小化完全被其它窗口遮挡的窗口
    void workMode();

    //获取
    void workWindow();

private:
    void readSettings();
    void saveSettings();

private:
    Ui::TransMasterClass ui;
    QSettings* sts;
    HWND self;
    //应用程序路径-透明度百分比
    QHash<QString, int> others;
    //快捷键
    QList<QKeySequenceEdit*> shortCutBtns;
    QTimer timerMode;
    QTimer timerWindow;
    QTimer timerSbCurrent;
    QTimer timerSbTaskbar;
    //窗口-应用程序路径 不用重复获取
    QHash<HWND, WindowInfo> hwnds;
    //透明度调节后需要重新处理的窗口 使用前需要检查并同步清理hwnds
    QMultiHash<QString, HWND> hwndsHash;
};
