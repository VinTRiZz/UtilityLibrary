#include "module.hpp"
#include <thread>

Components::Module::Module(const Components::ModuleConfiguration &config) :
    m_config {config}
{

}

Components::PModule Components::Module::createModule(const Components::ModuleConfiguration &config)
{
    PModule result;
    result = PModule(new Module(config));
    result->m_pSelf = result;
    return result;
}

Components::Module::~Module()
{

}

Components::ModuleType Components::Module::type() const
{
    return m_config.type;
}

std::string Components::Module::name() const
{
    return m_config.name;
}

Components::ModuleStatus Components::Module::status() const
{
    return m_status.load();
}

void Components::Module::setStatus(Components::ModuleStatus s)
{
    m_status.store(s);
}

void Components::ModuleConfiguration::addRequiredConnection(ModuleType _type)
{
    for (auto t : requiredConnections)
    {
        if (t == _type)
            return;
    }

    requiredConnections.push_back(_type);
}

Components::PMessage Components::Module::sendMessage(const PMessage &msg)
{
    auto receiverType = msg->receiver;
    for (auto con : m_connections)
    {
        if (con->type() == receiverType)
        {
            return con->process(msg);
        }
    }
    return PMessage();
}

Components::PMessage Components::Module::process(Components::PMessage msg)
{
    if (!m_config.messageProcessingFunction)
        return {};

    return m_config.messageProcessingFunction(msg, m_pSelf.lock());
}

std::future<void> Components::Module::init()
{
    if (!m_config.initFunction)
        return {};

    if (m_config.initAsync)
        return std::async([this](){setStatus(m_config.initFunction(m_pSelf.lock()));});

    setStatus(m_config.initFunction(m_pSelf.lock()));
    return {};
}

void Components::Module::start()
{
    if (!m_config.mainCycleFunction)
        return;

    if (m_config.workAsync)
    {
        m_asyncWorker = std::async(m_config.mainCycleFunction, m_pSelf.lock());
    }
    else
    {
        m_threadWorker = std::shared_ptr<std::thread>(
            new std::thread(m_config.mainCycleFunction, m_pSelf.lock()),
            [](std::thread * pThread)
            {
                if (pThread->joinable())
                    pThread->join();
                delete pThread;
            }
        );
    }
}

void Components::Module::poll()
{
    if (!m_config.mainCycleFunction)
        return;

    if (m_config.workAsync)
        m_asyncWorker.get();
    else
        m_threadWorker.reset();
}

void Components::Module::stop()
{
    setStatus(ModuleStatus::MODULE_STATUS_STOPPING);

    if (m_config.stopCallbackFunction) m_config.stopCallbackFunction(m_pSelf.lock());
}

void Components::Module::lock()
{
    m_workingThreadMx.lock();
}

void Components::Module::unlock()
{
    m_workingThreadMx.unlock();
}


void Components::Module::sleep_us(uint64_t time)
{
    std::this_thread::sleep_for(std::chrono::microseconds(time));
}

void Components::Module::sleep_ms(uint64_t time)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(time));
}

void Components::Module::sleep_s(uint64_t time)
{
    std::this_thread::sleep_for(std::chrono::seconds(time));
}
