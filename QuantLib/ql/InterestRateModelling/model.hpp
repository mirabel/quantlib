/*
 Copyright (C) 2001, 2002 Sadruddin Rejeb

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/
/*! \file model.hpp
    \brief Abstract interest rate model class

    \fullpath
    ql/InterestRateModelling/%model.hpp
*/

// $Id$

#ifndef quantlib_interest_rate_modelling_model_h
#define quantlib_interest_rate_modelling_model_h

#include <ql/array.hpp>
#include <ql/option.hpp>
#include <ql/termstructure.hpp>
#include <ql/Optimization/optimizer.hpp>

namespace QuantLib {

    namespace InterestRateModelling {

        class CalibrationHelper;

        class Model {
          public:
            enum Type { OneFactor, TwoFactor, Market };

            Model(Size nParams, Type type,
                  const RelinkableHandle<TermStructure>& termStructure)
            : params_(nParams), termStructure_(termStructure) {}
            virtual ~Model() {}

            virtual bool hasDiscountBondFormula() const { return false; }
            virtual double discountBond(
                Time now, 
                Time maturity, 
                Rate r) const { return Null<double>(); }

            virtual bool hasDiscountBondOptionFormula() const { return false; }
            virtual double discountBondOption(
                Option::Type type,
                double strike,
                Time maturity,
                Time bondMaturity) const { return Null<double>(); }

            void calibrate(
                const std::vector<Handle<CalibrationHelper> >& instruments,
                const Handle<Optimization::OptimizationMethod>& method);

            const RelinkableHandle<TermStructure>& termStructure() const {
                return termStructure_;
            }
            Type type() { return type_; }

            const Array& params() { return params_; }
            void setParams(const Array& params) {
                QL_REQUIRE(params.size() == params_.size(),
                    "incorrect parameter vector size");
                params_ = params; 
            }

          protected:
            Handle<Optimization::Constraint> constraint_;
            Array params_;

          private:
            class CalibrationFunction;
            friend class CalibrationFunction;
            const RelinkableHandle<TermStructure>& termStructure_;
            Type type_;
        };

        class ModelTermStructure : public DiscountStructure {
          public:
            ModelTermStructure(const Model& model, Time t0, Rate r0) 
            : model_(model), t0_(t0), r0_(r0) {
                QL_REQUIRE(model_.hasDiscountBondFormula(),
                    "No discount bond formula for this model");
            }

            virtual DiscountFactor discountImpl(Time t, bool extrapolate) {
                return model_.discountBond(t0_, t0_+t, r0_);
            }
          private:
            const Model& model_;
            Time t0_;
            Rate r0_;
        };

    }

}
#endif
