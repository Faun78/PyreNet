#include "NeuralNet.h"

namespace PyreNet {
    // Constructor

    NeuralNet::NeuralNet(int inputSize, const std::vector<LayerDefinition> &layers) {
        ActivationFactory* activationFactory = ActivationFactory::getInstance();
        this->inputSize = inputSize;
        this->layers.reserve(layers.size());
        int prevSize = inputSize;
        for (LayerDefinition layer : layers) {
            std::vector<Perceptron> tmp;
            tmp.reserve(layer.size);
            Activation* activation = activationFactory->getActivation(layer.activation);
            this->layers.emplace_back(layer.size, prevSize, activation);
            prevSize = layer.size;
        }
    }

    NeuralNet::NeuralNet(std::istream &is) {
        this->inputSize = 0;
        is >> *this;
    }

    // Predictor

    std::vector<double> NeuralNet::predict(const std::vector<double> &input) {
        if (input.size() != this->inputSize) throw InvalidInputSize();
        std::vector<double> layerData(input);
        for (Layer &l : this->layers) {
            layerData = l.calculate(layerData);
        }
        return layerData;
    }

    //  Bacpropagationsw
    void NeuralNet::backpropagate(const std::vector<double> &input,const std::vector<double> &target) {
        const std::vector<double> output=predict(input);
        std::vector<double> outputerror;
        double totalerror=0;
        int to= this->layers.size()-1;

        for (int i = 0; i < target.size(); i++) {
            totalerror+=0.5*double(std::sqrt(target[i]-output[i])); 
            outputerror.push_back(0.5*double(std::sqrt(target[i]-output[i])));
        }
        std::vector<double> layerData=this->layers[to-1].getValue();
        std::vector<double> gradient=this->layers[to].decalculate(layerData);
        
        if(outputerror.size()!=gradient.size()){
            return;
        }
        for(int j=0;j<gradient.size();j++){
            double temp11=gradient[j];
            for(int i=0;i<gradient.size();i++){
                gradient[j]+=temp11*outputerror[i];
            }
            gradient[j]*=this->learningrate;
        }
        layerData=this->layers[to-2].getValue();
        std::vector<double> gradient2=this->layers[to].decalculate(layerData);
        gradient2
    }


    // Mutators

    void NeuralNet::mutate_uniform(double lower, double upper, int layer) {
        if (layer == -1) {
            for (Layer &l : this->layers) {
                l.mutate_uniform(lower, upper);
            }
        } else {
            if (layer < 0 || layer >= this->layers.size())
                throw InvalidLayer();
            this->layers[layer].mutate_uniform(lower, upper);
        }
    }

    void NeuralNet::mutate_gaussian(double mean, double std, int layer) {
        if (layer == -1) {
            for (Layer &l : this->layers) {
                l.mutate_gaussian(mean, std);
            }
        } else {
            if (layer < 0 || layer >= this->layers.size())
                throw InvalidLayer();
            this->layers[layer].mutate_gaussian(mean, std);
        }
    }

    // Getters

    int NeuralNet::getInputSize() {
        return this->inputSize;
    }

    int NeuralNet::getOutputSize() {
        return this->layers.back().size();
    }

    // Serialize

    std::ostream &operator<<(std::ostream &os, const NeuralNet& nn) {
        os << nn.inputSize << " ";
        os << nn.layers.size() << " ";
        for (const Layer& l : nn.layers)
            os << l << " ";
        return os;
    }

    std::istream& operator>>(std::istream& is, NeuralNet& nn) {
        is >> nn.inputSize;
        int layerSize;
        is >> layerSize;
        nn.layers.resize(layerSize, Layer(0, 0, nullptr,nullptr));
        for (Layer& l : nn.layers)
            is >> l;
        return is;
    }
}
