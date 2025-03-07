import React, { useState } from "react";
import { useNavigate } from "react-router-dom";
import { MenuFoldOutlined, MenuUnfoldOutlined } from "@ant-design/icons";
import { Button, Layout, Menu, theme, Alert } from "antd";
import Marquee from "react-fast-marquee";

import "../styles/DashBoard.css";
import RobotContent from "./RobotContent";

const { Header, Sider } = Layout;

export default function DashBoard() {
  const userData = JSON.parse(localStorage.getItem("userData"));

  const [collapsed, setCollapsed] = useState(false);
  const {
    token: { colorBgContainer },
  } = theme.useToken();
  const navigate = useNavigate();
  const handleMenuClick = (key) => {
    if (key === "2") {
      // 在这里可以添加清除用户数据的逻辑
      localStorage.removeItem("userData");
      localStorage.removeItem("longTime");
      // 导航到登录页面
      navigate("/");
    }
  };
  return (
    <Layout style={{ height: "100vh" }}>
      <Sider trigger={null} collapsible collapsed={collapsed} className="sider">
        <div
          className="logo-container"
          style={{ padding: "16px", textAlign: "center" }}
        >
          <img
            src={require("../assets/logo.png")} // 确保路径正确
            alt="Logo"
            style={{ width: "100%", height: "auto", maxWidth: "150px" }} // 可以根据需要调整大小
          />
        </div>
        <Menu
          theme="dark"
          mode="inline"
          defaultSelectedKeys={["1"]}
          onClick={({ key }) => handleMenuClick(key)}
          items={[
            {
              key: "1",
              icon: (
                <img
                  src={require("../assets/robot2.png")}
                  style={{ width: "20px", height: "20px" }}
                />
              ),
              label: "我的机器人",
            },
            {
              key: "2",
              icon: (
                <img
                  src={require("../assets/exit.png")}
                  style={{ width: "20px", height: "20px" }}
                />
              ),
              label: "退出登录",
            },
          ]}
          style={{ backgroundColor: "#000000" }}
        />
      </Sider>
      <Layout>
        <Header
          style={{
            padding: 0,
            background: colorBgContainer,
            display: "flex",
            alignItems: "center",
          }}
        >
          <Button
            type="text"
            icon={collapsed ? <MenuUnfoldOutlined /> : <MenuFoldOutlined />}
            onClick={() => setCollapsed(!collapsed)}
            style={{
              fontSize: "16px",
              width: 64,
              height: 64,
            }}
          />
          <h3
            style={{
              margin: 0,
              marginLeft: "16px",
              color: "#595959",
              display: "flex", // 使用 Flexbox 布局
              alignItems: "center", // 垂直居中对齐
              whiteSpace: "nowrap", // 防止文本换行
            }}
          >
            用户ID：{userData.userid}
            <span style={{ marginLeft: "30px" }}>
              用户名：{userData.username}
            </span>
          </h3>

          <Alert
            style={{ marginLeft: "30px" }}
            banner
            message={
              <Marquee pauseOnHover gradient={false}>
                欢迎登录EMP，如果有任何疑问请反馈给管理员邮箱hongkun.li@galaxea.ai
              </Marquee>
            }
          />
        </Header>

        <RobotContent userid={userData.userid} />
      </Layout>
    </Layout>
  );
}
