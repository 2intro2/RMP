import React from "react";
import { BrowserRouter as Router, Routes, Route } from "react-router-dom";
import Login from "./components/Login"; // 导入你的 Login 组件
import DashBoard from "./components/DashBoard"; // 导入你的 Dashboard 组件

const App = () => {
  return (
    <Router>
      <Routes>
        <Route path="/" element={<Login />} />
        <Route path="/dashboard" element={<DashBoard />} />
      </Routes>
    </Router>
  );
};

export default App;
