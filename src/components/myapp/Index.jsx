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
            </div>
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
