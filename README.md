# TransMaster

最开始我出于工作时欣赏桌面壁纸的需求去找寻窗口半透明方法，glass2k比较完美的满足了我的需求。相比于mac，windows在全屏一个新窗口的时候并不会把其它窗口最小化或者挪到其它桌面。Autohotkey可以比较简单的实现这个功能，即使是多显示器。但glass2k自身会在一定情况下崩溃，而在另一些情况下失效。此外还有多窗口分屏实现全屏覆盖需要一定的计算。然后窗口半透明实现了弱化版本的增加显示器功能，一定程度上将单显示器变成了两层，这意味着在最小化其它窗口时可以保留一层全屏覆盖的窗口。这些促使我想搞个glass2k的替代品。

软件的主要功能分为两类。一个是glass2k已有的窗口和任务栏透明度调节和记录，不包含置顶这个已被PowerToys支持的功能。另一个是全局的休闲或工作模式，休闲模式在显示器实现新的全屏覆盖时最小化其它窗口以欣赏桌面背景，工作模式则额外保留底下一层窗口实现单显示器的两层窗口办公。事实上工作模式的支持可有可无，在两层半透明的窗口之下的桌面背景和其它窗口几乎完全看不出效果，因此它只有在半透明效果太好的情况下才考虑专门支持。

在开发需要调用win32 API的 Windows GUI 程序的选择上，GPT推荐主要有 C# .net 和 C++ QT两类。我在2019年之后就没怎么使用C++ QT了，那个时候C++ 11 和 QT 5都还是半升级状态，现在基本上没啥编码习惯残留了。我还记得那年我兴冲冲学了一段时间的Go，结果面试时手写代码 脑子里C++ Python Go代码集体打架的情况。现在C++和我当时的印象差距可太大了，它尝试像其它高级语言一样精简代码强调意图，但保留了之前让人容易犯错的设计倾向，在GPT大行其道的环境里，这个设计思路显得格格不入。

全局的修饰键+键盘和修饰键+鼠标是两种实现机制，后者GPT说是通过鼠标事件钩子实现的，有性能和安全隐患。QT没有官方的跨平台全局快捷键功能，但是对应的win32API比较简单。

C++ Qt中我想加一个按钮，它展示的是一个全局快捷键，记录在配置文件中，启动时读取并调用win32API注册全局快捷键；点击按钮时可以捕获新输入的全局快捷键进行替换，应该怎么搞? GPT:QSettings RegisterHotKey UnregisterHotKey QPushButton QKeyEvent

难绷，逻辑卡住了。通过窗口的边界判断窗口是否被完全覆盖在大多数复杂情况下不能正常工作，使用区域相关的API也会碰到透明度和层叠等问题。考虑分屏多窗口和浮动窗口下的单层覆盖模式逻辑实现遇到了极大的问题。
