import { app, BrowserWindow, shell, ipcMain } from 'electron';
import { createRequire } from 'node:module';
import { fileURLToPath } from 'node:url';
import path from 'node:path';
import os from 'node:os';
import dgram from 'dgram';

console.log('=== Electron主进程启动 ===');

const require = createRequire(import.meta.url);
const __dirname = path.dirname(fileURLToPath(import.meta.url));

// The built directory structure
//
// ├─�? dist-electron
// �? ├─�? main
// �? �? └── index.js    > Electron-Main
// �? └─�? preload
// �?   └── preload.mjs > Preload-Scripts
// ├─�? dist
// �? └── index.html    > Electron-Renderer
//
// 设置控制台编码为UTF-8，解决中文乱码问�?
if (process.platform === 'win32') {
  // 设置控制台编�?
  process.stdout.setDefaultEncoding('utf8');
  process.stderr.setDefaultEncoding('utf8');
}

process.env.APP_ROOT = path.join(__dirname, '../..');

export const MAIN_DIST = path.join(process.env.APP_ROOT, 'dist-electron');
export const RENDERER_DIST = path.join(process.env.APP_ROOT, 'dist');
export const VITE_DEV_SERVER_URL = process.env.VITE_DEV_SERVER_URL;

process.env.VITE_PUBLIC = VITE_DEV_SERVER_URL
  ? path.join(process.env.APP_ROOT, 'public')
  : RENDERER_DIST;

// Disable GPU Acceleration for Windows 7
if (os.release().startsWith('6.1')) app.disableHardwareAcceleration();

// Set application name for Windows 10+ notifications
if (process.platform === 'win32') app.setAppUserModelId(app.getName());

if (!app.requestSingleInstanceLock()) {
  app.quit();
  process.exit(0);
}

let win = null;
let udpServer = null;
let udpClient = null;
const preload = path.join(__dirname, '../preload/preload.mjs');
const indexHtml = path.join(RENDERER_DIST, 'index.html');

// UDP客户端配置（用于发送鼠标键盘信号到外部服务器）
// 注意：这里的目标端口和主机应该指向实际的外部UDP服务�?
const UDP_TARGET_PORT = 8888;
const UDP_TARGET_HOST = '127.0.0.1'; // 可以修改为实际的外部服务器IP

function createWindow() {
  win = new BrowserWindow({
    title: 'Main window',
    icon: path.join(process.env.VITE_PUBLIC, 'favicon.ico'),
    width: 1200,
    height: 800,
    webPreferences: {
      preload,
      // Warning: Enable nodeIntegration and disable contextIsolation is not secure in production
      nodeIntegration: true,

      // Consider using contextBridge.exposeInMainWorld
      // Read more on https://www.electronjs.org/docs/latest/tutorial/context-isolation
      contextIsolation: false,
    },
  });

  if (VITE_DEV_SERVER_URL) { // #298
    win.loadURL(VITE_DEV_SERVER_URL);
    // Open devTool if the app is not packaged
    win.webContents.openDevTools();
  } else {
    win.loadFile(indexHtml);
  }

  // Test actively push message to the Electron-Renderer
  win.webContents.on('did-finish-load', () => {
    win?.webContents.send('main-process-message', new Date().toLocaleString());
  });

  // Make all links open with the browser, not with the application
  win.webContents.setWindowOpenHandler(({ url }) => {
    if (url.startsWith('https:')) shell.openExternal(url);
    return { action: 'deny' };
  });

  // 监听渲染进程的状态更�?
  win.webContents.on('console-message', (event, level, message, line, sourceId) => {
    console.log(`[渲染进程] ${message}`);
  });

  const Module = require('../../public/OpenglWebTest.js');
  Module.onRuntimeInitialized = function() {
    // 在这里安全地调用C/C++导出的函�?
    console.log(Module._your_exported_function());
  };
}

// 新的IPC通信处理 - 从渲染进程接收状态更�?
ipcMain.handle('renderer-status-update', async (event, status) => {
  console.log(`[渲染进程状态更新] ${status}`);
  return { success: true };
});

ipcMain.handle('renderer-error', async (event, error) => {
  console.error(`[渲染进程错误] ${error}`);
  return { success: true };
});

// 处理渲染进程的鼠标键盘事�?
ipcMain.handle('renderer-input-event', async (event, eventData) => {
  console.log(`[输入事件] 类型: ${eventData.type}, 坐标: (${eventData.x}, ${eventData.y}), 按键: ${eventData.key}`);
  
  try {
    // 将输入事件转换为UDP命令并发�?
    const result = await sendInputEventAsUDP(eventData);
    return result;
  } catch (error) {
    console.error(`[IPC错误] 处理输入事件失败: ${error}`);
    return { success: false, error: error.message };
  }
});

// 可选：初始化UDP服务器（用于接收外部命令�?
// 如果不需要接收UDP命令，可以注释掉这个函数和下面的initUDPServer()调用
function initUDPServer() {
  udpServer = dgram.createSocket('udp4');
  
  udpServer.on('error', (err) => {
    console.error(`UDP服务器错�?: ${err.stack}`);
    udpServer.close();
  });

  udpServer.on('message', (msg, rinfo) => {
    // 确保UDP消息使用UTF-8编码
    const message = msg.toString('utf8').trim();
    console.log(`[UDP消息] 来自 ${rinfo.address}:${rinfo.port}: ${message}`);
    
    // 解析命令并发送到渲染进程
    handleUDPCommand(message, rinfo);
  });

  udpServer.on('listening', () => {
    const address = udpServer.address();
    console.log(`UDP服务器监�? ${address.address}:${address.port}`);
  });

  udpServer.bind(8888, '0.0.0.0');
}



// 处理UDP命令
function handleUDPCommand(command, rinfo) {
  // 发送命令到渲染进程
  if (win && win.webContents) {
    win.webContents.send('udp-command', {
      command: command,
      timestamp: new Date().toISOString(),
      source: `${rinfo.address}:${rinfo.port}`
    });
    console.log(`[UDP命令] 发送到渲染进程: ${command}`);
  } else {
    console.log(`[UDP命令] 窗口未就绪，无法发送命�?: ${command}`);
  }
}

// 初始化UDP客户�?
function initUDPClient() {
  try {
    udpClient = dgram.createSocket('udp4');
    
    udpClient.on('error', (err) => {
      console.error(`UDP客户端错�?: ${err.stack}`);
      // 发送连接失败状态到渲染进程
      if (win && win.webContents) {
        win.webContents.send('udp-status', {
          type: 'error',
          message: `UDP客户端连接失�?: ${err.message}`,
          timestamp: new Date().toISOString()
        });
      }
    });
    
    udpClient.on('listening', () => {
      const address = udpClient.address();
      console.log(`UDP客户端已就绪 ${address.address}:${address.port}`);
      // 发送连接成功状态到渲染进程
      if (win && win.webContents) {
        win.webContents.send('udp-status', {
          type: 'success',
          message: `UDP客户端已连接�? ${UDP_TARGET_HOST}:${UDP_TARGET_PORT}`,
          timestamp: new Date().toISOString()
        });
      }
    });
    
    // 绑定到随机端口，使用回调确保绑定完成
    udpClient.bind(0, '127.0.0.1', () => {
      console.log('UDP客户端绑定完�?');
    });
    console.log('UDP客户端初始化完成，等待绑�?...');
    
    return udpClient;
  } catch (error) {
    console.error(`UDP客户端初始化失败: ${error}`);
    return null;
  }
}

// 序列化数据包 - 使用与udp_test_client.js相同�?20字节协议格式
function serializePacket(userId, opType, mouseButton = 0, keyCode = 0, mouseX = 0, mouseY = 0) {
  const buffer = Buffer.alloc(20); // 20字节固定大小
  
  // userId (4字节)
  buffer.writeUInt32BE(userId, 0);
  
  // opType (1字节)
  buffer.writeUInt8(opType, 4);
  
  // mouseButton (1字节)
  buffer.writeUInt8(mouseButton, 5);
  
  // keyCode (2字节)
  buffer.writeUInt16BE(keyCode, 6);
  
  // mouseX (2字节)
  buffer.writeInt16BE(mouseX, 8);
  
  // mouseY (2字节)
  buffer.writeInt16BE(mouseY, 10);
  
  // timestamp (8字节)
  const timestamp = BigInt(Date.now());
  for (let i = 0; i < 8; i++) {
    buffer.writeUInt8(Number((timestamp >> BigInt(8 * (7 - i))) & 0xFFn), 12 + i);
  }
  
  return buffer;
}

// 将输入事件转换为UDP命令并发�?
function sendInputEventAsUDP(eventData) {
  console.log(`[调试] 收到输入事件: ${JSON.stringify(eventData)}`);
  
  if (!udpClient) {
    console.log('[调试] UDP客户端未初始化，正在初始�?...');
    udpClient = initUDPClient();
    // 如果初始化失败，直接返回
    if (!udpClient) {
      console.log('[调试] UDP客户端初始化失败，无法发送事�?');
      return { success: false, error: 'UDP客户端初始化失败' };
    }
  }
  
  // 检查UDP客户端是否已绑定
  try {
    const address = udpClient.address();
    if (!address) {
      console.log('[调试] UDP客户端未绑定，跳过发�?');
      return { success: false, error: 'UDP客户端未绑定' };
    }
  } catch (error) {
    console.log(`[调试] UDP客户端状态检查失�?: ${error.message}`);
    return { success: false, error: 'UDP客户端状态异�?' };
  }
  
  // 根据事件类型生成不同的UDP�?
  let packet = null;
  let description = '';
  const userId = 1001; // 固定用户ID，与udp_test_client保持一�?
  
  switch(eventData.type) {
    case 'mousemove':
      packet = serializePacket(userId, 1, 0, 0, eventData.x, eventData.y);
      description = `鼠标移动 (${eventData.x}, ${eventData.y})`;
      break;
    case 'mousedown':
      packet = serializePacket(userId, 2, eventData.button || 1, 0, eventData.x, eventData.y);
      description = `鼠标按下 按钮${eventData.button || 1}`;
      break;
    case 'mouseup':
      packet = serializePacket(userId, 3, eventData.button || 1, 0, eventData.x, eventData.y);
      description = `鼠标释放 按钮${eventData.button || 1}`;
      break;
    case 'click':
      packet = serializePacket(userId, 2, eventData.button || 1, 0, eventData.x, eventData.y);
      description = `鼠标点击 按钮${eventData.button || 1}`;
      break;
    case 'keydown':
      // 将按键转换为键码
      const keyCode = getKeyCode(eventData.key);
      packet = serializePacket(userId, 4, 0, keyCode);
      description = `按键按下 键码${keyCode} (${eventData.key})`;
      break;
    case 'keyup':
      const keyCodeUp = getKeyCode(eventData.key);
      packet = serializePacket(userId, 5, 0, keyCodeUp);
      description = `按键释放 键码${keyCodeUp} (${eventData.key})`;
      break;
    case 'keypress':
      const keyCodePress = getKeyCode(eventData.key);
      packet = serializePacket(userId, 4, 0, keyCodePress);
      description = `按键按下 键码${keyCodePress} (${eventData.key})`;
      break;
    default:
      console.log(`[调试] 未知事件类型: ${eventData.type}`);
      return { success: false, error: `未知事件类型: ${eventData.type}` };
  }
  
  console.log(`[调试] 生成的UDP�?: ${description}`);
  
  return new Promise((resolve) => {
    udpClient.send(packet, UDP_TARGET_PORT, UDP_TARGET_HOST, (err) => {
      if (err) {
        console.error(`[错误] 发送UDP包失�?: ${err}`);
        console.error(`[错误详情] ${err.stack}`);
        resolve({ success: false, error: err.message });
      } else {
        console.log(`[UDP发送成功] ${description}`);
        console.log(`[UDP目标] ${UDP_TARGET_HOST}:${UDP_TARGET_PORT}`);
        console.log(`[包大小] ${packet.length}字节`);
        resolve({ success: true });
      }
    });
  });
}

// 将按键字符转换为键码
function getKeyCode(key) {
  const keyMap = {
    'a': 65, 'A': 65,
    'b': 66, 'B': 66,
    'c': 67, 'C': 67,
    'd': 68, 'D': 68,
    'e': 69, 'E': 69,
    'f': 70, 'F': 70,
    'g': 71, 'G': 71,
    'h': 72, 'H': 72,
    'i': 73, 'I': 73,
    'j': 74, 'J': 74,
    'k': 75, 'K': 75,
    'l': 76, 'L': 76,
    'm': 77, 'M': 77,
    'n': 78, 'N': 78,
    'o': 79, 'O': 79,
    'p': 80, 'P': 80,
    'q': 81, 'Q': 81,
    'r': 82, 'R': 82,
    's': 83, 'S': 83,
    't': 84, 'T': 84,
    'u': 85, 'U': 85,
    'v': 86, 'V': 86,
    'w': 87, 'W': 87,
    'x': 88, 'X': 88,
    'y': 89, 'Y': 89,
    'z': 90, 'Z': 90,
    ' ': 32, // 空格
    'Enter': 13,
    'Escape': 27,
    'Tab': 9,
    'Backspace': 8,
    'ArrowUp': 38,
    'ArrowDown': 40,
    'ArrowLeft': 37,
    'ArrowRight': 39
  };
  
  return keyMap[key] || key.charCodeAt(0) || 0;
}

// 当Electron完成初始化并准备创建浏览器窗口时调用此方�?
app.whenReady().then(() => {
  createWindow();
  
  // 如果只需要作为UDP客户端发送消息，注释掉下面这�?
  // initUDPServer();
  
  // 立即初始化UDP客户�?
  console.log('正在初始化UDP客户�?...');
  initUDPClient();
});

// 当所有窗口关闭时退出应�?
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('second-instance', () => {
  const allWindows = BrowserWindow.getAllWindows();
  if (allWindows.length) {
    // Focus on the main window if the user tried to open another
    const mainWindow = allWindows[0];
    if (mainWindow.isMinimized()) mainWindow.restore();
    mainWindow.focus();
  }
});

app.on('activate', () => {
  const allWindows = BrowserWindow.getAllWindows();
  if (allWindows.length) {
    allWindows[0].focus();
  } else {
    createWindow();
  }
});

// New window example arg: new windows url
ipcMain.handle('open-win', (_, arg) => {
  const childWindow = new BrowserWindow({
    webPreferences: {
      preload: path.join(__dirname, '../preload/preload.mjs'),
      nodeIntegration: true,
      contextIsolation: false,
    },
  });

  if (VITE_DEV_SERVER_URL) {
    childWindow.loadURL(`${VITE_DEV_SERVER_URL}#${arg}`);
  } else {
    childWindow.loadFile(path.join(RENDERER_DIST, 'index.html'), { hash: arg });
  }
});
