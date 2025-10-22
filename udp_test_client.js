/**
 * UDP测试客户端 - 用于测试游戏服务器UDP通信
 * 可以直接复制到其他Node.js项目中使用
 */

const dgram = require('dgram');
const readline = require('readline');

class UdpTestClient {
    constructor(host = 'localhost', port = 8888) {
        this.host = host;
        this.port = port;
        this.client = dgram.createSocket('udp4');
        this.userId = Math.floor(Math.random() * 10000) + 1;
        this.isConnected = false;
        
        this.setupEventListeners();
        this.setupInputHandler();
    }

    setupEventListeners() {
        // 监听消息接收
        this.client.on('message', (msg, rinfo) => {
            console.log(`[接收] 来自 ${rinfo.address}:${rinfo.port}: ${msg.toString()}`);
        });

        // 监听连接成功
        this.client.on('listening', () => {
            const address = this.client.address();
            console.log(`[连接] UDP客户端已启动，监听 ${address.address}:${address.port}`);
            this.isConnected = true;
        });

        // 监听错误
        this.client.on('error', (err) => {
            console.error(`[错误] UDP客户端错误: ${err.message}`);
            this.isConnected = false;
        });

        // 监听关闭
        this.client.on('close', () => {
            console.log('[关闭] UDP客户端已关闭');
            this.isConnected = false;
        });
    }

    setupInputHandler() {
        const rl = readline.createInterface({
            input: process.stdin,
            output: process.stdout
        });

        console.log('\n=== UDP测试客户端 ===');
        console.log('命令列表:');
        console.log('  move x y     - 发送鼠标移动 (例如: move 100 200)');
        console.log('  click btn    - 发送鼠标点击 (例如: click 1)');
        console.log('  release btn  - 发送鼠标释放 (例如: release 1)');
        console.log('  press key    - 发送按键按下 (例如: press 65)');
        console.log('  release key  - 发送按键释放 (例如: release 65)');
        console.log('  custom       - 发送自定义数据包');
        console.log('  status       - 显示连接状态');
        console.log('  quit         - 退出程序');
        console.log('================================\n');

        rl.on('line', (input) => {
            this.handleCommand(input.trim());
        });

        rl.on('close', () => {
            console.log('\n再见!');
            this.close();
        });
    }

    handleCommand(input) {
        const parts = input.split(' ');
        const command = parts[0].toLowerCase();

        switch (command) {
            case 'move':
                if (parts.length === 3) {
                    this.sendMouseMove(parseInt(parts[1]), parseInt(parts[2]));
                } else {
                    console.log('用法: move x y');
                }
                break;

            case 'click':
                if (parts.length === 2) {
                    this.sendMouseClick(parseInt(parts[1]));
                } else {
                    console.log('用法: click button (1=左键, 2=右键, 3=中键)');
                }
                break;

            case 'release':
                if (parts.length === 2) {
                    this.sendMouseRelease(parseInt(parts[1]));
                } else {
                    console.log('用法: release button (1=左键, 2=右键, 3=中键)');
                }
                break;

            case 'press':
                if (parts.length === 2) {
                    this.sendKeyPress(parseInt(parts[1]));
                } else {
                    console.log('用法: press keyCode');
                }
                break;

            case 'releasekey':
                if (parts.length === 2) {
                    this.sendKeyRelease(parseInt(parts[1]));
                } else {
                    console.log('用法: releasekey keyCode');
                }
                break;

            case 'custom':
                this.sendCustomPacket();
                break;

            case 'status':
                this.showStatus();
                break;

            case 'quit':
            case 'exit':
                this.close();
                process.exit(0);
                break;

            default:
                console.log('未知命令，请输入 help 查看可用命令');
                break;
        }
    }

    // 序列化数据包 (与C++服务器端格式匹配)
    serializePacket(userId, opType, mouseButton = 0, keyCode = 0, mouseX = 0, mouseY = 0) {
        const buffer = Buffer.alloc(20); // 20字节固定大小
        
        // userId (4字节)
        buffer.writeUInt32BE(userId, 0);
        
        // opType (1字节)
        buffer.writeUInt8(opType, 4);
        
        // mouseButton (1字节)
        buffer.writeUInt8(mouseButton, 5);
        
        // keyCode (2字节)
        buffer.writeUInt16BE(keyCode, 6);
        
        // mouseX (2字节)
        buffer.writeInt16BE(mouseX, 8);
        
        // mouseY (2字节)
        buffer.writeInt16BE(mouseY, 10);
        
        // timestamp (8字节)
        const timestamp = BigInt(Date.now());
        for (let i = 0; i < 8; i++) {
            buffer.writeUInt8(Number((timestamp >> BigInt(8 * (7 - i))) & 0xFFn), 12 + i);
        }
        
        return buffer;
    }

    sendMouseMove(x, y) {
        const packet = this.serializePacket(this.userId, 1, 0, 0, x, y);
        this.sendPacket(packet, `鼠标移动 (${x}, ${y})`);
    }

    sendMouseClick(button) {
        const packet = this.serializePacket(this.userId, 2, button, 0, 100, 100);
        this.sendPacket(packet, `鼠标点击 按钮${button}`);
    }

    sendMouseRelease(button) {
        const packet = this.serializePacket(this.userId, 3, button, 0, 100, 100);
        this.sendPacket(packet, `鼠标释放 按钮${button}`);
    }

    sendKeyPress(keyCode) {
        const packet = this.serializePacket(this.userId, 4, 0, keyCode);
        this.sendPacket(packet, `按键按下 键码${keyCode}`);
    }

    sendKeyRelease(keyCode) {
        const packet = this.serializePacket(this.userId, 5, 0, keyCode);
        this.sendPacket(packet, `按键释放 键码${keyCode}`);
    }

    sendCustomPacket() {
        // 发送一个自定义的测试数据包
        const packet = this.serializePacket(this.userId, 1, 0, 0, 500, 300);
        this.sendPacket(packet, '自定义测试数据包');
    }

    sendPacket(packet, description) {
        if (!this.isConnected) {
            console.log('[错误] 客户端未连接，请先启动');
            return;
        }

        this.client.send(packet, this.port, this.host, (err) => {
            if (err) {
                console.error(`[发送失败] ${description}: ${err.message}`);
            } else {
                console.log(`[发送成功] ${description} (用户ID: ${this.userId})`);
            }
        });
    }

    showStatus() {
        console.log('\n=== 客户端状态 ===');
        console.log(`服务器: ${this.host}:${this.port}`);
        console.log(`用户ID: ${this.userId}`);
        console.log(`连接状态: ${this.isConnected ? '已连接' : '未连接'}`);
        console.log('==================\n');
    }

    connect() {
        this.client.bind(() => {
            console.log(`[连接] 正在连接到 ${this.host}:${this.port}`);
        });
    }

    close() {
        if (this.client) {
            this.client.close();
        }
    }

    // 自动测试功能
    autoTest() {
        console.log('\n=== 开始自动测试 ===');
        
        const tests = [
            () => this.sendMouseMove(100, 200),
            () => this.sendMouseClick(1),
            () => this.sendMouseRelease(1),
            () => this.sendKeyPress(65), // A键
            () => this.sendKeyRelease(65)
        ];

        let index = 0;
        const interval = setInterval(() => {
            if (index < tests.length) {
                tests[index]();
                index++;
            } else {
                clearInterval(interval);
                console.log('=== 自动测试完成 ===');
            }
        }, 1000);
    }
}

// 使用示例
if (require.main === module) {
    const client = new UdpTestClient();
    client.connect();

    // 可选：5秒后开始自动测试
    setTimeout(() => {
        console.log('\n开始自动测试演示...');
        client.autoTest();
    }, 5000);
}

module.exports = UdpTestClient;
