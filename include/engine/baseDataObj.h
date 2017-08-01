//
// Created by Kewin Brandsma on 31/07/2017.
//

#pragma once

#include "../../extern/json/json.hpp"

using json = nlohmann::json;

class BaseDataObj {
public:
    int id;

    BaseDataObj() {}

    virtual int update(json data) {
        return 0;
    }
private:
};
