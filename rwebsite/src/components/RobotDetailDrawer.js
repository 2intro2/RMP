// RobotDetailDrawer.js
import React, { useEffect, useState } from "react";
import {
  Drawer,
  Select,
  Spin,
  message,
  Badge,
  Button,
  Table,
  Typography,
} from "antd";
import { DownloadOutlined } from "@ant-design/icons";

import axios from "axios"; // 引入 axios

const { Option } = Select;
const { Text } = Typography;

const RobotDetailDrawer = ({ visible, onClose, selectedRobot }) => {
  const [model, setModel] = useState("");
  const [name, setName] = useState("");
  const [loading, setLoading] = useState(true);
  const [softwarePackages, setSoftwarePackages] = useState([]);
  const [downloadedPackages, setDownloadedPackages] = useState([]);
  const [deployedPackagesInfo, setDeployedPackagesInfo] = useState([]);
  const [packageLoading, setPackageLoading] = useState(true);
  const [_packageLoading, set_PackageLoading] = useState(true);
  const [isOnline, setIsOnline] = useState(false); // 在线状态
  const [isDownloading, setIsDownloading] = useState(false); // 添加下载状态
  const [isdeploying, setIsdeploying] = useState(false); // 部署状态
  const [selectedPackage, setSelectedPackage] = useState(null); // 选择的软件包ID
  const [selectedDownloadedPackage, setSelectedDownloadedPackage] =
    useState(null);
  const [IP, setIP] = useState("");

  useEffect(() => {
    if (visible && selectedRobot) {
      setDownloadedPackages([]);
      setDeployedPackagesInfo([]);
      setSelectedPackage(null);
      setSelectedDownloadedPackage(null);
      fetchOnlineStauts();
      fetchRobotDetails();
      fetchSoftwarePackages();
      fetchDonwloadedPackages();
      fetchDeployedPackagesInfo();

      // 设置定时器每5秒获取一次在线状态
      const intervalId = setInterval(fetchOnlineStauts, 5000);

      // 清除定时器
      return () => clearInterval(intervalId);
    }
  }, [visible, selectedRobot]);

  const fetchOnlineStauts = async () => {
    try {
      const response = await axios.post(
        "http://192.168.247.128:8080/getonlinestatus",
        {
          robotid: selectedRobot,
        }
      );
      if (response.data.code === 0) {
        setIsOnline(true);
      } else {
        setIsOnline(false);
      }
    } catch (error) {
      setIsOnline(false);
      console.error("连接远程服务器失败！");
    }
  };

  const fetchRobotDetails = async () => {
    setLoading(true);
    try {
      const response = await axios.post(
        "http://192.168.247.128:8080/getrobotinfo",
        {
          robotid: selectedRobot,
        }
      );

      if (response.data.code === 0) {
        setModel(response.data.model);
        setName(response.data.name);
        setIP(response.data.IP);
        message.success("获取机器人信息成功！");
      } else {
        message.error("获取机器人信息失败！");
      }
    } catch (error) {
      message.error("连接远程服务器失败！");
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
        message.success("获取可下载软件包信息成功！");
      } else {
        message.error("获取可下载软件包信息失败！");
      }
    } catch (error) {
      message.error("连接远程服务器失败！");
    } finally {
      setPackageLoading(false);
    }
  };

  const fetchDonwloadedPackages = async () => {
    set_PackageLoading(true);
    try {
      const response = await axios.post(
        "http://192.168.247.128:8080/getdownloadedpackages",
        {
          robotid: selectedRobot,
        }
      );

      if (response.data.code === 0) {
        const packages = response.data.packages;
        setDownloadedPackages(packages);
        message.success("获取已下载软件包信息成功！");
      } else {
        message.error("获取已下载软件包信息失败: " + response.data.errmsg);
      }
    } catch (error) {
      message.error("连接远程服务器失败！");
    } finally {
      set_PackageLoading(false);
    }
  };

  const fetchDeployedPackagesInfo = async () => {
    // 新增函数获取已部署软件包信息
    try {
      const response = await axios.post(
        "http://192.168.247.128:8080/getdeployedpackagesinfo",
        {
          robotid: selectedRobot,
        }
      );

      if (response.data.code === 0) {
        const packagesInfo = response.data.deployedpackagesinfo.map((pkgInfo) =>
          JSON.parse(pkgInfo)
        );
        setDeployedPackagesInfo(packagesInfo);
        message.success("获取已部署软件包信息成功！");
      } else {
        message.error("获取已部署软件包信息失败: " + response.data.errmsg);
      }
    } catch (error) {
      message.error("连接远程服务器失败！");
    }
  };

  const handleDownload = async () => {
    if (!selectedPackage) {
      message.warning("请选择软件包！");
      return;
    }
    setIsDownloading(true); // 开始下载时设置下载状态为 true
    try {
      const response = await axios.post(
        "http://192.168.247.128:8080/sendpackage",
        {
          robotid: selectedRobot,
          packagename: selectedPackage,
        }
      );

      if (response.data.code === 0) {
        message.success("下载软件包成功");
        fetchDonwloadedPackages();
      } else {
        message.error(response.data.errmsg);
      }
    } catch (error) {
      message.error("连接远程服务器失败！");
    } finally {
      setIsDownloading(false);
    }
  };

  const handledeployed = async () => {
    if (!selectedDownloadedPackage) {
      message.warning("请选择软件包！");
      return;
    }
    setIsdeploying(true);
    try {
      const response = await axios.post(
        "http://192.168.247.128:8080/deploypackage",
        {
          robotid: selectedRobot,
          packagename: selectedDownloadedPackage,
        }
      );

      if (response.data.code === 0) {
        message.success("部署软件包成功");
        fetchDeployedPackagesInfo();
      } else {
        message.error("部署软件包失败: " + response.data.errmsg);
      }
    } catch (error) {
      message.error("连接远程服务器失败！");
    } finally {
      setIsdeploying(false);
    }
  };

  const handleClose = () => {
    if (isDownloading) {
      message.warning("下载正在进行中，请等待下载完成！");
    } else {
      onClose(); // 允许关闭
    }
  };

  const handlePackageSelect = (value) => {
    setSelectedPackage(value);
  };

  const handleDonwnloadedPackageSelect = (value) => {
    setSelectedDownloadedPackage(value);
  };
  return (
    <Drawer
      title="详细信息"
      placement="top"
      onClose={handleClose}
      open={visible}
      width={500}
      height={500}
    >
      <>
        <div>
          <span>
            机器人ID: <span style={{ color: "#595959" }}>{selectedRobot}</span>
          </span>
          <Badge
            status={isOnline ? "success" : "error"}
            style={{ marginLeft: "20px" }}
          />
          <span
            style={{
              color: isOnline ? "#52c41a" : "#f5222d", // 颜色与Badge一致
              marginLeft: 5, // 适当的间隔
            }}
          >
            {isOnline ? "在线" : "离线"}
          </span>
          <span style={{ marginLeft: "18px" }}>
            型号: <span style={{ color: "#595959" }}>{model}</span>
          </span>
          <span style={{ marginLeft: "20px" }}>
            昵称: <span style={{ color: "#595959" }}>{name}</span>
          </span>
          <span style={{ marginLeft: "20px" }}>
            IP: <span style={{ color: "#595959" }}>{IP}</span>
          </span>
        </div>

        <div style={{ marginTop: 20 }}>
          <span style={{}}>可下载软件包:</span>
          {packageLoading ? (
            <Spin size="small" style={{ marginLeft: 10 }} />
          ) : (
            <>
              <Select
                style={{ width: 270, height: 25, marginLeft: 10 }}
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
                style={{ marginLeft: 10 }}
                onClick={handleDownload}
                loading={isDownloading}
              />
            </>
          )}
          <span style={{ marginLeft: "25px" }}>已下载软件包:</span>
          {_packageLoading ? (
            <Spin size="small" style={{ marginLeft: 10 }} />
          ) : (
            <>
              <Select
                style={{ width: 270, height: 25, marginLeft: 10 }}
                placeholder="选择软件包"
                value={selectedDownloadedPackage}
                onChange={handleDonwnloadedPackageSelect}
              >
                {downloadedPackages.map((pkgname) => (
                  <Option key={pkgname} value={pkgname}>
                    {pkgname}
                  </Option>
                ))}
              </Select>
              <Button
                type="primary"
                style={{ marginLeft: 10 }}
                onClick={handledeployed}
                loading={isdeploying}
              >
                部署
              </Button>
            </>
          )}
        </div>
        <div style={{ marginTop: 20 }}>
          <p>已部署软件包信息:</p>
          {deployedPackagesInfo.length === 0 ? (
            <p style={{ color: "#999" }}>暂无已部署软件包信息</p>
          ) : (
            <Table
              size="small"
              style={{ marginTop: 10 }}
              dataSource={deployedPackagesInfo}
              columns={[
                {
                  title: "项目",
                  dataIndex: "project",
                  key: "project",
                },
                {
                  title: "包名",
                  dataIndex: "package_name",
                  key: "package_name",
                },
                {
                  title: "分支",
                  dataIndex: "branch",
                  key: "branch",
                },
                {
                  title: "提交",
                  dataIndex: "commit",
                  key: "commit",
                },
                {
                  title: "部署时间",
                  dataIndex: "deploy_time",
                  key: "deploy_time",
                },
              ]}
              pagination={false} // 如果不需要分页，可以设置为 false
              rowKey="package_name" // 设置每一行的唯一键
            />
          )}
        </div>
      </>
      {loading ? <Spin style={{ marginTop: 20 }} /> : <div></div>}
    </Drawer>
  );
};

export default RobotDetailDrawer;
