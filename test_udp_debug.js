// UDP调试测试脚本
import dgram from 'dgram';

// UDP服务器配置
const UDP_PORT = 8888;
const UDP_HOST = '127.0.0.1';

// 创建UDP服务器
const server = dgram.createSocket('udp4');

server.on('error', (err) => {
  console.error(`UDP服务器错误: ${err.stack}`);
  server.close();
});

server.on('message', (msg, rinfo) => {
  const message = msg.toString('utf8').trim();
  console.log(`[UDP服务器收到] 来自 ${rinfo.address}:${rinfo.port}: ${message}`);
});

server.on('listening', () => {
  const address = server.address();
  console.log(`UDP服务器监听 ${address.address}:${address.port}`);
});

server.bind(UDP_PORT, UDP_HOST);

// 创建UDP客户端
const client = dgram.createSocket('udp4');

client.on('error', (err) => {
  console.error(`UDP客户端错误: ${err.stack}`);
});

client.on('listening', () => {
  const address = client.address();
  console.log(`UDP客户端已就绪 ${address.address}:${address.port}`);
});

client.bind();

// 测试发送UDP消息
function sendTestMessage(message) {
  const msgBuffer = Buffer.from(message, 'utf8');
  
  client.send(msgBuffer, UDP_PORT, UDP_HOST, (err) => {
    if (err) {
      console.error(`发送UDP消息失败: ${err}`);
    } else {
      console.log(`[UDP客户端发送] ${message}`);
    }
  });
}

// 延迟发送测试消息
setTimeout(() => {
  console.log('\n=== 开始UDP测试 ===');
  sendTestMessage('test_mouse_move:100,200');
  sendTestMessage('test_mouse_click:0,150,250');
  sendTestMessage('test_key_press:Enter');
}, 1000);

// 10秒后关闭
setTimeout(() => {
  console.log('\n=== 测试完成，关闭UDP连接 ===');
  server.close();
  client.close();
  process.exit(0);
}, 10000);
