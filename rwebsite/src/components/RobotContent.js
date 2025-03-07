// RobotContent.js
import React, { useState, useEffect } from "react";
import {
  Layout,
  theme,
  List,
  Typography,
  Spin,
  message,
  Button,
  Select,
  Modal,
  Badge,
} from "antd";
import { RobotOutlined, DownloadOutlined } from "@ant-design/icons";
import axios from "axios";

import RobotDetailDrawer from "./RobotDetailDrawer"; // 导入新的抽屉组件

const { Content } = Layout;
const { Title } = Typography;
const { Option } = Select;

export default function RobotContent({ userid }) {
  const {
    token: { colorBgContainer, borderRadiusLG },
  } = theme.useToken();

  const [robots, setRobots] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  const [drawerVisible, setDrawerVisible] = useState(false);
  const [selectedRobot, setSelectedRobot] = useState(null); // 存储选定机器人的详细信息
  const [packageLoading, setPackageLoading] = useState(true);
  const [softwarePackages, setSoftwarePackages] = useState([]);
  const [selectedPackage, setSelectedPackage] = useState(null); // 选择的软件包ID
  const [isDownloading, setIsDownloading] = useState(false); // 添加下载状态
  const [modalVisible, setModalVisible] = useState(false);
  const [downloadResults, setDownloadResults] = useState([]); // 存储下载结果
  const [onlineStatus, setOnlineStatus] = useState({}); // 存储每个机器人的在线状态

  useEffect(() => {
    const intervalId = setInterval(() => {
      fetchAllOnlineStauts();
    }, 4000);

    fetchAllOnlineStauts();

    return () => clearInterval(intervalId);
  }, [userid, robots]);

  useEffect(() => {
    fetchRobots();
    fetchSoftwarePackages();
  }, [userid]);

  const showDrawer = (robotId) => {
    setSelectedRobot(robotId);
    setDrawerVisible(true);
  };

  const onClose = () => {
    setDrawerVisible(false);
    setSelectedRobot(null); // 清空选择的机器人ID
  };
  const fetchRobots = async () => {
    try {
      setLoading(true);

      const userData = JSON.parse(localStorage.getItem("userData"));
      const userId = userData ? userData.userid : null;

      if (!userId) {
        message.error("请先登录！");
        setLoading(false);
        return;
      }

      const response = await axios.post(
        "http://192.168.247.128:8080/getrobotlist",
        {
          userid: userId,
        }
      );

      const { code, robotlist } = response.data;

      if (code === 0) {
        const robotIds = robotlist.map((item) => {
          const robot = JSON.parse(item);
          return robot.robotid;
        });
        setRobots(robotIds);
        message.success("获得机器人列表成功！");
      } else {
        message.error("获取机器人列表失败！");
      }
    } catch (error) {
      setError(error.response ? error.response.data : error.message);
    } finally {
      setLoading(false);
    }
  };

  const fetchSoftwarePackages = async () => {
    setPackageLoading(true);
    try {
      const response = await axios.get("http://192.168.247.128:8080/getpackages");

      if (response.data.code === 0) {
        const packages = response.data.packages.map((pkg) => JSON.parse(pkg));
        setSoftwarePackages(packages);
        message.success("获取软件包信息成功！");
      } else {
        message.error("获取软件包信息失败！");
      }
    } catch (error) {
      message.error("连接远程服务器失败！");
    } finally {
      setPackageLoading(false);
    }
  };

  const handleDownload = async () => {
    if (!selectedPackage) {
      message.warning("请选择软件包！");
      return;
    }
    setIsDownloading(true);
    try {
      const response = await axios.post(
        "http://192.168.247.128:8080/bulksendpackage",
        {
          robotids: robots,
          packagename: selectedPackage,
        }
      );

      if (response.data.code === 0) {
        const parsedResults = response.data.result
          .filter((result) => result.trim() !== "")
          .map((result) => JSON.parse(result));

        parsedResults.forEach((result) => {
          console.log(`Robot ID: ${result.robotid}, Result: ${result.code}`);

          setDownloadResults(parsedResults); // 存储解析后的结果
          setModalVisible(true); // 显示弹窗
        });
      } else {
        message.error(response.data.errmsg);
      }
    } catch (error) {
      message.error("连接远程服务器失败！");
    } finally {
      setIsDownloading(false);
    }
  };

  const handlePackageSelect = (value) => {
    setSelectedPackage(value);
  };

  const fetchAllOnlineStauts = async () => {
    try {
      const response = await axios.post(
        "http://192.168.247.128:8080/getallonlinestatus",
        {
          userid: userid,
          robotids: robots,
        }
      );
      if (response.data.code === 0) {
        const result = response.data.result;
        const robotStatus = result.map((robotStr) => JSON.parse(robotStr));

        const statusMap = {};
        robotStatus.forEach((status) => {
          statusMap[status.robotid] = status.code;
        });

        // 更新在线状态
        setOnlineStatus(statusMap);
      }
    } catch (error) {
      const statusMap = robots.reduce((acc, robotId) => {
        acc[robotId] = -1;
        return acc;
      }, {});

      setOnlineStatus(statusMap);
      console.error("连接远程服务器失败！");
    }
  };

  // 渲染 badge 的函数
  const renderBadge = (code) => {
    if (code === -1) {
      return (
        <>
          <Badge status="error" style={{ marginLeft: "10px" }} />
          <span
            style={{
              color: "#f5222d",
              marginLeft: 5,
            }}
          >
            离线
          </span>
        </>
      );
    } else if (code === 0) {
      return (
        <>
          <Badge status="success" style={{ marginLeft: "10px" }} />
          <span
            style={{
              color: "#52c41a",
              marginLeft: 5,
            }}
          >
            在线
          </span>
        </>
      );
    }
  };

  return (
    <Content
      style={{
        margin: "24px 16px",
        padding: 24,
        minHeight: 280,
        background: colorBgContainer,
        borderRadius: borderRadiusLG,
      }}
    >
      {loading ? (
        <Spin size="large" />
      ) : error ? (
        <Title level={4} style={{ color: "red" }}>
          错误: {error}
        </Title>
      ) : (
        <>
          <Title level={4} style={{ marginTop: "0px" }}>
            你管理 {robots.length} 个机器人
          </Title>

          <span style={{}}>可下载软件包:</span>

          {packageLoading ? (
            <Spin size="small" style={{ marginLeft: 10 }} />
          ) : (
            <>
              <Select
                style={{
                  width: 250,
                  height: 35,
                  marginLeft: 10,
                  marginBottom: 10,
                }}
                placeholder="选择软件包"
                value={selectedPackage}
                onChange={handlePackageSelect}
              >
                {softwarePackages.map((pkg) => (
                  <Option key={pkg.id} value={pkg.name}>
                    {pkg.name}
                  </Option>
                ))}
              </Select>
              <Button
                type="primary"
                icon={<DownloadOutlined />}
                style={{ marginLeft: 10, height: 35 }}
                onClick={handleDownload}
                loading={isDownloading}
              >
                下载至所有机器人
              </Button>
            </>
          )}

          <List
            bordered
            dataSource={robots}
            renderItem={(robotid) => (
              <List.Item
                key={robotid}
                style={{ display: "flex", alignItems: "center" }}
              >
                <RobotOutlined style={{ marginRight: 8 }} />
                机器人ID: {robotid}
                {renderBadge(onlineStatus[robotid])}
                <Button
                  type="link"
                  onClick={() => showDrawer(robotid)}
                  style={{ marginLeft: "auto" }} // 将按钮推到右侧
                  disabled={isDownloading}
                >
                  详细信息
                </Button>
              </List.Item>
            )}
          />

          {/* 使用新的机器人详情抽屉组件 */}
          <RobotDetailDrawer
            visible={drawerVisible}
            onClose={onClose}
            selectedRobot={selectedRobot}
          />

          <Modal
            title="下载结果"
            visible={modalVisible}
            onCancel={() => setModalVisible(false)}
            footer={null}
            centered // 弹窗居中
          >
            <List
              dataSource={downloadResults}
              renderItem={(result) => (
                <List.Item>
                  <span>机器人ID: {result.robotid}</span>
                  {result.code === 0 ? (
                    <span style={{ color: "green", marginLeft: 10 }}>
                      下载软件包成功
                    </span>
                  ) : (
                    <span style={{ color: "red", marginLeft: 10 }}>
                      下载软件包失败: {result.errmsg}
                    </span>
                  )}
                </List.Item>
              )}
            />
          </Modal>
        </>
      )}
    </Content>
  );
}
