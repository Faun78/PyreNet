//
// Created by Poppro on 12/2/2019.
//

#include "Layer.h"

#include <condition_variable>

#include "activations/ActivationFactory.h"
#include "thread/LayerThreadPool.h"

namespace PyreNet {
// Constructor

    Layer::Layer(int size, int prevSize, Activation *activation, Activation *deactivation) {
        this->nodes.reserve(size);
        for (int i = 0; i < size; i++) {
            this->nodes.emplace_back(prevSize);
        }
        this->activation = activation;
        this->deactivation = deactivation;

    }

// Main Layer Logic
    std::vector<double> Layer::getValue() {
        std::vector<double> ans;
        ans.reserve(this->nodes.size());
        for (const Perceptron &p : this->nodes) {
            ans.push_back(p.getValue());
        }
        return ans;
    }
    std::vector<double> Layer::calculate(const std::vector<double> &input) {
        LayerThreadPool* layerThreadPool = LayerThreadPool::getInstance();
        int track = this->nodes.size();
        for (Perceptron &p : this->nodes) {
            LayerThreadPool::LayerQueueJob job(input, p, this->activation, track);
            layerThreadPool->addJob(job);
        }
        layerThreadPool->waitForTasks(track);
        std::vector<double> ans;
        ans.reserve(this->nodes.size());
        
        return getValue();
    }
    
    std::vector<double> Layer::decalculate(const std::vector<double> &input) {
        LayerThreadPool* layerThreadPool = LayerThreadPool::getInstance();
        int track = this->nodes.size();
        for (Perceptron &p : this->nodes) {
            LayerThreadPool::LayerQueueJob job(input, p, this->deactivation, track);
            layerThreadPool->addJob(job);
        }
        layerThreadPool->waitForTasks(track);
        return getValue();
    }

// Mutators
    void Layer::add(double lower) {
        for (Perceptron &p : this->nodes) {
            p.add(lower, upper);
        }
    }

    void Layer::mutate_uniform(double lower, double upper) {
        for (Perceptron &p : this->nodes) {
            p.mutate_uniform(lower, upper);
        }
    }

    void Layer::mutate_gaussian(double mean, double std) {
        for (Perceptron &p : this->nodes) {
            p.mutate_gaussian(mean, std);
        }
    }

// Getters

    int Layer::size() {
        return this->nodes.size();
    }

// Serialize

    std::ostream &operator<<(std::ostream& os, const Layer &l) {
        os << ActivationFactory::toString(l.activation->type()) << " ";
        os << l.nodes.size() << " ";
        for (const Perceptron& p : l.nodes)
            os << p << " ";
        return os;
    }

    std::istream& operator>>(std::istream& is, Layer &l) {
        ActivationFactory* activationFactory = ActivationFactory::getInstance();
        std::string activationString;
        is >> activationString;
        l.activation = activationFactory->getActivation(ActivationFactory::fromString(activationString));
        int nodesSize;
        is >> nodesSize;
        l.nodes.resize(nodesSize, Perceptron(0));
        for (Perceptron& p : l.nodes)
            is >> p;
        return is;
    }
}
