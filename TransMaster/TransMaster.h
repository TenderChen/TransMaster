#pragma once

#include <QtWidgets/QMainWindow>
#include <QSettings>
#include <QHash>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include "ui_TransMaster.h"
#include <Windows.h>

struct WindowInfo {
    QString title;
    QString path;
    RECT rect;
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
    //void on_comboBox_mode_currentIndexChanged(int index);
    void on_checkBox_toggled(bool checked);
    void on_comboBox_icon_currentIndexChanged(int index);
    void on_keySequenceEdit_mode_keySequenceChanged(const QKeySequence& keySequence);
    void on_keySequenceEdit_window_keySequenceChanged(const QKeySequence& keySequence);
    void on_spinBox_current_valueChanged(int value);
    void on_spinBox_taskbar_valueChanged(int value);
    void on_spinBox_scan_valueChanged(int value);

    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    void sbCurrentTimer();
    void sbTaskbarTimer();
    void sbScanTimer();

    void workWindow();

private:
    void readSettings();
    void saveSettings();

private:
    Ui::TransMasterClass ui;
    QSettings* sts;
    HWND self;
    int appIcon;
    //应用程序路径-透明度百分比
    QHash<QString, int> others;
    //快捷键
    QList<QKeySequenceEdit*> shortCutBtns;
    QTimer timerWindow;
    QTimer timerSbCurrent;
    QTimer timerSbTaskbar;
    QTimer timerSbScan;
    //窗口-应用程序路径 不用重复获取
    QHash<HWND, WindowInfo> hwnds;
    //透明度调节后需要重新处理的窗口 使用前需要检查并同步清理hwnds
    QMultiHash<QString, HWND> hwndsHash;

    QAction* quitAction;
    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
};
