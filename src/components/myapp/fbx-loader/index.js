const { loadFBXFile, createSharedMemory, getSharedMemoryData } = require('./build/Release/fbx_loader.node');

class FBXLoader {
  constructor() {
    this.sharedMemoryId = null;
  }

  /**
   * 加载FBX文件并创建共享内存
   * @param {string} filePath FBX文件路径
   * @returns {Promise<Object>} 包含共享内存ID和元数据的对象
   */
  async loadFBX(filePath) {
    try {
      const result = loadFBXFile(filePath);
      this.sharedMemoryId = result.sharedMemoryId;
      
      console.log(`FBX文件加载成功: ${filePath}`);
      console.log(`共享内存ID: ${this.sharedMemoryId}`);
      console.log(`顶点数量: ${result.metadata.vertexCount}`);
      console.log(`三角形数量: ${result.metadata.triangleCount}`);
      
      return result;
    } catch (error) {
      console.error(`FBX文件加载失败: ${error.message}`);
      throw error;
    }
  }

  /**
   * 从共享内存获取数据
   * @param {string} sharedMemoryId 共享内存ID
   * @returns {Promise<Object>} 解析后的FBX数据
   */
  async getFBXData(sharedMemoryId) {
    try {
      const data = getSharedMemoryData(sharedMemoryId);
      return data;
    } catch (error) {
      console.error(`从共享内存获取数据失败: ${error.message}`);
      throw error;
    }
  }

  /**
   * 创建空的共享内存区域
   * @param {number} size 内存大小（字节）
   * @returns {string} 共享内存ID
   */
  async createSharedMemory(size) {
    try {
      const sharedMemoryId = createSharedMemory(size);
      console.log(`创建共享内存成功，ID: ${sharedMemoryId}, 大小: ${size}字节`);
      return sharedMemoryId;
    } catch (error) {
      console.error(`创建共享内存失败: ${error.message}`);
      throw error;
    }
  }

  /**
   * 获取当前共享内存ID
   * @returns {string|null} 共享内存ID
   */
  getSharedMemoryId() {
    return this.sharedMemoryId;
  }
}

module.exports = {
  FBXLoader,
  loadFBXFile,
  createSharedMemory,
  getSharedMemoryData
};
