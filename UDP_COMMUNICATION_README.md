# UDP 通讯功能说明

## 功能概述

本功能在主进程中添加了 UDP 服务器，监听端口 8888，接收 UDP 消息并根据内容触发渲染进程执行不同的操作。

## 实现架构

### 1. 主进程 (electron/main/index.js)

- 创建 UDP 服务器，监听端口 8888
- 接收 UDP 消息并解析命令
- 通过 IPC 将命令发送到渲染进程

### 2. 预加载脚本 (electron/preload/preload.js)

- 添加 UDP 命令监听器
- 提供`onUDPCommand` API 供渲染进程使用

### 3. 渲染进程 (src/components/myapp/Index.jsx)

- 监听 UDP 命令事件
- 根据命令执行相应的操作
- 显示命令历史和状态信息

## 可用命令

| 命令     | 功能描述     |
| -------- | ------------ |
| `cmd1`   | 切换背景颜色 |
| `cmd2`   | 开始动画     |
| `cmd3`   | 停止动画     |
| `cmd4`   | 重置动画     |
| `status` | 获取当前状态 |

## 使用方法

### 1. 启动应用

```bash
npm run dev
```

### 2. 发送 UDP 命令

使用任何 UDP 客户端工具发送命令到 `127.0.0.1:8888`

#### 使用提供的测试脚本：

```bash
node test_udp_simple.cjs
```

#### 手动发送命令（Windows）：

```bash
echo "cmd1" | nc -u 127.0.0.1 8888
```

#### 使用 Python 脚本：

```python
import socket

client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client.sendto(b"cmd1", ("127.0.0.1", 8888))
client.close()
```

## 界面说明

应用界面包含两个控制面板：

1. **左侧控制面板**：

   - 显示当前状态
   - 显示动画运行状态
   - 显示最后接收的命令

2. **右侧历史面板**：
   - 显示最近 10 条 UDP 命令历史
   - 包含命令内容、来源 IP 和时间戳

## 技术细节

### UDP 服务器配置

- 端口：8888
- 绑定地址：0.0.0.0（监听所有网络接口）
- 协议：UDP/IPv4

### 通信流程

1. UDP 客户端发送命令到 `127.0.0.1:8888`
2. 主进程接收并解析命令
3. 主进程通过 IPC 发送命令到渲染进程
4. 渲染进程执行相应操作并更新界面

### 错误处理

- UDP 服务器错误处理
- 窗口未就绪时的命令缓存
- 未知命令处理

## 扩展命令

要添加新的命令，请在渲染进程的 `handleCommand` 函数中添加新的 case：

```javascript
case 'new_command':
    // 执行新命令的操作
    console.log('执行新命令');
    break;
```

## 注意事项

1. 确保防火墙允许 UDP 端口 8888 的通信
2. 应用启动时会自动初始化 UDP 服务器
3. 命令不区分大小写
4. 命令历史最多保存 10 条记录
