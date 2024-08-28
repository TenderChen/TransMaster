# TransMaster

最开始我出于工作时欣赏桌面壁纸的需求去找寻窗口半透明方法，glass2k比较完美的满足了我的需求。相比于mac，windows在全屏一个新窗口的时候并不会把其它窗口最小化或者挪到其它桌面。Autohotkey可以比较简单的实现这个功能，即使是多显示器。但glass2k自身会在一定情况下崩溃，而在另一些情况下失效。此外还有多窗口分屏实现全屏覆盖需要一定的计算。然后窗口半透明实现了弱化版本的增加显示器功能，一定程度上将单显示器变成了两层，这意味着在最小化其它窗口时可以保留一层全屏覆盖的窗口。这些促使我想搞个glass2k的替代品。

软件的主要功能分为两类。一个是glass2k已有的窗口和任务栏透明度调节和记录，不包含置顶这个已被PowerToys支持的功能。另一个是全局的休闲或工作模式，休闲模式在显示器实现新的全屏覆盖时最小化其它窗口以欣赏桌面背景，工作模式则额外保留底下一层窗口实现单显示器的两层窗口办公。事实上工作模式的支持可有可无，在两层半透明的窗口之下的桌面背景和其它窗口几乎完全看不出效果，因此它只有在半透明效果太好的情况下才考虑专门支持。

在开发需要调用win32 API的 Windows GUI 程序的选择上，GPT推荐主要有 C# .net 和 C++ QT两类。我在2019年之后就没怎么使用C++ QT了，那个时候C++ 11 和 QT 5都还是半升级状态，现在基本上没啥编码习惯残留了。我还记得那年我兴冲冲学了一段时间的Go，结果面试时手写代码 脑子里C++ Python Go代码集体打架的情况。现在C++和我当时的印象差距可太大了，它尝试像其它高级语言一样精简代码强调意图，但保留了之前让人容易犯错的设计倾向，在GPT大行其道的环境里，这个设计思路显得格格不入。

全局的修饰键+键盘和修饰键+鼠标是两种实现机制，后者GPT说是通过鼠标事件钩子实现的，有性能和安全隐患。QT没有官方的跨平台全局快捷键功能，但是对应的win32API比较简单。

C++ Qt中我想加一个按钮，它展示的是一个全局快捷键，记录在配置文件中，启动时读取并调用win32API注册全局快捷键；点击按钮时可以捕获新输入的全局快捷键进行替换，应该怎么搞? GPT:QSettings RegisterHotKey UnregisterHotKey QPushButton QKeyEvent

难绷，逻辑卡住了。通过窗口的边界判断窗口是否被完全覆盖在大多数复杂情况下不能正常工作，使用区域相关的API也会碰到透明度和层叠等问题。考虑分屏多窗口和浮动窗口下的单层覆盖模式逻辑实现遇到了极大的问题。

先考虑调节窗口透明度的方案好了，GPT提供了 SetLayeredWindowAttributes/DirectComposition API等方法，就用简单粗暴的SetLayeredWindowAttributes好了。

windows中我想在窗口获取焦点后调节它的透明度，最简单的api实现方案是什么

windows中我想实现操作系统在其它的窗口程序获得焦点后通知我针对它触发一系列操作，用C++ QT在mainwindow中如何实现
这个和使用系统全局钩子监听焦点事件相比有什么优劣

使用EnumChildWindows来对所有子窗口调节透明度效果很奇怪，就像两个半透明窗口叠在一起一样整体变的黑灰了。RestoreAllChildWindows在这种情况下也没法完美修复，只能重启。它的引入没有解决问题反而增加了问题，只对前台主窗口调节透明度是更好的选择。

针对SetLayeredWindowAttributes不能很好的处理层叠子窗口的情况，GPT提供了UpdateLayeredWindow的方案，但提及了管理上额外的复杂度。这个以后无聊了再考虑。

再返回考虑单层覆盖的问题，WindowFromPoint 方案是失败的，那只能处理当前窗口的覆盖区域了。遍历其它窗口，如果被当前激活主窗口区域包含，则最小化。

最后是参考qt的 systray 示例项目实现 关闭窗口后到托盘区，托盘区右键退出 的机制。

完结撒花，告一段落，以后可能考虑 双层覆盖 这种适用于工作的单屏变双屏的模式支持。

新发现的问题：Snipaste 的截图窗口 会被捕获为前台窗口然后在单层覆盖模式下最小化该屏幕下的其它窗口，但钉钉的截图不会；单层覆盖模式应该规避这类置顶窗口

添加单层透明模式，适用于工作时前台窗口半透明，底层窗口不透明实现拓展屏幕的作用

添加一个列表展示读取过的窗口，双击可以切换过去调透明度

周期扫描中清理列表中无效的窗口句柄减少长度