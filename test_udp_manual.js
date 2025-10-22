// UDP手动测试脚本
import dgram from 'dgram';

console.log('=== UDP手动测试开始 ===');

// 测试UDP客户端初始化
function testUDPClient() {
    console.log('1. 创建UDP客户端...');
    const client = dgram.createSocket('udp4');
    
    client.on('error', (err) => {
        console.error(`UDP客户端错误: ${err.stack}`);
    });
    
    client.on('listening', () => {
        const address = client.address();
        console.log(`2. UDP客户端已就绪 ${address.address}:${address.port}`);
    });
    
    // 绑定到随机端口
    client.bind(0, '127.0.0.1', () => {
        console.log('3. UDP客户端绑定完成');
        
        // 测试发送消息
        const message = Buffer.from('测试消息');
        client.send(message, 8888, '127.0.0.1', (err) => {
            if (err) {
                console.error(`4. 发送失败: ${err}`);
            } else {
                console.log('4. 发送成功');
            }
            
            // 关闭客户端
            client.close();
            console.log('5. UDP客户端已关闭');
            console.log('=== UDP手动测试结束 ===');
        });
    });
}

// 运行测试
testUDPClient();
