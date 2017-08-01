//
// Created by Kewin Brandsma on 31/07/2017.
//

#include "baseRenderObj.h"
#include "shader.h"

BaseRenderObj::BaseRenderObj()  {

}

int BaseRenderObj::init(Context &context) {
    this->context = context;
    return 0;
}
//
int BaseRenderObj::update() {
    return 0;
}

int BaseRenderObj::draw() {

    return 0;
}

int BaseRenderObj::destroy() {

    return 0;
}
