#include "workerthreadevent.h"

namespace Tufao{

WorkerThreadEvent::WorkerThreadEvent(Type t) : QEvent((QEvent::Type)t)
{
}

} //namepace Tufao
