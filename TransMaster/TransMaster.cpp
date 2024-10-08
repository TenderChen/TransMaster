#include "stdafx.h"
#include "TransMaster.h"

#include <Psapi.h>

TransMaster::TransMaster(QSettings& settings, QWidget* parent)
    : QMainWindow(parent, Qt::WindowStaysOnTopHint)
    , sts(&settings)
{
    ui.setupUi(this);

    shortCutBtns.reserve(2);
    shortCutBtns.append(ui.keySequenceEdit_mode);
    shortCutBtns.append(ui.keySequenceEdit_window);

    self = reinterpret_cast<HWND>(this->winId());

    readSettings();

    //跳过任务栏窗口
    sbTaskbarTimer();

    //系统图标
    QIcon icon = QIcon::fromTheme(QIcon::ThemeIcon(appIcon));
    setWindowIcon(icon);

    //托盘区菜单
    quitAction = new QAction(tr("退出"), this);
    connect(quitAction, &QAction::triggered, this, &TransMaster::quitSave);
    quitRestoreAction = new QAction(tr("恢复不透明退出"), this);
    connect(quitRestoreAction, &QAction::triggered, this, &TransMaster::quitRestore);

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(quitAction);
    trayIconMenu->addAction(quitRestoreAction);

    //托盘图标
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(icon);
    trayIcon->show();

    connect(trayIcon, &QSystemTrayIcon::activated, this, &TransMaster::iconActivated);

    timerWindow.setInterval(ui.spinBox_scan->value());
    connect(&timerWindow, &QTimer::timeout, this, &TransMaster::workWindow);

    timerSbCurrent.setSingleShot(true);
    timerSbCurrent.setInterval(300);
    connect(&timerSbCurrent, &QTimer::timeout, this, &TransMaster::sbCurrentTimer);

    timerSbTaskbar.setSingleShot(true);
    timerSbTaskbar.setInterval(300);
    connect(&timerSbTaskbar, &QTimer::timeout, this, &TransMaster::sbTaskbarTimer);

    timerSbScan.setSingleShot(true);
    timerSbScan.setInterval(300);
    connect(&timerSbScan, &QTimer::timeout, this, &TransMaster::sbScanTimer);
}

TransMaster::~TransMaster()
{
}

void TransMaster::closeEvent(QCloseEvent* event)
{
    quitSave();
}

void TransMaster::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            hide();
        }
    }
    QMainWindow::changeEvent(event);
}

BOOL CALLBACK RestoreWindowTransparency(HWND hwnd, LPARAM lParam) {
    // 移除 WS_EX_LAYERED 样式
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
    // 恢复不透明度
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    // 重绘窗口
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
    return TRUE;
}

//void RestoreAllChildWindows(HWND parentHwnd) {
//    EnumChildWindows(parentHwnd, RestoreWindowTransparency, 0);
//}

// 假设hWnd是你要操作的窗口句柄，bTransparency 0-255(不透明)
void SetWindowTransparency(HWND hWnd, BYTE bTransparency) {
    if (bTransparency >= 255) {
        RestoreWindowTransparency(hWnd, bTransparency);
    }
    else {
        // 添加 WS_EX_LAYERED 扩展样式
        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        // 设置透明度
        SetLayeredWindowAttributes(hWnd, 0, bTransparency, LWA_ALPHA);
    }
}

/*void SetWindowTransparencyAll(HWND hwndMain, BYTE bTransparency) {
    if (bTransparency >= 255) {
        RestoreAllChildWindows(hwndMain);
    }
    else {
        SetWindowTransparency(hwndMain, bTransparency);
        EnumChildWindows(hwndMain, [](HWND hwnd, LPARAM lParam) -> BOOL {
            SetWindowTransparency(hwnd, (BYTE)lParam);
            return TRUE;
            }, bTransparency);
    }
}*/

bool TransMaster::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
{
    MSG* msg = static_cast<MSG*>(message);

    if (msg->message == WM_HOTKEY)
    {
        // 检查是哪个热键被触发
        switch (msg->wParam)
        {
        case 0:  // 切换模式
            ui.comboBox_mode->setCurrentIndex((ui.comboBox_mode->currentIndex() + 1) % ui.comboBox_mode->count());
            break;
        case 1:  // 呼出窗口
            workWindow();
            showNormal();
            activateWindow();
            break;
        default:
            break;
        }
        return true;
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}

//void TransMaster::on_comboBox_mode_currentIndexChanged(int index)
//{
//    //
//}

UINT QtKeyToWinVirtualKey(Qt::Key key) {
    // 映射 Qt 键码到 Windows 虚拟键码，不含修饰键
    // 完整的让gpt列，我这是让gpt列的笔记本的87键部分
    switch (key) {
        //top
    case Qt::Key_Escape: return VK_ESCAPE;
    case Qt::Key_F1: return VK_F1;
    case Qt::Key_F2: return VK_F2;
    case Qt::Key_F3: return VK_F3;
    case Qt::Key_F4: return VK_F4;
    case Qt::Key_F5: return VK_F5;
    case Qt::Key_F6: return VK_F6;
    case Qt::Key_F7: return VK_F7;
    case Qt::Key_F8: return VK_F8;
    case Qt::Key_F9: return VK_F9;
    case Qt::Key_F10: return VK_F10;
    case Qt::Key_F11: return VK_F11;
    case Qt::Key_F12: return VK_F12;
        //1
    case Qt::Key_QuoteLeft: return VK_OEM_3;
    case Qt::Key_1: return '1';
    case Qt::Key_2: return '2';
    case Qt::Key_3: return '3';
    case Qt::Key_4: return '4';
    case Qt::Key_5: return '5';
    case Qt::Key_6: return '6';
    case Qt::Key_7: return '7';
    case Qt::Key_8: return '8';
    case Qt::Key_9: return '9';
    case Qt::Key_0: return '0';
    case Qt::Key_Minus: return VK_OEM_MINUS;
    case Qt::Key_Equal:
    case Qt::Key_Plus: return VK_OEM_PLUS;
    case Qt::Key_Backspace: return VK_BACK;
        //2
    case Qt::Key_Tab:
    case Qt::Key_Backtab: return VK_TAB;
    case Qt::Key_BraceLeft:
    case Qt::Key_BracketLeft: return VK_OEM_4;
    case Qt::Key_BraceRight:
    case Qt::Key_BracketRight: return VK_OEM_6;
    case Qt::Key_Backslash: return VK_OEM_5;
        //3
    case Qt::Key_CapsLock: return VK_CAPITAL;
    case Qt::Key_Semicolon: return VK_OEM_1;
    case Qt::Key_QuoteDbl: return VK_OEM_7;
    case Qt::Key_Return:
    case Qt::Key_Enter: return VK_RETURN;
        //4
    case Qt::Key_Comma: return VK_OEM_COMMA;
    case Qt::Key_Period: return VK_OEM_PERIOD;
    case Qt::Key_Slash: return VK_OEM_2;
        //5
    case Qt::Key_Space: return VK_SPACE;
    case Qt::Key_Menu: return VK_APPS;
        //
    case Qt::Key_Left: return VK_LEFT;
    case Qt::Key_Up: return VK_UP;
    case Qt::Key_Right: return VK_RIGHT;
    case Qt::Key_Down: return VK_DOWN;
    case Qt::Key_PageUp: return VK_PRIOR;
    case Qt::Key_PageDown: return VK_NEXT;
    case Qt::Key_End: return VK_END;
    case Qt::Key_Home: return VK_HOME;
    case Qt::Key_Insert: return VK_INSERT;
    case Qt::Key_Delete: return VK_DELETE;
        //
    case Qt::Key_A: return 'A';
    case Qt::Key_B: return 'B';
    case Qt::Key_C: return 'C';
    case Qt::Key_D: return 'D';
    case Qt::Key_E: return 'E';
    case Qt::Key_F: return 'F';
    case Qt::Key_G: return 'G';
    case Qt::Key_H: return 'H';
    case Qt::Key_I: return 'I';
    case Qt::Key_J: return 'J';
    case Qt::Key_K: return 'K';
    case Qt::Key_L: return 'L';
    case Qt::Key_M: return 'M';
    case Qt::Key_N: return 'N';
    case Qt::Key_O: return 'O';
    case Qt::Key_P: return 'P';
    case Qt::Key_Q: return 'Q';
    case Qt::Key_R: return 'R';
    case Qt::Key_S: return 'S';
    case Qt::Key_T: return 'T';
    case Qt::Key_U: return 'U';
    case Qt::Key_V: return 'V';
    case Qt::Key_W: return 'W';
    case Qt::Key_X: return 'X';
    case Qt::Key_Y: return 'Y';
    case Qt::Key_Z: return 'Z';
        //
    default:
        qDebug() << "QtKeyToWinVirtualKey: " << key;
        return 0; // 如果没有找到匹配的键，返回 0
    }
}

UINT QtModifierToWinModifiers(Qt::KeyboardModifiers modifiers) {
    // 映射 Qt 修饰键到 Windows 修饰键
    UINT winModifiers = 0;
    if (modifiers & Qt::ShiftModifier) {
        winModifiers |= MOD_SHIFT;
    }
    if (modifiers & Qt::ControlModifier) {
        winModifiers |= MOD_CONTROL;
    }
    if (modifiers & Qt::AltModifier) {
        winModifiers |= MOD_ALT;
    }
    //https://github.com/pbek/QOwnNotes/issues/1671 使用win键前按下其它修饰键
    if (modifiers & Qt::MetaModifier) {
        winModifiers |= MOD_WIN;
    }
    return winModifiers;
}

void TransMaster::onShortCutChanged(const QKeySequence& keySequence, int id) {
    UnregisterHotKey(self, id);

    if (!keySequence.isEmpty()) {
        auto k = keySequence[0];
        UINT winVirtualKey = QtKeyToWinVirtualKey(k.key());
        UINT winModifiers = QtModifierToWinModifiers(k.keyboardModifiers());
        RegisterHotKey(self, id, winModifiers, winVirtualKey);
    }
}

void TransMaster::changePath(const QString& path)
{
    if (path.isEmpty() || ui.label_path->text() == path) {
        return;
    }
    ui.label_path->setText(path);
    ui.spinBox_current->setValue(others.value(path, 100));
}

BOOL TransMaster::checkWindow(HWND hwnd)
{
    auto rt = IsWindow(hwnd) && IsWindowVisible(hwnd);
    if (!rt) {
        if (hwnds.contains(hwnd)) {
            hwndsHash.remove(hwnds[hwnd].path, hwnd);
            hwnds.remove(hwnd);
        }
        if (hwndItems.contains(hwnd)) {
            auto item = hwndItems[hwnd];
            ui.listWidget->takeItem(ui.listWidget->row(item));
            hwndItems.remove(hwnd);
            delete item;
        }
    }
    return rt;
}

void TransMaster::on_checkBox_toggled(bool checked)
{
    ui.checkBox->setEnabled(false);
    ui.comboBox_icon->clear();
    for (int i = 0; i < (int)QIcon::ThemeIcon::NThemeIcons; i++) {
        QString text = QString::number(i);
        QIcon icon = QIcon::fromTheme(QIcon::ThemeIcon(i));
        ui.comboBox_icon->addItem(icon, text);
    }
}

void TransMaster::on_checkBox_scan_toggled(bool checked)
{
    if (checked) {
        timerWindow.start();
    }
    else {
        timerWindow.stop();
    }
}

void TransMaster::on_comboBox_icon_currentIndexChanged(int index)
{
    QIcon icon = ui.comboBox_icon->itemIcon(index);
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
    appIcon = ui.comboBox_icon->itemText(index).toInt();
}

void TransMaster::on_keySequenceEdit_mode_keySequenceChanged(const QKeySequence& keySequence)
{
    onShortCutChanged(keySequence, 0);
}

void TransMaster::on_keySequenceEdit_window_keySequenceChanged(const QKeySequence& keySequence)
{
    onShortCutChanged(keySequence, 1);
}

void TransMaster::on_spinBox_current_valueChanged(int value)
{
    timerSbCurrent.start();
}

void TransMaster::on_spinBox_taskbar_valueChanged(int value)
{
    timerSbTaskbar.start();
}

void TransMaster::on_spinBox_scan_valueChanged(int value)
{
    timerSbScan.start();
}

void TransMaster::on_listWidget_itemDoubleClicked(QListWidgetItem* item)
{
    auto hwnd = (HWND)(item->text().split(" - ")[0].toULongLong(nullptr, 16));
    if (checkWindow(hwnd)) {
        // 切换到窗口
        SwitchToThisWindow(hwnd, TRUE);
    }
}

void TransMaster::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::MiddleClick:
        showNormal();
        activateWindow();
        break;
    default:
        ;
    }
}

void TransMaster::sbCurrentTimer()
{
    auto value = ui.spinBox_current->value();
    //qDebug() << "sbCurrentTimer " << value;
    auto path = ui.label_path->text();
    if (path.isEmpty()) {
        return;
    }
    if (value == others.value(path, 100)) {
        return;
    }
    others.insert(path, value);
    value = 255 * value / 100;
    QVector<HWND> invalids;
    for (auto hwnd : hwndsHash.values(path)) {
        if (checkWindow(hwnd)) {
            SetWindowTransparency(hwnd, value);
        }
        else {
            invalids.append(hwnd);
        }
    }
    for (auto hwnd : invalids) {
        hwndsHash.remove(path, hwnd);
        hwnds.remove(hwnd);

        hwndItems.remove(hwnd);
    }
}

void TransMaster::sbTaskbarTimer()
{
    auto value = ui.spinBox_taskbar->value();
    //qDebug() << "sbTaskbarTimer " << value;
    value = 255 * value / 100;
    QVector<HWND> tbs;
    HWND taskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (taskbar) {
        tbs.append(taskbar);
    }
    taskbar = FindWindow(L"Shell_SecondaryTrayWnd", NULL);
    while (taskbar) {
        tbs.append(taskbar);
        taskbar = FindWindowEx(NULL, taskbar, L"Shell_SecondaryTrayWnd", NULL);
    }
    for (auto hwnd : tbs) {
        skipWindows.insert(hwnd);
        SetWindowTransparency(hwnd, value);
    }
}

void TransMaster::sbScanTimer()
{
    timerWindow.setInterval(ui.spinBox_scan->value());
}

BOOL isCoveredBy(RECT rectSmall, RECT rectBig) {
    //qDebug() << rectSmall.left << rectSmall.right << rectSmall.top << rectSmall.bottom << rectBig.left << rectBig.right << rectBig.top << rectBig.bottom;
    return rectSmall.left >= rectBig.left && rectSmall.right <= rectBig.right && rectSmall.top >= rectBig.top && rectSmall.bottom <= rectBig.bottom;
}

bool IsWindowTopMost(HWND hWnd) {
    // 获取窗口的样式
    LONG style = GetWindowLong(hWnd, GWL_EXSTYLE);

    // 判断是否有 WS_EX_TOPMOST 标志
    return (style & WS_EX_TOPMOST) != 0;
}

// 函数：从HWND获取图标，并转换为QIcon
QIcon iconFromHWND(HWND hwnd) {
    HICON hIcon = nullptr;

    // 尝试获取大图标
    SendMessage(hwnd, WM_GETICON, ICON_BIG, (LPARAM)&hIcon);
    if (!hIcon) {
        // 如果没有大图标，尝试获取小图标
        SendMessage(hwnd, WM_GETICON, ICON_SMALL, (LPARAM)&hIcon);
    }
    if (!hIcon) {
        // 如果还是没有图标，尝试从类信息中获取
        hIcon = (HICON)GetClassLongPtr(hwnd, GCLP_HICON);
    }

    if (hIcon) {
        // 将HICON转换为QPixmap
        QPixmap pixmap = QPixmap::fromImage(QImage::fromHICON(hIcon));
        if (!pixmap.isNull()) {
            return QIcon(pixmap);
        }
    }

    return QIcon();
}

void TransMaster::workWindow()
{
    //清理listWidget中的无效窗口
    for (auto k : hwndItems.keys()) {
        checkWindow(k);
    }

    auto currentActiveWindow = GetForegroundWindow();
    //qDebug() << "Current Active Window: " << currentActiveWindow;
    if (currentActiveWindow == nullptr) {
        // 没有活动窗口，不做任何操作
        return;
    }
    if (currentActiveWindow == self) {
        // 自己就是当前活动窗口，不做任何操作
        return;
    }
    if (skipWindows.contains(currentActiveWindow)) {
        // 跳过任务栏等不方便处理的窗口
        return;
    }

    

    // 获取当前活动窗口的标题
    const int titleSize = 1024;
    TCHAR  title[titleSize];
    GetWindowText(currentActiveWindow, title, titleSize);
    QString titleStr = QString::fromWCharArray(title);

    if (titleStr.isEmpty()) {
        //跳过无标题的特殊窗口
        return;
    }

    WindowInfo wi = hwnds[currentActiveWindow];
    wi.title = titleStr;

    // 获取当前活动窗口的边界
    GetWindowRect(currentActiveWindow, &wi.rect);

    if (wi.path.isEmpty()) {
        // 获取当前活动窗口的执行文件路径
        DWORD pid = 0;
        GetWindowThreadProcessId(currentActiveWindow, &pid);
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

        if (processHandle != NULL) {
            TCHAR  path[MAX_PATH];
            if (GetModuleFileNameEx(processHandle, NULL, path, MAX_PATH) > 0) {
                wi.path = QString::fromWCharArray(path);
                hwndsHash.insert(wi.path, currentActiveWindow);
            }
            else {
                qDebug() << "Failed to get current active window executable path." << currentActiveWindow << wi.title;
            }
            CloseHandle(processHandle);
        }
        else {
            qDebug() << "Failed to open process for window." << currentActiveWindow << wi.title;
        }
    }

    hwnds.insert(currentActiveWindow, wi);
    //qDebug() << "Current Active Window: " << currentActiveWindow << wi.title << wi.path;

    QString key = QString::number((qulonglong)currentActiveWindow, 16);
    ui.label_last->setText(key);
    ui.label_title->setText(wi.title);
    changePath(wi.path);
    SetWindowTransparency(currentActiveWindow, 255 * ui.spinBox_current->value() / 100);

    //更新list
    auto item = hwndItems[currentActiveWindow];
    QString text = key + " - " + wi.title;
    if (item == nullptr) { //末尾添加
        item = new QListWidgetItem(iconFromHWND(currentActiveWindow), text, ui.listWidget);
        hwndItems.insert(currentActiveWindow, item);
    }
    else { //移到末尾
        item->setText(text);
        ui.listWidget->addItem(ui.listWidget->takeItem(ui.listWidget->row(item)));
    }
    ui.listWidget->setCurrentItem(item);

    if (ui.checkBox_scan->isChecked() && ui.comboBox_mode->currentIndex() != 0 && !IsRectEmpty(&wi.rect) && !IsWindowTopMost(currentActiveWindow)) {
        qDebug() << "rect compare start: " << currentActiveWindow << wi.title << wi.path << wi.rect.left << wi.rect.right << wi.rect.top << wi.rect.bottom;
        //最小化/不透明 被当前主窗口覆盖的其它主窗口
        for (auto i = hwnds.cbegin(), end = hwnds.cend(); i != end; ++i) {
            auto rt = (i.key() != currentActiveWindow && IsWindowVisible(i.key()) && !IsIconic(i.key())
                && !IsRectEmpty(&i.value().rect)
                && isCoveredBy(i.value().rect, wi.rect));
            qDebug() << "minimize/not transparent: " << rt << i.key() << i.value().title << i.value().path << i.value().rect.left << i.value().rect.right << i.value().rect.top << i.value().rect.bottom;
            if (rt) {
                switch (ui.comboBox_mode->currentIndex())
                {
                case 1: // 单层覆盖
                    ShowWindow(i.key(), SW_SHOWMINIMIZED);
                    break;
                case 2: // 单层透明
                    RestoreWindowTransparency(i.key(), 0);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void TransMaster::quitSave()
{
    saveSettings();

    QCoreApplication::quit();
}

void TransMaster::quitRestore()
{
    timerWindow.stop();
    timerSbCurrent.stop();
    timerSbTaskbar.stop();
    timerSbScan.stop();

    for (auto hwnd : hwnds.keys()) {
        RestoreWindowTransparency(hwnd, 0);
    }
    for (auto hwnd : skipWindows) {
        RestoreWindowTransparency(hwnd, 0);
    }

    quitSave();
}

void TransMaster::readSettings()
{
    //global
    sts->beginGroup("global");
    ui.comboBox_mode->setCurrentIndex(sts->value("mode", 0).toInt());
    ui.spinBox_taskbar->setValue(sts->value("taskbar", 100).toInt());
    ui.spinBox_scan->setValue(sts->value("scan", 1000).toInt());
    appIcon = sts->value("icon").toInt();
    sts->endGroup();

    //shortcuts
    sts->beginGroup("shortcuts");
    for (auto& key : sts->allKeys()) {
        int i = key.toInt();
        if (i >= 0 && i < shortCutBtns.size())
            shortCutBtns[i]->setKeySequence(sts->value(key).toString());
    }
    sts->endGroup();

    //others
    sts->beginGroup("others");
    others.reserve(sts->allKeys().size());
    for (auto& key : sts->allKeys()) {
        others.insert(key, sts->value(key).toInt());
    }
    sts->endGroup();
}

void TransMaster::saveSettings()
{
    //global
    sts->beginGroup("global");
    sts->setValue("mode", ui.comboBox_mode->currentIndex());
    sts->setValue("taskbar", ui.spinBox_taskbar->value());
    sts->setValue("scan", ui.spinBox_scan->value());
    sts->setValue("icon", appIcon);
    sts->endGroup();

    //shortcuts
    sts->beginGroup("shortcuts");
    sts->remove("");
    for (int i = 0; i < shortCutBtns.size(); i++) {
        sts->setValue(QString::number(i), shortCutBtns[i]->keySequence().toString());
    }
    sts->endGroup();

    //others
    sts->beginGroup("others");
    for (auto& key : others.keys()) {
        if (others[key] < 100) {
            sts->setValue(key, others[key]);
        }
        else {
            sts->remove(key);
        }
    }
    sts->endGroup();
}