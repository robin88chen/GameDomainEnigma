#include "Query.h"
#include "QueryDispatcher.h"

using namespace Enigma::Frameworks;

void IQuery::dispatch()
{
    QueryDispatcher::dispatch(shared_from_this());
}
