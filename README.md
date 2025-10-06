# Electron + Vite + React + OpenGL WebAssembly Project

## 📋 Project Overview

This project is a custom implementation based on the **electron-vite-react** template, enhanced with OpenGL WebAssembly capabilities for graphics rendering.

## 🔗 Reference Projects

This project is built upon and inspired by the following open-source projects and resources:

### Primary Reference
- **[electron-vite-react](https://github.com/electron-vite/electron-vite-react)** - The base Electron + Vite + React template
  - Original author: [caoxiemeihao](https://github.com/caoxiemeihao)
  - License: MIT

### Technical References
- **[Electron + Vite + React 开发桌面应用](https://zhuanlan.zhihu.com/p/664016921)** - Development guide for Electron + Vite + React desktop applications
- **[Electron](https://www.electronjs.org/)** - Cross-platform desktop apps
- **[Vite](https://vitejs.dev/)** - Next generation frontend tooling
- **[React](https://reactjs.org/)** - JavaScript library for building user interfaces
- **[OpenGL](https://www.opengl.org/)** via WebAssembly - Graphics rendering

## 🚀 Quick Start

```bash
# Install dependencies
npm install

# Development mode
npm run dev

# Build for production
npm run build

# Build Electron app
npm run build:electron
```

## 🏗️ Project Structure

```
├── electron/                 # Electron main and preload scripts
│   ├── main/                # Main process code
│   └── preload/             # Preload scripts
├── src/                     # React application source
│   ├── components/          # React components
│   │   └── myapp/           # OpenGL WebAssembly components
│   └── assets/              # Static assets
├── public/                  # Public assets including WebAssembly files
└── build/                   # Build configuration and icons
```

## 🎯 Features

- **Cross-platform Desktop App** - Built with Electron
- **Fast Development** - Powered by Vite
- **Modern UI** - React-based interface
- **Graphics Rendering** - OpenGL via WebAssembly
- **Hot Reload** - Instant development feedback
- **TypeScript Support** - Type-safe development

## 📝 License

This project inherits the MIT license from the original electron-vite-react template. Please refer to the original project's license for detailed terms.

## 🤖 Development Tools

This project was developed with the assistance of AI tools for code generation, documentation, and problem-solving.

## 🙏 Acknowledgments

Special thanks to:
- The original authors and maintainers of the electron-vite-react template
- The technical community for sharing knowledge and resources
- AI assistance tools that helped accelerate development

---

*Note: This is a modified version of the original electron-vite-react template with custom OpenGL WebAssembly integration for graphics applications, developed with AI assistance.*
