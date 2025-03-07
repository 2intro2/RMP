import React, { useEffect, useState } from "react";
import { LockOutlined, UserOutlined } from "@ant-design/icons";
import { useNavigate } from "react-router-dom";
import { Button, Checkbox, Form, Input, message } from "antd";
import axios from "axios";
import logo from "../assets/logo.png";

const Login = () => {
  const [loading, setLoading] = useState(false);
  const navigate = useNavigate();

  // 检查是否已经登录
  useEffect(() => {
    const userData = localStorage.getItem("userData");
    const loginTime = localStorage.getItem("loginTime");

    if (userData) {
      const now = Date.now();
      if (now - loginTime < 24 * 60 * 60 * 1000) {
        // 检查是否在一天之内
        navigate("/dashboard"); // 如果在一天内，自动跳转到导航页面
      }
    }
  }, [navigate]);

  const onFinish = async (values) => {
    setLoading(true);
    try {
      const response = await axios.post("http://192.168.247.128:8080/login", {
        userid: values.userid,
        password: values.password,
      });

      const { data: res } = response;

      if (res.code === 0) {
        const userData = { userid: res.userid, username: res.username };

        // 只有当用户选择“记住我”时才在浏览器保存用户信息
        if (values.remember) {
          localStorage.setItem("userData", JSON.stringify(userData));
          localStorage.setItem("loginTime", Date.now()); // 保存登录时间戳
        } else {
          sessionStorage.setItem("userData", JSON.stringify(userData));
        }

        message.success("登录成功！");
        navigate("/dashboard"); // 登录成功后跳转到导航页面
      } else {
        message.error("登录失败：请检查你的用户ID和密码是否正确！");
      }
    } catch (error) {
      console.error(error);
      message.error("登录失败：连接远程服务器失败请稍后重试！");
    } finally {
      setLoading(false);
    }
  };

  return (
    <div
      style={{
        height: "100vh",
        width: "100vw",
        background:
          "linear-gradient(to bottom right, #000000, #141414  ,#434343  )",
        display: "flex",
        justifyContent: "center",
        alignItems: "center",
        position: "absolute",
        top: 0,
        left: 0,
        zIndex: 1,
      }}
    >
      <div
        style={{
          width: "400px",
          borderRadius: "8px",
          padding: "20px",
          boxShadow: "0 2px 8px rgba(0, 0, 0, 1)",
          background: "rgba(255, 255, 255, 0.05)",
        }}
      >
        <img
          src={logo}
          alt="Logo"
          style={{ width: "50%", marginBottom: "20px" }}
        />
        <Form
          name="login"
          initialValues={{ remember: true }}
          onFinish={onFinish}
        >
          <Form.Item
            name="userid"
            rules={[{ required: true, message: "请输入用户ID" }]}
          >
            <Input prefix={<UserOutlined />} placeholder="UserID" />
          </Form.Item>
          <Form.Item
            name="password"
            rules={[{ required: true, message: "请输入密码" }]}
          >
            <Input
              prefix={<LockOutlined />}
              type="password"
              placeholder="Password"
            />
          </Form.Item>
          <Form.Item>
            <div
              style={{
                display: "flex",
                justifyContent: "space-between",
                alignItems: "center",
              }}
            >
              <Form.Item name="remember" valuePropName="checked" noStyle>
                <Checkbox style={{ color: "#fff" }}>记住我</Checkbox>
              </Form.Item>
            </div>
          </Form.Item>

          <Form.Item>
            <Button block type="primary" htmlType="submit" loading={loading}>
              登录
            </Button>
          </Form.Item>
        </Form>
      </div>
    </div>
  );
};

export default Login;
