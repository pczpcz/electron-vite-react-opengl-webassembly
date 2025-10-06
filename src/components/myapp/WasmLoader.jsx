import React, { useState, useEffect } from 'react';

// WebAssembly加载器组件
function WasmLoader({ onWasmLoaded, onWasmError }) {
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  const [wasmModule, setWasmModule] = useState(null);

  useEffect(() => {
    const loadWasmModule = async () => {
      try {
        setLoading(true);
        setError(null);

        // 通知主进程开始加载
        if (window.electronAPI) {
          await window.electronAPI.sendStatusUpdate('开始加载WebAssembly模块');
        }

        // 使用script标签加载WebAssembly模块
        // Vite不允许从public目录动态导入，所以使用传统script标签方式
        const wasmPath = '/OpenglWebTest.js';
        
        console.log('Loading WebAssembly from:', wasmPath);
        
        // 创建script标签加载WebAssembly模块
        const module = await new Promise((resolve, reject) => {
          const script = document.createElement('script');
          script.src = wasmPath;
          script.onload = () => {
            // Emscripten模块通常会在全局暴露Module对象
            if (window.Module) {
              // 等待模块初始化完成
              if (window.Module.calledRun) {
                resolve(window.Module);
              } else {
                const originalOnRuntimeInitialized = window.Module.onRuntimeInitialized;
                window.Module.onRuntimeInitialized = () => {
                  if (originalOnRuntimeInitialized) {
                    originalOnRuntimeInitialized();
                  }
                  resolve(window.Module);
                };
              }
            } else {
              reject(new Error('WebAssembly module not found in global scope'));
            }
          };
          script.onerror = () => reject(new Error(`Failed to load WebAssembly script: ${wasmPath}`));
          document.head.appendChild(script);
        });

        console.log('WebAssembly模块在渲染进程中加载完成');
        
        if (window.electronAPI) {
          await window.electronAPI.sendStatusUpdate('WebAssembly模块加载完成');
        }

        setWasmModule(module);
        setLoading(false);
        
        if (onWasmLoaded) {
          onWasmLoaded(module);
        }

      } catch (err) {
        console.error('加载WebAssembly模块时出错:', err);
        const errorMsg = `加载WebAssembly模块失败: ${err.message}`;
        setError(errorMsg);
        setLoading(false);
        
        if (window.electronAPI) {
          await window.electronAPI.sendError(errorMsg);
        }
        
        if (onWasmError) {
          onWasmError(err);
        }
      }
    };

    loadWasmModule();
  }, [onWasmLoaded, onWasmError]);

  if (loading) {
    return (
      <div className="wasm-loader">
        <div className="loading-spinner"></div>
        <p>正在加载WebAssembly模块...</p>
      </div>
    );
  }

  if (error) {
    return (
      <div className="wasm-error">
        <h3>WebAssembly加载失败</h3>
        <p>{error}</p>
        <button onClick={() => window.location.reload()}>重新加载</button>
      </div>
    );
  }

  return (
    <div className="wasm-loaded">
      <p>WebAssembly模块已加载</p>
    </div>
  );
}

export default WasmLoader;
