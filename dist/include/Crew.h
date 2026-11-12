#ifndef CREW_H
#define CREW_H

#include <string>
#include <vector>
#include <ctime>

// 定义机组成员角色
enum class CrewRole {
    PILOT,
    CABIN_CREW
};

// 机组成员类
class Crew {
public:
    // 构造函数
    Crew(const std::string& id, const std::string& name, CrewRole role);

    // 获取ID
    std::string getId() const;

    // 获取姓名
    std::string getName() const;

    // 获取角色
    CrewRole getRole() const;

    // 添加可驾驶的飞机型号 (仅对机长有效)
    void addQualification(const std::string& aircraftType);

    // 获取可驾驶的飞机型号列表
    const std::vector<std::string>& getQualifications() const;

    // 上次被分配任务的时间（Unix timestamp），0 表示未分配过或未知
    void setLastAssigned(std::time_t t);
    std::time_t getLastAssigned() const;

private:
    std::string id;                 // 员工ID
    std::string name;               // 姓名
    CrewRole role;                  // 角色
    std::vector<std::string> qualifiedAircraftTypes; // 可驾驶的飞机型号
    std::time_t lastAssigned = 0;
};

#endif // CREW_H
