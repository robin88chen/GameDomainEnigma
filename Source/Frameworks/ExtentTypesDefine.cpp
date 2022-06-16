#include "ExtentTypesDefine.h"

future_error make_future_err(std::error_code er)
{
    return std::async([=]() { return er; });
}
