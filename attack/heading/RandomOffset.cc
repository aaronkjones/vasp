/*
 * MIT License
 *
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc., SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Project: V2X Application Spoofing Platform (VASP)
 * Author: Raashid Ansari
 * Email: quic_ransari@quicinc.com
 */

#include <omnetpp/csimulation.h>
#include <omnetpp/distrib.h>
#include <vasp/attack/heading/RandomOffset.h>
#include <vasp/messages/BasicSafetyMessage_m.h>

namespace vasp {
namespace attack {
namespace heading {

void RandomOffset::update(double const offset, veins::Heading const& prevHeading, simtime_t_cref prevBeaconTime)
{
    offset_ = offset;
    prevHeading_ = prevHeading;
    prevBeaconTime_ = prevBeaconTime;
}

void RandomOffset::attack(veins::BasicSafetyMessage* bsm)
{
    auto rng = getEnvir()->getRNG(0);

    if (offset_ > 2 * M_PI or offset_ < -2 * M_PI) {
        return;
    }

    switch (type_) {
    case kHyraTypeHeading: {
        bsm->setAttackType("RandomHeadingOffset");
        // get new randomly offset heading
        bsm->setHeading(
            veins::Heading(fmod(bsm->getHeading().getRad() + uniform(rng, -offset_, offset_), 2 * M_PI)));
        break;
    }
    case kHyraTypeYawRate: {
        bsm->setAttackType("RandomYawRateOffset");
        bsm->setYawRate(fmod(bsm->getYawRate() + uniform(rng, -offset_, offset_), 2 * M_PI));
        break;
    }
    case kHyraTypeBoth: {
        bsm->setAttackType("RandomHeadingYawRateOffset");
        auto const kYawRate{bsm->getYawRate() + uniform(rng, -offset_, offset_)};
        bsm->setYawRate(kYawRate);

        veins::Heading newHeading{getNewHeading(prevHeading_, kYawRate, prevBeaconTime_)};
        bsm->setHeading(newHeading);
        break;
    }
    }
}

} // namespace heading
} // namespace attack
} // namespace vasp