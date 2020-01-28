
/**
* @version 1.0.0
* @author skyfire
* @file sf_router_shared_ptr_compare.hpp
*/

#pragma once

#include "sf_router_shared_ptr_compare.h"

namespace skyfire {

template <typename RouterType>
bool sf_router_shared_ptr_compare__<RouterType>::operator()(
    const std::shared_ptr<RouterType> &router1,
    const std::shared_ptr<RouterType> &router2) const {
    return router1->priority() < router2->priority();
}
}    // namespace skyfire