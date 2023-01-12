#pragma once

// the main class of the pre-computing program
// generate all profiles for all valid QuadDags

#include "quad_dag_profile.hpp"

namespace flowbench {

template <class T> // where T : QuadDagInstantiater
class QuadDagGenerator : public Singleton<QuadDagGenerator<T>> {
public:
    // generate all profiles for all valid QuadDags and write them to the given output stream
    void operator()(std::ostream& os);

private:
    // use the analyzer to find all valid QuadDags, and use the profile to generate the profile for each of them
    QuadDagAnalyzer analyzer;
    QuadDagProfile profile;

    // print the profile of the given QuadDag to the given output stream
    void print(std::ostream& os) const;
};

template <class T>
void QuadDagGenerator<T>::operator()(std::ostream& os) {
    do {
        if (analyzer.check() && profile.generate(analyzer, T::getInstance(), QuadDagVirtualizer::getInstance())) {
            print(os);
        }
    } while (analyzer.next());
    os << "EOF" << "\n";
}

template <class T>
void QuadDagGenerator<T>::print(std::ostream& os) const {
    os << "DAG              " << analyzer.getDag().toString() << "\n";
    os << profile << "END\n\n";
}

}