import { ipcRenderer } from 'electron';
import fs from 'fs';
import path from 'path';

// 由于 contextIsolation: false 和 nodeIntegration: true，
// 渲染进程可以直接访问 Node.js API，不需要使用 contextBridge

// 直接挂载到全局对象，供渲染进程使用
window.electronAPI = {
  // 向主进程发送状态更新
  sendStatusUpdate: (status) => 
    ipcRenderer.invoke('renderer-status-update', status),
  
  // 向主进程发送错误信息
  sendError: (error) => 
    ipcRenderer.invoke('renderer-error', error),
  
  // 监听主进程消息
  onMessage: (callback) => 
    ipcRenderer.on('message', callback),
  
  // 监听UDP命令
  onUDPCommand: (callback) => 
    ipcRenderer.on('udp-command', callback),
  
  // 移除监听器
  removeAllListeners: (channel) => 
    ipcRenderer.removeAllListeners(channel),
  
  // 文件系统API
  readFile: (filePath) => {
    return new Promise((resolve, reject) => {
      fs.readFile(filePath, (err, data) => {
        if (err) {
          reject(err);
        } else {
          resolve(data);
        }
      });
    });
  },
  
  // 读取二进制文件
  readBinaryFile: (filePath) => {
    return new Promise((resolve, reject) => {
      fs.readFile(filePath, (err, data) => {
        if (err) {
          reject(err);
        } else {
          // 转换为Uint8Array
          const uint8Array = new Uint8Array(data);
          resolve(uint8Array);
        }
      });
    });
  },
  
  // 检查文件是否存在
  fileExists: (filePath) => {
    return new Promise((resolve) => {
      fs.access(filePath, fs.constants.F_OK, (err) => {
        resolve(!err);
      });
    });
  },
  
  // 获取项目根目录
  getProjectRoot: () => {
    return process.cwd();
  },
  
  // 发送鼠标键盘事件到主进程
  sendInputEvent: (eventData) => 
    ipcRenderer.invoke('renderer-input-event', eventData),
  
  // 监听UDP状态更新
  onUDPStatus: (callback) => 
    ipcRenderer.on('udp-status', callback)
};

// 由于 nodeIntegration: true，渲染进程也可以直接访问 Node.js API
console.log('Preload script loaded with direct Node.js API access');
