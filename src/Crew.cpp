#include "Crew.h"

// 构造函数
Crew::Crew(const std::string& id, const std::string& name, CrewRole role)
    : id(id), name(name), role(role) {}

// 获取ID
std::string Crew::getId() const {
    return id;
}

// 获取姓名
std::string Crew::getName() const {
    return name;
}

// 获取角色
CrewRole Crew::getRole() const {
    return role;
}

// 添加可执勤/可驾驶的机型（机长/乘务员均使用）
void Crew::addQualification(const std::string& aircraftType) {
    qualifiedAircraftTypes.push_back(aircraftType);
}

// 获取可驾驶的飞机型号列表
const std::vector<std::string>& Crew::getQualifications() const {
    return qualifiedAircraftTypes;
}

// 设置上次分配时间
void Crew::setLastAssigned(std::time_t t) {
    lastAssigned = t;
}

// 获取上次分配时间
std::time_t Crew::getLastAssigned() const {
    return lastAssigned;
}
