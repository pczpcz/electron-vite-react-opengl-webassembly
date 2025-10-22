const dgram = require('dgram');

// UDP服务器配置
const UDP_PORT = 8888;
const UDP_HOST = '127.0.0.1';

// 创建UDP服务器来接收鼠标键盘事件
const server = dgram.createSocket('udp4');

server.on('error', (err) => {
    console.error(`UDP服务器错误: ${err.stack}`);
    server.close();
});

server.on('message', (msg, rinfo) => {
    const message = msg.toString().trim();
    console.log(`[UDP接收] 来自 ${rinfo.address}:${rinfo.port}: ${message}`);
    
    // 解析并显示输入事件
    parseInputEvent(message);
});

server.on('listening', () => {
    const address = server.address();
    console.log(`UDP输入事件监听器启动 ${address.address}:${address.port}`);
    console.log('等待Electron应用发送鼠标键盘事件...');
    console.log('==========================================');
});

server.bind(UDP_PORT, UDP_HOST);

// 解析输入事件
function parseInputEvent(command) {
    const parts = command.split(':');
    const eventType = parts[0];
    const data = parts[1];
    
    switch(eventType) {
        case 'mouse_move':
            const [x, y] = data.split(',');
            console.log(`🐭 鼠标移动: (${x}, ${y})`);
            break;
            
        case 'mouse_down':
            const [btn1, x1, y1] = data.split(',');
            const buttonName = getMouseButtonName(btn1);
            console.log(`🖱️ 鼠标按下: ${buttonName} 在 (${x1}, ${y1})`);
            break;
            
        case 'mouse_up':
            const [btn2, x2, y2] = data.split(',');
            const buttonName2 = getMouseButtonName(btn2);
            console.log(`🖱️ 鼠标释放: ${buttonName2} 在 (${x2}, ${y2})`);
            break;
            
        case 'mouse_click':
            const [btn3, x3, y3] = data.split(',');
            const buttonName3 = getMouseButtonName(btn3);
            console.log(`🖱️ 鼠标点击: ${buttonName3} 在 (${x3}, ${y3})`);
            break;
            
        case 'key_down':
            console.log(`⌨️ 按键按下: ${data}`);
            break;
            
        case 'key_up':
            console.log(`⌨️ 按键释放: ${data}`);
            break;
            
        case 'key_press':
            console.log(`⌨️ 按键输入: ${data}`);
            break;
            
        default:
            console.log(`📝 输入事件: ${command}`);
    }
}

// 获取鼠标按钮名称
function getMouseButtonName(buttonCode) {
    switch(buttonCode) {
        case '0': return '左键';
        case '1': return '中键';
        case '2': return '右键';
        default: return `按钮${buttonCode}`;
    }
}

// 优雅关闭
process.on('SIGINT', () => {
    console.log('\n正在关闭UDP输入事件监听器...');
    server.close();
    process.exit();
});

console.log('UDP输入事件监听器启动中...');
