# SA_AutoChat

> **整蛊用。**

《心灵终结》3.3.6 的独立 Syringe 插件。从第 2800 帧起，每隔 2800 个逻辑帧自动发送「我是羊了个羊加号！」。联机发送到全体聊天，单机显示在本地消息区。

从 [Releases](https://github.com/SapphireAero/SA_AutoChat/releases) 下载并解压，将 DLL 放到 `gamemd.exe` 同目录即可加载。退出游戏后移除 DLL 即可关闭。

使用 32 位 MinGW-w64 运行 `python build.py`，生成 `dist/SA_AutoChat.dll`。可通过 `MOE_MINGW` 指定工具链的 `bin` 目录。

## 许可证

整个项目（源码、构建脚本、文档和发布的插件）采用 [MIT 许可证](LICENSE)。
