import React from 'react';
import ReactDOM from 'react-dom/client';
import App from '@/components/myapp'

// 创建React根节点并渲染应用
const root = ReactDOM.createRoot(document.getElementById('root'));
root.render(
  <React.StrictMode>
    <App />
  </React.StrictMode>
);