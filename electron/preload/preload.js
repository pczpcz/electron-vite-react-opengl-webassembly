import { contextBridge, ipcRenderer } from 'electron';

// 向渲染进程暴露安全的API
contextBridge.exposeInMainWorld('electronAPI', {
  // 向主进程发送状态更新
  sendStatusUpdate: (status) => 
    ipcRenderer.invoke('renderer-status-update', status),
  
  // 向主进程发送错误信息
  sendError: (error) => 
    ipcRenderer.invoke('renderer-error', error),
  
  // 监听主进程消息
  onMessage: (callback) => 
    ipcRenderer.on('message', callback),
  
  // 移除监听器
  removeAllListeners: (channel) => 
    ipcRenderer.removeAllListeners(channel)
});
