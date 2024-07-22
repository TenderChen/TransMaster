#include "stdafx.h"
#include "TransMaster.h"

#include <Windows.h>
#include <Psapi.h>

TransMaster::TransMaster(QSettings& settings, QWidget* parent)
    : QMainWindow(parent)
    , sts(&settings)
{
    ui.setupUi(this);

    shortCutBtns.reserve(2);
    shortCutBtns.append(ui.keySequenceEdit_mode);
    shortCutBtns.append(ui.keySequenceEdit_window);

    ui.comboBox_mode;
    ui.label_last;

    readSettings();

    timer.setInterval(500);
    connect(&timer, &QTimer::timeout, this, &TransMaster::work);
}

TransMaster::~TransMaster()
{
}

void TransMaster::closeEvent(QCloseEvent* event)
{
    saveSettings();
    event->accept();
}

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
            show();
            break;
        default:
            break;
        }
        return true;
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}

void TransMaster::on_comboBox_mode_currentIndexChanged(int index)
{
    if (index == 1) {
        timer.start();
    }
    else {
        timer.stop();
    }
}

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
    if (modifiers & Qt::MetaModifier) {
        winModifiers |= MOD_WIN;
    }
    return winModifiers;
}

void TransMaster::onShortCutChanged(const QKeySequence& keySequence, int id) {
    HWND hwnd = reinterpret_cast<HWND>(this->winId());
    UnregisterHotKey(hwnd, id);

    if (!keySequence.isEmpty()) {
        auto k = keySequence[0];
        UINT winVirtualKey = QtKeyToWinVirtualKey(k.key());
        UINT winModifiers = QtModifierToWinModifiers(k.keyboardModifiers());
        RegisterHotKey(hwnd, id, winModifiers, winVirtualKey);
    }
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
}

void TransMaster::on_spinBox_taskbar_valueChanged(int value)
{
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindowVisible(hwnd)) {
        return TRUE; // 窗口不可见，继续枚举
    }

    // 检查窗口是否最小化
    if (IsIconic(hwnd)) {
        return TRUE; // 窗口已最小化，继续枚举
    }

    // 检查窗口边界是否被完全覆盖
    RECT rect;
    if (GetWindowRect(hwnd, &rect) && rect.right != rect.left && rect.bottom != rect.top) {
        // 获取窗口标题
        const int titleSize = 1024;
        TCHAR  title[titleSize];
        GetWindowText(hwnd, title, titleSize);
        qDebug() << "Window Title: " << hwnd << QString::fromWCharArray(title);

        // 获取执行文件路径
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if (processHandle != NULL) {
            TCHAR  path[MAX_PATH];
            if (GetModuleFileNameEx(processHandle, NULL, path, MAX_PATH) > 0) {
                qDebug() << "Executable Path: " << QString::fromWCharArray(path);
            }
            else {
                qDebug() << "Failed to get executable path.";
            }
            CloseHandle(processHandle);
        }
        else {
            qDebug() << "Failed to open process for window.";
        }

        auto points = QVector<POINT>{
            POINT{rect.left + 1, rect.top + 1},
            POINT{rect.right - 1, rect.top + 1},
            POINT{rect.left + 1, rect.bottom - 1},
            POINT{rect.right - 1, rect.bottom - 1} };
        BOOL covered = TRUE;
        for (auto point : points) {
            HWND hTop = WindowFromPoint(point);
            qDebug() << hTop << hwnd << point.x << point.y;
            if (hTop == hwnd) {
                covered = FALSE;
                break;
            }
        }
        if (covered) {
            qDebug() << hwnd << "Window is covered by other window.";
            // 窗口边界被其他窗口覆盖
            //ShowWindow(hwnd, SW_MINIMIZE);
        }
        return TRUE; // 继续枚举其他窗口
    }

    return TRUE; // 继续枚举其他窗口
}

void TransMaster::work()
{
    EnumWindows(EnumWindowsProc, 0);
}

void TransMaster::readSettings()
{
    //global
    sts->beginGroup("global");
    ui.comboBox_mode->setCurrentIndex(sts->value("mode", 0).toInt());
    ui.spinBox_taskbar->setValue(sts->value("taskbar", 100).toInt());
    sts->endGroup();

    //shortcuts
    sts->beginGroup("shortcuts");
    for (auto key : sts->allKeys()) {
        int i = key.toInt();
        if (i >= 0 && i < shortCutBtns.size())
            shortCutBtns[i]->setKeySequence(sts->value(key).toString());
    }
    sts->endGroup();

    //others
    sts->beginGroup("others");
    others.reserve(sts->allKeys().size());
    for (auto key : sts->allKeys()) {
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
    for (auto key : others.keys()) {
        if (others[key] < 100)
            sts->setValue(key, others[key]);
    }
    sts->endGroup();
}