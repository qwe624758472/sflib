
/**
* @version 1.0.0
* @author skyfire
* @file sf_single_instance.hpp
*/

/*
 * sf_single_instance单例支持
 */

#pragma once
#include "sf_single_instance.h"
#include "sf_stdc++.h"

namespace skyfire {

template <typename ThisClass, typename BaseClass>
template <typename... Args>
std::shared_ptr<ThisClass> sf_single_instance<ThisClass, BaseClass>::instance(
    Args&&... args)
{
    std::call_once(flag, [&] {
        instance__ = std::shared_ptr<ThisClass>(
            new ThisClass(std::forward<Args>(args)...));
    });

    return instance__;
}

} // namespace skyfire