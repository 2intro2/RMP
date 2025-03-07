#include "robotclient.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: ./robotClient <robot_id>" << std::endl;
        return 0;
    }
    RobotClient client(atoi(argv[1]));
    client.start("192.168.247.128", 6868);
    return 0;
}
