#include "config.h"
#include "environment.h"

namespace bifang
{

SystemLogger();

static void listAllMember(std::string prefix, const Json::Value& root,
                std::vector<std::pair<std::string, const Json::Value> >& nodes)
{
    std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

    // 会多压入一些无用的数据, 考虑到map配置点和配置文件并非频繁使用, 暂不优化
    nodes.push_back(std::make_pair(prefix, root));
    if (root.isObject())
    {
        auto names = root.getMemberNames();
        for (auto& i : names)
            listAllMember(prefix.empty() ? i : prefix + "." + i, root[i], nodes);
    }
}

void ConfigManager::load(Json::Value& root)
{
    std::vector<std::pair<std::string, const Json::Value> > nodes;
    listAllMember("", root, nodes);

    for (auto& i : nodes)
    {
        if (i.first.empty())
            continue;

        auto it = m_datas.find(i.first);
        ConfigBase::ptr var = it == m_datas.end() ? nullptr : it->second;
        if (var)
        {
            if (i.second.isString()) // 防止字符串值打出双引号, 只能对其单独处理
            {
                var->fromString(i.second.asString());

                // 打印配置点
                //std::cout << i.first << "=" << i.second.asString() << std::endl;
            }
            else
            {
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());

                // 打印配置点
                //std::cout << i.first << "=" << ss.str() << std::endl;
            }
        }
    }
}

void ConfigManager::load(const std::string& path)
{
    std::string absoulte_path = FileUtil::PureDirname(path);
    absoulte_path = EnvMgr::GetInstance()->getAbsolutePath(absoulte_path);
    std::vector<std::string> files;

    FileUtil::ListAllFile(files, absoulte_path, ".json");

    for (auto& i : files)
    {
        try
        {
            Json::Value root;
            Json::String errs;
            Json::CharReaderBuilder reader;
            std::ifstream ifs;
            ifs.open(i);
            if (!Json::parseFromStream(reader, ifs, &root, &errs))
            {
                log_error << "parse json err " << errs;
                throw std::logic_error("read file:" + i + " error!");
            }

            load(root);
            //log_debug << "LoadConfFile file=" << i << " ok";
        }
        catch (...)
        {
            log_error << "LoadConfFile file=" << i << " failed";
        }
    }
}

}
