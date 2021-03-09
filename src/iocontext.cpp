#include "iocontext.h"
#include "log.h"
#include "Assert.h"

namespace bifang
{

SystemLogger();

IOContext::IOContext(int fd)
    :m_fd(fd)
{
}

IOContext::Context& IOContext::get(Event event)
{
    switch (event)
    {
        case READ:
            return m_context[0];
        case WRITE:
            return m_context[1];
        default:
            ASSERT_MSG(false, "IOContext::get");
    }
    throw std::invalid_argument("IOContext::get: invalid event");
}

void IOContext::reset(Event event)
{
    switch (event)
    {
        case READ:
            m_context[0].scheduler = nullptr;
            m_context[0].fiber.reset();
            m_context[0].cb = nullptr;
            break;
        case WRITE:
            m_context[1].scheduler = nullptr;
            m_context[1].fiber.reset();
            m_context[1].cb = nullptr;
            break;
        default:
            ASSERT_MSG(false, "IOContext::reset");
            break;
    }
    throw std::invalid_argument("IOContext::reset: invalid event");
}

void IOContext::reset()
{
    reset(READ);
    reset(WRITE);
}

void IOContext::trigger(Event event)
{
    ASSERT(m_event & event);
    m_event = m_event & ~event;
    Context& ctx = get(event);
    if (ctx.cb)
    {
        ctx.scheduler->schedule(ctx.cb);
        ctx.cb = nullptr;
    }
    else
    {
        ctx.scheduler->schedule(ctx.fiber);
        ctx.fiber.reset();
    }
    ctx.scheduler = nullptr;
}

}
