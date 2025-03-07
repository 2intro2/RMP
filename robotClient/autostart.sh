#!/bin/bash

# 检查是否以 root 身份执行
if [[ $EUID -ne 0 ]]; then
   echo "请使用 root 身份执行此脚本" 
   exit 1
fi

#获取脚本所在目录
CURRENT_PATH=$(pwd)

# 设置程序的路径和参数
EXECUTABLE_PATH="${CURRENT_PATH}/bin/robotClient"  
WORKING_DIRECTORY="${CURRENT_PATH}"      
ROBOT_ID="112801" #请在此处绑定机器人ID                                          

# 创建 systemd 服务文件
SERVICE_FILE="/etc/systemd/system/robotClient.service"

echo "[Unit]
Description=Robot Client

[Service]
ExecStart=${EXECUTABLE_PATH} ${ROBOT_ID}
WorkingDirectory=${WORKING_DIRECTORY}
Restart=always

[Install]
WantedBy=multi-user.target" > ${SERVICE_FILE}

# 重新加载 systemd
systemctl daemon-reload

# 启动服务
systemctl start robotClient.service

# 设置开机自启动
systemctl enable robotClient.service

echo "服务已创建并设置为开机自启动。"

echo
echo "-------------------------------------------------------------"
echo "请使用以下命令管理服务："
echo "启动服务 sudo systemctl start robotClient.service"
echo "停止服务 sudo systemctl stop robotClient.service"
echo "重启服务 sudo systemctl restart robotClient.service"
echo "查看服务状态 sudo systemctl status robotClient.service"
echo "禁用开机自启动 sudo systemctl disable robotClient.service"
echo "启用开机自启动 sudo systemctl enable robotClient.service"
echo "删除服务文件 sudo rm /etc/systemd/system/robotClient.service"
echo "重新加载systemd systemctl daemon-reload"
echo "查看日志 sudo journalctl -u robotClient.service"
echo "实时查看日志 sudo journalctl -u robotClient.service -f"

#停止服务 sudo systemctl stop robotClient.service
#重启服务 sudo systemctl restart robotClient.service
#查看服务状态 sudo systemctl status robotClient.service
#禁用开机自启动 sudo systemctl disable robotClient.service
#删除服务文件 sudo rm /etc/systemd/system/robotClient.service

#查看日志 sudo journalctl -u robotClient.service
#实时查看日志 sudo journalctl -u robotClient.service -f




