import React, { useRef, useEffect, useState } from 'react';
import './App.css';
import wasmUrl from './build_emscripten/OpenglWebTest.wasm?url';
import wasmJsUrl from './build_emscripten/OpenglWebTest.js?url';

//import ozzRenderer from './render.js'

// 主应用组件
/**
 * 主应用组件，负责管理整个应用的状态和组件渲染
 * 包含WebAssembly加载、SDL渲染器集成和控制面板等功能
 */
function App() {
    // 使用useRef来引用canvas元素
    const canvasRef = useRef(null);
    const [animationStatus, setAnimationStatus] = useState('Not initialized');
    const [isAnimationRunning, setIsAnimationRunning] = useState(false);
    const [lastUDPCommand, setLastUDPCommand] = useState('');
    const [commandHistory, setCommandHistory] = useState([]);
    const [udpStatus, setUdpStatus] = useState({ type: 'info', message: 'UDP客户端初始化中...' });
    const [udpStatusHistory, setUdpStatusHistory] = useState([]);

    // 监听UDP命令
    useEffect(() => {
        if (window.electronAPI && window.electronAPI.onUDPCommand) {
            const handleUDPCommand = (event, data) => {
                console.log('收到UDP命令:', data);
                setLastUDPCommand(data.command);
                setCommandHistory(prev => [...prev.slice(-9), data]);
                
                // 处理不同的命令
                handleCommand(data.command);
            };
            
            window.electronAPI.onUDPCommand(handleUDPCommand);
            
            // 清理函数
            return () => {
                window.electronAPI.removeAllListeners('udp-command');
            };
        }
    }, []);

    // 监听UDP状态更新
    useEffect(() => {
        if (window.electronAPI && window.electronAPI.onUDPStatus) {
            const handleUDPStatus = (event, status) => {
                console.log('收到UDP状态更新:', status);
                setUdpStatus(status);
                setUdpStatusHistory(prev => [...prev.slice(-4), status]);
            };
            
            window.electronAPI.onUDPStatus(handleUDPStatus);
            
            // 清理函数
            return () => {
                window.electronAPI.removeAllListeners('udp-status');
            };
        }
    }, []);

    // 监听鼠标键盘事件
    useEffect(() => {
        const canvas = canvasRef.current;
        if (!canvas) return;

        // 鼠标事件处理函数
        const handleMouseEvent = (event) => {
            const rect = canvas.getBoundingClientRect();
            const x = event.clientX - rect.left;
            const y = event.clientY - rect.top;
            
            const eventData = {
                type: event.type,
                x: Math.round(x),
                y: Math.round(y),
                button: event.button,
                timestamp: Date.now()
            };
            
            // 发送到主进程
            if (window.electronAPI && window.electronAPI.sendInputEvent) {
                window.electronAPI.sendInputEvent(eventData);
            }
        };

        // 键盘事件处理函数
        const handleKeyboardEvent = (event) => {
            const eventData = {
                type: event.type,
                key: event.key,
                code: event.code,
                keyCode: event.keyCode,
                timestamp: Date.now()
            };
            
            // 发送到主进程
            if (window.electronAPI && window.electronAPI.sendInputEvent) {
                window.electronAPI.sendInputEvent(eventData);
            }
        };

        // 添加鼠标事件监听器
        canvas.addEventListener('mousemove', handleMouseEvent);
        canvas.addEventListener('mousedown', handleMouseEvent);
        canvas.addEventListener('mouseup', handleMouseEvent);
        canvas.addEventListener('click', handleMouseEvent);
        
        // 添加键盘事件监听器（需要canvas获得焦点）
        canvas.addEventListener('keydown', handleKeyboardEvent);
        canvas.addEventListener('keyup', handleKeyboardEvent);
        canvas.addEventListener('keypress', handleKeyboardEvent);
        
        // 设置canvas可聚焦
        canvas.setAttribute('tabindex', '0');
        canvas.style.outline = 'none';

        // 清理函数
        return () => {
            canvas.removeEventListener('mousemove', handleMouseEvent);
            canvas.removeEventListener('mousedown', handleMouseEvent);
            canvas.removeEventListener('mouseup', handleMouseEvent);
            canvas.removeEventListener('click', handleMouseEvent);
            canvas.removeEventListener('keydown', handleKeyboardEvent);
            canvas.removeEventListener('keyup', handleKeyboardEvent);
            canvas.removeEventListener('keypress', handleKeyboardEvent);
        };
    }, []);

    // 处理UDP命令
    const handleCommand = (command) => {
        console.log(`执行命令: ${command}`);
        
        switch(command.toLowerCase()) {
            case 'cmd1':
                // 执行命令1：切换背景颜色
                if (typeof _toggle_background_color !== 'undefined') {
                    _toggle_background_color();
                    console.log('执行cmd1: 切换背景颜色');
                    setAnimationStatus('背景颜色已切换');
                }
                break;
                
            case 'cmd2':
                // 执行命令2：开始动画
                console.log('执行cmd2: 开始动画');
                setAnimationStatus('动画开始');
                setIsAnimationRunning(true);
                break;
                
            case 'cmd3':
                // 执行命令3：停止动画
                console.log('执行cmd3: 停止动画');
                setAnimationStatus('动画停止');
                setIsAnimationRunning(false);
                break;
                
            case 'cmd4':
                // 执行命令4：重置动画
                console.log('执行cmd4: 重置动画');
                setAnimationStatus('动画重置');
                setIsAnimationRunning(false);
                break;
                
            case 'status':
                // 获取状态
                console.log('执行status: 获取状态');
                if (window.electronAPI) {
                    window.electronAPI.sendStatusUpdate(`当前状态: ${animationStatus}, 动画运行: ${isAnimationRunning}`);
                }
                break;
                
            default:
                console.log(`未知命令: ${command}`);
                setAnimationStatus(`未知命令: ${command}`);
                break;
        }
    };

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

                // 添加timeout，等待wasm加载完成
                setTimeout(() => {
                    console.log('WASM loaded, initializing animation...');
                    
                    // 后续考虑共享内存或者postMessage时，使用transfer转移而不是克隆？https://juejin.cn/post/7503847913251356710
                    // 1. 直接使用Node.js读取数据,一段memoryArray（避免从主进程传递过来的数据拷贝开销）
                    // 这里使用fetch API来读取动画数据文件
                    const loadAnimationData = async () => {
                        try {
                            // 读取骨架数据
                            const skeletonResponse = await fetch('/data/media/skeleton.ozz');
                            const skeletonData = await skeletonResponse.arrayBuffer();
                            
                            // 读取动画数据
                            const animationResponse = await fetch('/data/media/animation1.ozz');
                            const animationData = await animationResponse.arrayBuffer();
                            
                            console.log('Animation data loaded:', {
                                skeletonSize: skeletonData.byteLength,
                                animationSize: animationData.byteLength
                            });
                            
                            // 2. 将读取的数据传递给WebAssembly模块
                            let animaInstance = Module._create_ozz_animation();
                            Module._initialize_ozz_animation(animaInstance);
                            
                            // 调用Module.load_animation_from_memory函数加载动画数据
                            // 首先需要将ArrayBuffer转换为Module可用的内存
                            const animationPtr = _malloc(animationData.byteLength);
                            HEAPU8.set(new Uint8Array(animationData), animationPtr);
                            
                            const success = Module._load_animation_from_memory(
                                animaInstance, 
                                animationPtr, 
                                animationData.byteLength, 
                                0 // 动画索引
                            );
                            
                            if (success) {
                                console.log('Animation loaded successfully from memory');
                                setAnimationStatus('Animation loaded from memory');
                                
                                // 测试获取状态
                                const statusPtr = Module._get_ozz_status(animaInstance);
                                const status = UTF8ToString(statusPtr);
                                console.log('Animation status:', status);
                            } else {
                                console.error('Failed to load animation from memory');
                                setAnimationStatus('Failed to load animation');
                            }
                            
                        } catch (error) {
                            console.error('Error loading animation data:', error);
                            setAnimationStatus('Error loading animation data');
                        }
                    };
                    
                    loadAnimationData();
                    
                }, 1000); // 1秒延迟确保WASM完全加载
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
            };
        }
    }, []);
    
    return (
        <>
            <div style={{ position: 'absolute', top: 10, left: 10, background: 'rgba(0,0,0,0.7)', color: 'white', padding: '10px', borderRadius: '5px', zIndex: 1000 }}>
                <h3>Ozz Animation Controls</h3>
                <div>
                    <button 
                        //onClick={initializeOzzAnimation}
                        disabled={isAnimationRunning}
                        style={{ marginRight: '10px' }}
                    >
                        Start Animation
                    </button>
                </div>
                <div style={{ marginTop: '10px', fontSize: '12px' }}>
                    <div>状态: {animationStatus}</div>
                    <div>动画运行: {isAnimationRunning ? '是' : '否'}</div>
                    {lastUDPCommand && (
                        <div>最后命令: {lastUDPCommand}</div>
                    )}
                </div>
            </div>
            
            {/* UDP状态面板 */}
            <div style={{ 
                position: 'absolute', 
                top: 10, 
                right: 10, 
                background: 'rgba(0,0,0,0.7)', 
                color: 'white', 
                padding: '10px', 
                borderRadius: '5px', 
                zIndex: 1000,
                width: '300px'
            }}>
                <h4>UDP通讯状态</h4>
                <div style={{ 
                    padding: '5px', 
                    borderRadius: '3px', 
                    marginBottom: '10px',
                    backgroundColor: udpStatus.type === 'success' ? 'rgba(0,255,0,0.2)' : 
                                   udpStatus.type === 'error' ? 'rgba(255,0,0,0.2)' : 
                                   'rgba(255,255,0,0.2)',
                    border: udpStatus.type === 'success' ? '1px solid #00ff00' : 
                           udpStatus.type === 'error' ? '1px solid #ff0000' : 
                           '1px solid #ffff00'
                }}>
                    <div style={{ fontSize: '12px', fontWeight: 'bold' }}>
                        {udpStatus.type === 'success' ? '✅' : 
                         udpStatus.type === 'error' ? '❌' : 'ℹ️'} 
                        {udpStatus.message}
                    </div>
                    <div style={{ fontSize: '10px', color: '#ccc', marginTop: '2px' }}>
                        更新时间: {new Date().toLocaleTimeString()}
                    </div>
                </div>
                
                {/* UDP状态历史 */}
                {udpStatusHistory.length > 0 && (
                    <div style={{ fontSize: '10px', maxHeight: '150px', overflowY: 'auto' }}>
                        <div style={{ fontWeight: 'bold', marginBottom: '5px' }}>状态历史:</div>
                        {udpStatusHistory.slice().reverse().map((status, index) => (
                            <div key={index} style={{ 
                                marginBottom: '3px', 
                                padding: '2px 5px', 
                                borderRadius: '2px',
                                backgroundColor: status.type === 'success' ? 'rgba(0,255,0,0.1)' : 
                                              status.type === 'error' ? 'rgba(255,0,0,0.1)' : 
                                              'rgba(255,255,0,0.1)',
                                borderLeft: status.type === 'success' ? '2px solid #00ff00' : 
                                          status.type === 'error' ? '2px solid #ff0000' : 
                                          '2px solid #ffff00'
                            }}>
                                <div>{status.message}</div>
                                <div style={{ fontSize: '8px', color: '#999' }}>
                                    {new Date(status.timestamp).toLocaleTimeString()}
                                </div>
                            </div>
                        ))}
                    </div>
                )}
            </div>

            {/* UDP命令历史面板 */}
            {commandHistory.length > 0 && (
                <div style={{ 
                    position: 'absolute', 
                    top: 200, 
                    right: 10, 
                    background: 'rgba(0,0,0,0.7)', 
                    color: 'white', 
                    padding: '10px', 
                    borderRadius: '5px', 
                    zIndex: 1000,
                    maxHeight: '300px',
                    overflowY: 'auto'
                }}>
                    <h4>UDP命令历史</h4>
                    <div style={{ fontSize: '10px' }}>
                        {commandHistory.slice().reverse().map((cmd, index) => (
                            <div key={index} style={{ marginBottom: '5px', borderBottom: '1px solid #444', paddingBottom: '5px' }}>
                                <div>命令: {cmd.command}</div>
                                <div>来源: {cmd.source}</div>
                                <div>时间: {new Date(cmd.timestamp).toLocaleTimeString()}</div>
                            </div>
                        ))}
                    </div>
                </div>
            )}
            <canvas 
                ref={canvasRef}
                id="canvas"
                onContextMenu={(e) => e.preventDefault()}
                style={{ width: '100%', height: '100%', display: 'block' }}
            ></canvas>
        </>
    );
}

export default App;
