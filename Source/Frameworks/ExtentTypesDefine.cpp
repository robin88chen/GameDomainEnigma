#include "ExtentTypesDefine.h"

future_error make_future_err(std::system_error er)
{
    return std::async([=]() { return er; });
}
