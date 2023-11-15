#include<yaml-cpp/yaml.h>
#include<iostream>
#include<fstream>
#include "GGo.h"

//日志输出遍历yml
void printyml(YAML::Node node,int level){
    if (node.IsScalar())
    {
        GGO_LOG_INFO(GGO_LOG_ROOT()) << std::string(level * 4, ' ')
                                         << node.Scalar() << " - " << node.Type() << " - " << level;
    }
    else if (node.IsNull())
    {
        GGO_LOG_INFO(GGO_LOG_ROOT()) << std::string(level * 4, ' ')
                                         << "NULL - " << node.Type() << " - " << level;
    }
    else if (node.IsMap())
    {
        for (auto it = node.begin();
             it != node.end(); ++it)
        {
            GGO_LOG_INFO(GGO_LOG_ROOT()) << std::string(level * 4, ' ')
                                             << it->first << " - " << it->second.Type() << " - " << level;
            printyml(it->second, level + 1);
        }
    }
    else if (node.IsSequence())
    {
        for (size_t i = 0; i < node.size(); ++i)
        {
            GGO_LOG_INFO(GGO_LOG_ROOT()) << std::string(level * 4, ' ')
                                             << i << " - " << node[i].Type() << " - " << level;
            printyml(node[i], level + 1);
        }
    }
}

int main(){
    //用文件名打开yml文件 解析为 YAML::node 并用流的方式输入输出
    YAML::Node root1 = YAML::LoadFile("/root/workspace/GGoSeverFrame/conf/test01.yml");
    std::ifstream file1("/root/workspace/GGoSeverFrame/conf/test01.yml");
    YAML::Node root2 = YAML::Load(file1);
    std::cout << root1 << std::endl << root2 << std::endl;
    std::ofstream file2("/root/workspace/GGoSeverFrame/conf/test02.yml");
    file2 << root1;
    //更改node
    // root1[root2] = root2;
    file2 << root1;
    std::cout <<"===================================" << std::endl;
    std::cout << root1;
    std::cout <<"===================================" << std::endl;
    printyml(root1,0);
    root1[root2] = root1;
    return 0;
}