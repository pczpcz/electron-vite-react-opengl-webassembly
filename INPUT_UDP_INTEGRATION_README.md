# 鼠标键盘信号与 UDP 通讯集成说明

## 概述

本项目成功实现了渲染进程的鼠标键盘信号与主进程 UDP 通讯的衔接。当用户在 Electron 应用的 canvas 上进行鼠标操作或键盘输入时，这些事件会被捕获并通过 UDP 协议发送到指定的目标地址。

## 架构设计

### 数据流

```
渲染进程 (React) → 预加载脚本 → 主进程 (Electron) → UDP客户端 → 目标UDP服务器
```

### 组件说明

1. **渲染进程** (`src/components/myapp/Index.jsx`)

   - 监听 canvas 上的鼠标事件（移动、点击、按下、释放）
   - 监听 canvas 上的键盘事件（按下、释放、输入）
   - 通过 IPC 将事件发送到主进程

2. **预加载脚本** (`electron/preload/preload.js`)

   - 提供`sendInputEvent` API 供渲染进程调用
   - 处理 IPC 通信桥接

3. **主进程** (`electron/main/index.js`)

   - 接收渲染进程的输入事件
   - 初始化 UDP 客户端
   - 将输入事件转换为 UDP 命令并发送

4. **UDP 客户端**
   - 负责将输入事件发送到目标 UDP 服务器
   - 支持多种事件类型编码

## 事件类型与 UDP 命令格式

### 鼠标事件

- `mouse_move:x,y` - 鼠标移动
- `mouse_down:button,x,y` - 鼠标按下
- `mouse_up:button,x,y` - 鼠标释放
- `mouse_click:button,x,y` - 鼠标点击

### 键盘事件

- `key_down:key` - 按键按下
- `key_up:key` - 按键释放
- `key_press:key` - 按键输入

### 鼠标按钮编码

- `0` - 左键
- `1` - 中键
- `2` - 右键

## 配置参数

### UDP 服务器配置

```javascript
const UDP_PORT = 8888;
const UDP_HOST = "0.0.0.0";
```

### UDP 客户端配置

```javascript
const UDP_TARGET_PORT = 8888;
const UDP_TARGET_HOST = "127.0.0.1";
```

## 使用方法

### 1. 启动 Electron 应用

```bash
npm run dev
```

### 2. 启动 UDP 事件监听器（测试用）

```bash
node test_input_udp.js
```

### 3. 测试功能

- 在 Electron 应用的 canvas 上移动鼠标
- 点击 canvas（左键、右键）
- 点击 canvas 使其获得焦点，然后按键盘按键

### 4. 观察输出

在 UDP 事件监听器的控制台中，您将看到类似以下的输出：

```
🐭 鼠标移动: (150, 200)
🖱️ 鼠标点击: 左键 在 (150, 200)
⌨️ 按键按下: a
⌨️ 按键释放: a
```

## 技术特点

1. **实时性**: 输入事件立即转换为 UDP 命令发送
2. **精确坐标**: 鼠标坐标基于 canvas 元素计算
3. **完整事件**: 支持所有主要的鼠标和键盘事件
4. **可扩展**: 易于添加新的事件类型和命令格式
5. **错误处理**: 完善的错误处理和日志记录
6. **状态反馈**: 实时显示 UDP 连接状态和通讯结果
7. **可视化界面**: 在 UI 中清晰显示连接状态和历史记录

## 自定义配置

要修改 UDP 目标地址，请编辑 `electron/main/index.js` 中的配置：

```javascript
const UDP_TARGET_PORT = 8888; // 修改为目标端口
const UDP_TARGET_HOST = "127.0.0.1"; // 修改为目标IP地址
```

## 注意事项

1. 键盘事件需要 canvas 获得焦点才能触发
2. UDP 是无连接协议，不保证数据包顺序和到达
3. 在高频事件（如鼠标移动）场景下，建议进行事件节流
4. 生产环境中应考虑 UDP 数据包的安全性和验证

## 故障排除

### 问题：UDP 命令未收到

- 检查目标 UDP 服务器是否运行
- 验证端口和 IP 地址配置
- 检查防火墙设置

### 问题：键盘事件不触发

- 确保 canvas 已获得焦点（点击 canvas）
- 检查浏览器控制台是否有错误信息

### 问题：坐标不准确

- 确认 canvas 元素正确渲染
- 检查事件监听器是否正确绑定
