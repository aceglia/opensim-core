/* -------------------------------------------------------------------------- *
 * OpenSim Muscollo: MucoConstraint.cpp                                       *
 * -------------------------------------------------------------------------- *
 * Copyright (c) 2017 Stanford University and the Authors                     *
 *                                                                            *
 * Author(s): Nicholas Bianco                                                 *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may    *
 * not use this file except in compliance with the License. You may obtain a  *
 * copy of the License at http://www.apache.org/licenses/LICENSE-2.0          *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */

#include "MucoConstraint.h"

using namespace OpenSim;

// ============================================================================
// MucoConstraint
// ============================================================================

MucoConstraint::MucoConstraint() {
    constructProperties();
    if (getName().empty()) setName("constraint");
}

MucoConstraint::MucoConstraint(const std::string& name, 
    const std::vector<MucoBounds>& bounds, 
    const std::vector<std::string>& suffixes) : MucoConstraint() {
    
    setName(name);
    for (int i = 0; i < bounds.size(); ++i) {
        upd_lower_bounds(i) = bounds[i].getLower();
        upd_upper_bounds(i) = bounds[i].getUpper();
    }
    set_suffixes(suffixes);

}

void MucoConstraint::constructProperties() {
    constructProperty_lower_bounds();
    constructProperty_upper_bounds();
    constructProperty_suffixes();
}

void MucoConstraint::calcPositionErrors(const SimTK::State&,
    SimTK::Vector_<double> out) const  
{}

// ============================================================================
// MucoSimbodyConstraint
// ============================================================================


void MucoSimbodyConstraint::initialize(Model& model) const {





}


void MucoSimbodyConstraint::calcAccelerationsFromMultipliers(const Model& model, 
    const SimTK::State& state,
    const SimTK::Vector& multipliers, SimTK::Vector& udot) const {

    const SimTK::MultibodySystem& multibody = model.getMultibodySystem();
    const SimTK::Vector_<SimTK::SpatialVec>& appliedBodyForces =
        multibody.getRigidBodyForces(state, SimTK::Stage::Dynamics);
    const SimTK::Vector& appliedMobilityForces 
        = multibody.getMobilityForces(state, SimTK::Stage::Dynamics);

    const SimTK::SimbodyMatterSubsystem& matter = model.getMatterSubsystem();
    // TODO make these mutable member variables
    SimTK::Vector_<SimTK::SpatialVec> constraintBodyForces;
    SimTK::Vector constraintMobilityForces;
    // Multipliers are negated so constraint forces can be used like applied 
    // forces.
    matter.calcConstraintForcesFromMultipliers(state, -multipliers,
        constraintBodyForces, constraintMobilityForces);

    SimTK::Vector_<SimTK::SpatialVec> A_GB;
    matter.calcAccelerationIgnoringConstraints(state,
        appliedMobilityForces + constraintMobilityForces,
        appliedBodyForces + constraintBodyForces, udot, A_GB);
}