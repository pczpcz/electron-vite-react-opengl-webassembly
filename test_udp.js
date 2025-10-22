import dgram from 'dgram';

// UDP客户端配置
const UDP_PORT = 8888;
const UDP_HOST = '127.0.0.1';

// 创建UDP客户端
const client = dgram.createSocket('udp4');

// 测试命令列表
const testCommands = [
    'cmd1',
    'cmd2', 
    'cmd3',
    'cmd4',
    'status',
    'unknown_command'
];

let currentIndex = 0;

// 发送测试命令
function sendTestCommand() {
    if (currentIndex >= testCommands.length) {
        console.log('所有测试命令发送完成');
        client.close();
        return;
    }
    
    const command = testCommands[currentIndex];
    const message = Buffer.from(command);
    
    client.send(message, UDP_PORT, UDP_HOST, (err) => {
        if (err) {
            console.error(`发送命令失败: ${err}`);
        } else {
            console.log(`发送命令: ${command}`);
        }
    });
    
    currentIndex++;
    
    // 每隔2秒发送下一个命令
    setTimeout(sendTestCommand, 2000);
}

console.log('开始UDP命令测试...');
console.log('可用命令:');
console.log('- cmd1: 切换背景颜色');
console.log('- cmd2: 开始动画');
console.log('- cmd3: 停止动画');
console.log('- cmd4: 重置动画');
console.log('- status: 获取状态');
console.log('');

// 开始测试
sendTestCommand();

// 处理错误
client.on('error', (err) => {
    console.error(`UDP客户端错误: ${err}`);
    client.close();
});
