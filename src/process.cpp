#include "process.h"
#include "log.h"
#include "util.h"
#include "config.h"
#include "environment.h"


namespace bifang
{

SystemLogger();

static Config<uint32_t>::ptr g_daemon_restart_interval =
           ConfigMgr::GetInstance()->get("system.daemon.restart_interval", (uint32_t)5, "daemon restart interval");

std::string ProcessManager::toString() const
{
    std::stringstream ss;
    ss << "[ProcessManager"
       << " parentId=" << m_parentId
       << ", mainId=" << m_mainId
       << ", parentStartTime=" << time_to_string(m_parentStartTime)
       << ", mainStartTime=" << time_to_string(m_mainStartTime)
       << ", restartCount=" << m_restartCount << "]";
    return ss.str();
}

int ProcessManager::start(int argc, char** argv,
                        std::function<int(int argc, char** argv)> main_cb,
                        bool is_daemon)
{
    if (!is_daemon)
    {
        m_parentId = getpid();
        m_parentStartTime = time(0);
        return normalStart(argc, argv, main_cb);
    }
    return daemonStart(argc, argv, main_cb);
}

// private
int ProcessManager::normalStart(int argc, char** argv,
                        std::function<int(int argc, char** argv)> main_cb)
{
    m_mainId = getpid();
    m_mainStartTime = time(0);
    return main_cb(argc, argv);
}

// private
int ProcessManager::daemonStart(int argc, char** argv,
                        std::function<int(int argc, char** argv)> main_cb)
{
    daemon(1, 0);
    m_parentId = getpid();
    m_parentStartTime = time(0);

    while (1)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // 子进程返回
            log_debug << "process start pid=" << getpid();
            return normalStart(argc, argv, main_cb);
        }
        else if (pid < 0)
        {
            log_error << "fork fail return=" << pid
                << ", errno=" << errno << ", errstr=" << strerror(errno);
            return -1;
        }
        else
        {
            // 父进程返回
            int status = 0;
            waitpid(pid, &status, 0);
            if (status)
            {
                if (status == SIGUSR2)
                {
                    kill(pid, SIGKILL);
                    log_info << "child be killed";
                    Config<std::string>::ptr g_pid_file = ConfigMgr::GetInstance()->get<std::string>("system.pid_file");
                    std::string pidfile = EnvMgr::GetInstance()->getAbsolutePath(g_pid_file->getValue());
                    FileUtil::Rm(pidfile);
                    break;
                }
                else
                    log_error << "child crash pid=" << pid << ", status=" << status << ", will restart";
            }
            else
            {
                log_info << "child finished pid=" << pid;
                Config<std::string>::ptr g_pid_file = ConfigMgr::GetInstance()->get<std::string>("system.pid_file");
                std::string pidfile = EnvMgr::GetInstance()->getAbsolutePath(g_pid_file->getValue());
                FileUtil::Rm(pidfile);
                break;
            }
            sleep(g_daemon_restart_interval->getValue());
            log_info << "child restart time=" << ++m_restartCount;
        }
    }

    return 0;
}


}
