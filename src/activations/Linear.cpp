//
// Created by Poppro on 12/3/2019.
//

#include "Linear.h"

namespace PyreNet {
    const double Linear::activate(double input) {
        return input;
    }
    const double Linear::deactivate(double input) {
        return 1;
    }

    const LayerDefinition::activationType Linear::type() {
        return LayerDefinition::activationType::linear;
    }
};