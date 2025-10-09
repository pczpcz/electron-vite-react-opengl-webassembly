import React, { useRef, useEffect } from 'react';
import './App.css';
import wasmUrl from './build_emscripten/OpenglWebTest.wasm?url';
import wasmJsUrl from './build_emscripten/OpenglWebTest.js?url';

// 主应用组件
/**
 * 主应用组件，负责管理整个应用的状态和组件渲染
 * 包含WebAssembly加载、SDL渲染器集成和控制面板等功能
 */
function App() {
    // 使用useRef来引用canvas元素
    const canvasRef = useRef(null);
    
    // 使用useEffect动态加载OpenglWebTest.js并配置Module
    useEffect(() => {
        if (canvasRef.current) {
            console.log('Setting up Module configuration...');
            
            // 检查是否已经加载过WASM脚本
            if (document.querySelector(`script[src="${wasmJsUrl}"]`)) {
                console.log('WASM script already loaded, skipping...');
                return;
            }
            
            // 创建script元素来设置Module配置
            const configScript = document.createElement('script');
            configScript.innerHTML = `
                console.log('Setting up Module configuration...');
                var canv = document.getElementById('canvas');
                console.log('Canvas element found:', canv);
                var Module = {
                    canvas: canv,
                    locateFile: function(path, prefix) {
                        if (path.endsWith('.wasm')) {
                            return '${wasmUrl}';
                        }
                        return prefix + path;
                    }
                };
                console.log('Module configured with canvas:', Module.canvas);
            `;
            document.body.appendChild(configScript);
            
            // 动态创建script标签来加载OpenglWebTest.js
            const wasmScript = document.createElement('script');
            wasmScript.src = wasmJsUrl;
            wasmScript.async = true;
            wasmScript.onload = () => {
                console.log('OpenglWebTest.js loaded successfully');
                
                // 创建script元素来添加事件监听器
                const eventScript = document.createElement('script');
                eventScript.innerHTML = `
                    console.log('Adding event listeners...');
                    var canv = document.getElementById('canvas');
                    canv.addEventListener('click', function() {
                        if (typeof _toggle_background_color !== 'undefined') {
                            _toggle_background_color();
                            console.log('_toggle_background_color called');
                        } else {
                            console.log('_toggle_background_color function not defined yet');
                        }
                    }, false);
                    
                    canv.addEventListener('touchend', function() {
                        if (typeof _toggle_background_color !== 'undefined') {
                            _toggle_background_color();
                            console.log('_toggle_background_color called');
                        } else {
                            console.log('_toggle_background_color function not defined yet');
                        }
                    }, false);
                    console.log('Canvas event listeners added');
                `;
                document.body.appendChild(eventScript);
            };
            wasmScript.onerror = () => {
                console.error('Failed to load OpenglWebTest.js');
            };
            
            document.body.appendChild(wasmScript);
            
            // 清理函数
            return () => {
                if (document.body.contains(configScript)) {
                    document.body.removeChild(configScript);
                }
                // 注意：我们不移除WASM脚本，因为它包含全局状态
            };
        }
    }, []);
    
    return (
        <>
            <h1>ldjlsjlflsj</h1>
            <canvas 
                ref={canvasRef}
                id="canvas"
                onContextMenu={(e) => e.preventDefault()}
            ></canvas>
        </>
    );
}

export default App;
